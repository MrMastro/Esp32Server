#include "EspDevices.h"

#include <ArduinoJson.h>
#include <SD.h>
#include <WiFi.h>
#include <lwip/etharp.h>
#include <lwip/tcpip.h>
#include <lwip/priv/tcpip_priv.h> // tcpip_api_call

static const uint16_t ESP_DEVICES_HTTP_PORT = 80;
static const uint32_t ESP_DEVICES_HTTP_TIMEOUT_MS = 7000; // default (hello): l'Esp32Server può impiegare ~5s a rispondere
static const size_t ESP_DEVICES_MAX_RESPONSE = 2048;

static std::vector<EspDevice> devices;
static std::map<String, EspDeviceState> devicesState;
static uint32_t stateVersion = 0;

String espDeviceStatusToString(ESP_DEVICE_STATUS status)
{
  switch (status)
  {
  case ESP_DEVICE_STATUS::ONLINE:
    return "ONLINE";
  case ESP_DEVICE_STATUS::CONNECTED:
    return "CONNECTED";
  default:
    return "OFFLINE";
  }
}

// "#RRGGBB" (o "RRGGBB") -> RGB565; false se il formato non è valido
static bool hexColorTo565(String hex, uint16_t &out)
{
  hex.trim();
  if (hex.startsWith("#"))
    hex = hex.substring(1);
  if (hex.length() != 6)
    return false;
  for (size_t i = 0; i < hex.length(); i++)
  {
    if (!isHexadecimalDigit(hex[i]))
      return false;
  }
  uint32_t rgb = strtoul(hex.c_str(), nullptr, 16);
  uint8_t r = (rgb >> 16) & 0xFF, g = (rgb >> 8) & 0xFF, b = rgb & 0xFF;
  out = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  return true;
}

static String normalizeMac(String mac)
{
  mac.trim();
  mac.toUpperCase();
  return mac;
}

bool espDevicesLoad(const char *path, bool sdAvailable, String *info)
{
  devices.clear();
  devicesState.clear();
  stateVersion++;

  if (!sdAvailable)
  {
    if (info)
      *info = "[Devices] microSD assente: nessun dispositivo conosciuto";
    return false;
  }

  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    if (info)
      *info = String("[Devices] ") + path + " assente: nessun dispositivo conosciuto";
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();
  if (error)
  {
    if (info)
      *info = String("[Devices] ") + path + " non valido: " + error.c_str();
    return false;
  }
  if (!doc.is<JsonArray>())
  {
    if (info)
      *info = String("[Devices] ") + path + " deve contenere una lista [...]";
    return false;
  }

  int skipped = 0;
  int overLimit = 0;
  for (JsonObjectConst item : doc.as<JsonArrayConst>())
  {
    if (devices.size() >= ESP_DEVICES_MAX)
    {
      overLimit++;
      continue;
    }
    EspDevice device;
    device.deviceName = item["deviceName"] | "";
    device.ip = item["ip"] | "";
    device.macAdress = normalizeMac(item["macAdress"] | "");
    device.color = item["color"] | "";
    device.devicePassword = item["devicePassword"] | "";
    if (!hexColorTo565(device.color, device.color565))
      device.color565 = 0xFFFF; // colore mancante o non valido: bianco
    if (device.ip.length() == 0 || device.macAdress.length() == 0)
    {
      skipped++;
      continue; // senza ip o macAdress il dispositivo non è verificabile
    }
    devices.push_back(device);

    EspDeviceState state;
    state.deviceName = device.deviceName;
    state.ip = device.ip;
    state.macAdress = device.macAdress;
    state.color = device.color;
    state.color565 = device.color565;
    state.error = "mai controllato";
    devicesState[device.macAdress] = state;
  }

  if (info)
  {
    *info = "[Devices] dispositivi conosciuti: " + String(devices.size());
    if (skipped > 0)
      *info += " (" + String(skipped) + " scartati: ip o macAdress mancanti)";
    if (overLimit > 0)
      *info += " (" + String(overLimit) + " ignorati: massimo " + String(ESP_DEVICES_MAX) + ")";
  }
  return true;
}

const std::vector<EspDevice> &espDevicesList()
{
  return devices;
}

const std::map<String, EspDeviceState> &espDevicesState()
{
  return devicesState;
}

// Richiesta minimale su HTTP in chiaro (niente HTTPClient: si porterebbe dietro il TLS e
// appesantirebbe il firmware, che deve restare sotto lo slot OTA).
// method "GET" (requestBody ignorato) o "POST" (requestBody JSON).
static bool httpRequest(const char *method, const String &ip, const char *path, const String &requestBody,
                        uint32_t timeoutMs, int &statusCode, String &body, String &error)
{
  IPAddress address;
  if (!address.fromString(ip))
  {
    error = "ip non valido";
    return false;
  }

  WiFiClient client;
  uint32_t connectStart = millis();
  if (!client.connect(address, ESP_DEVICES_HTTP_PORT, timeoutMs))
  {
    // errno qui non è affidabile (resta EINPROGRESS): conta il tempo trascorso
    uint32_t elapsed = millis() - connectStart;
    if (elapsed + 50 >= timeoutMs)
      error = "nessuna risposta in " + String(elapsed) + "ms (pacchetti non arrivano o non tornano)";
    else
      error = "connessione rifiutata in " + String(elapsed) + "ms (raggiungibile, porta chiusa)";
    return false;
  }

  bool post = strcmp(method, "POST") == 0;
  String request = String(method) + " " + path + " HTTP/1.1\r\n" +
                   "Host: " + ip + "\r\n" +
                   "Connection: close\r\n";
  if (post)
    request += "Content-Type: application/json\r\nContent-Length: " + String(requestBody.length()) + "\r\n";
  request += "\r\n";
  if (post)
    request += requestBody; // un'unica scrittura: il server riceve il body in un solo pezzo
  client.print(request);

  // Si legge finché: la risposta è completa (header + Content-Length byte di body),
  // il server chiude la connessione, o scade il timeout.
  String response;
  int headerEnd = -1;
  long contentLength = -1;
  uint32_t start = millis();
  while (millis() - start < timeoutMs)
  {
    while (client.available() && response.length() < ESP_DEVICES_MAX_RESPONSE)
    {
      response += (char)client.read();
    }

    if (headerEnd < 0)
    {
      headerEnd = response.indexOf("\r\n\r\n");
      if (headerEnd >= 0)
      {
        String headers = response.substring(0, headerEnd);
        headers.toLowerCase();
        int cl = headers.indexOf("content-length:");
        if (cl >= 0)
          contentLength = headers.substring(cl + 15, headers.indexOf('\r', cl)).toInt();
      }
    }
    if (headerEnd >= 0 && contentLength >= 0 &&
        (long)response.length() >= headerEnd + 4 + contentLength)
      break; // risposta completa

    if (!client.connected() && !client.available())
      break;
    delay(5);
  }
  client.stop();

  // "HTTP/1.1 200 OK\r\n...headers...\r\n\r\nbody"
  if (!response.startsWith("HTTP/"))
  {
    error = response.length() == 0 ? "nessuna risposta" : "risposta non HTTP";
    return false;
  }
  int space = response.indexOf(' ');
  statusCode = response.substring(space + 1, space + 4).toInt();
  int bodyStart = response.indexOf("\r\n\r\n");
  body = bodyStart >= 0 ? response.substring(bodyStart + 4) : "";
  return true;
}

static bool httpGet(const String &ip, const char *path, int &statusCode, String &body, String &error)
{
  return httpRequest("GET", ip, path, "", ESP_DEVICES_HTTP_TIMEOUT_MS, statusCode, body, error);
}

// ARP verso un dispositivo (eseguito nel thread di lwIP con tcpip_api_call)
struct ArpCall
{
  struct tcpip_api_call_data base; // deve essere il primo campo
  ip4_addr_t ip;
  struct eth_addr mac;
  bool unicastSent;
};

// Richiesta ARP "chi ha <ip>?" inviata in unicast al MAC del dispositivo invece che in broadcast:
// il dispositivo la riceve come un pacchetto normale anche in modem sleep, impara il MAC del telecomando
// (una richiesta indirizzata a lui aggiorna la sua tabella ARP) e risponde in unicast.
static bool sendUnicastArpRequest(struct netif *netif, const ip4_addr_t *ip, const struct eth_addr *mac)
{
  const uint16_t frameLen = 14 + 28; // header Ethernet + pacchetto ARP
  struct pbuf *p = pbuf_alloc(PBUF_RAW, frameLen, PBUF_RAM);
  if (p == nullptr)
    return false;
  uint8_t *f = (uint8_t *)p->payload;
  const uint8_t *ownMac = netif->hwaddr;
  const ip4_addr_t *ownIp = netif_ip4_addr(netif);
  // Ethernet: destinazione = dispositivo, sorgente = telecomando, tipo ARP
  memcpy(f + 0, mac->addr, 6);
  memcpy(f + 6, ownMac, 6);
  f[12] = 0x08;
  f[13] = 0x06;
  // ARP: Ethernet/IPv4, richiesta
  uint8_t *a = f + 14;
  a[0] = 0x00; a[1] = 0x01; // hardware type: Ethernet
  a[2] = 0x08; a[3] = 0x00; // protocol type: IPv4
  a[4] = 6;                 // hardware size
  a[5] = 4;                 // protocol size
  a[6] = 0x00; a[7] = 0x01; // opcode: request
  memcpy(a + 8, ownMac, 6);
  memcpy(a + 14, &ownIp->addr, 4);
  memcpy(a + 18, mac->addr, 6);
  memcpy(a + 24, &ip->addr, 4);
  err_t err = netif->linkoutput(netif, p);
  pbuf_free(p);
  return err == ERR_OK;
}

static err_t prepareArpInTcpip(struct tcpip_api_call_data *data)
{
  ArpCall *call = (ArpCall *)data;
  err_t err = etharp_add_static_entry(&call->ip, &call->mac);
  if (err != ERR_OK)
    return err;
  struct netif *netif = netif_default;
  call->unicastSent = netif != nullptr && netif_is_up(netif) && netif->linkoutput != nullptr &&
                      sendUnicastArpRequest(netif, &call->ip, &call->mac);
  return ERR_OK;
}

bool espDevicesPrepareArp(const String &ip, const String &mac, String &error)
{
  IPAddress address;
  if (!address.fromString(ip))
  {
    error = "ip non valido";
    return false;
  }
  unsigned int b[6];
  if (sscanf(mac.c_str(), "%x:%x:%x:%x:%x:%x", &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) != 6)
  {
    error = "macAdress non valido";
    return false;
  }
  ArpCall call = {};
  IP4_ADDR(&call.ip, address[0], address[1], address[2], address[3]);
  for (int i = 0; i < 6; i++)
    call.mac.addr[i] = (uint8_t)b[i];
  err_t err = tcpip_api_call(prepareArpInTcpip, &call.base);
  if (err != ERR_OK)
  {
    error = "etharp_add_static_entry: errore " + String(err);
    return false;
  }
  if (!call.unicastSent)
  {
    error = "voce statica impostata, richiesta ARP unicast non inviata";
    return false;
  }
  return true;
}

bool espDevicesHttpPost(const String &ip, const char *path, const String &jsonBody, uint32_t timeoutMs,
                        int &statusCode, String &responseBody, String &error)
{
  return httpRequest("POST", ip, path, jsonBody, timeoutMs, statusCode, responseBody, error);
}

void espDevicesSetState(const EspDeviceState &state)
{
  EspDeviceState &current = devicesState[state.macAdress];
  if (current.status != state.status)
    stateVersion++;
  current = state;
}

String espDevicesProbe(const String &ip, const char *path)
{
  int statusCode = 0;
  String body, error;
  uint32_t start = millis();
  if (!httpGet(ip, path, statusCode, body, error))
    return "ERRORE " + error;
  return "HTTP " + String(statusCode) + " in " + String(millis() - start) + "ms: " + body.substring(0, 200);
}

uint32_t espDevicesStateVersion()
{
  return stateVersion;
}

String espDevicesStateToJson()
{
  JsonDocument doc;
  JsonArray list = doc.to<JsonArray>();
  for (const auto &entry : devicesState)
  {
    const EspDeviceState &state = entry.second;
    JsonObject item = list.add<JsonObject>();
    item["deviceName"] = state.deviceName;
    item["ip"] = state.ip;
    item["macAdress"] = state.macAdress;
    item["color"] = state.color;
    item["status"] = espDeviceStatusToString(state.status);
    if (state.error.length() > 0)
      item["error"] = state.error;
    item["paired"] = state.paired;
    if (state.pairError.length() > 0)
      item["pairError"] = state.pairError;
  }
  String out;
  serializeJsonPretty(doc, out);
  return out;
}
