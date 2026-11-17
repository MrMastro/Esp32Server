#include "EspDevices.h"

#include <ArduinoJson.h>
#include <SD.h>
#include <WiFi.h>

static const uint16_t ESP_DEVICES_HTTP_PORT = 80;
static const uint32_t ESP_DEVICES_HTTP_TIMEOUT_MS = 7000; // l'Esp32Server può impiegare ~5s a rispondere
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
                        int &statusCode, String &body, String &error)
{
  IPAddress address;
  if (!address.fromString(ip))
  {
    error = "ip non valido";
    return false;
  }

  WiFiClient client;
  uint32_t connectStart = millis();
  if (!client.connect(address, ESP_DEVICES_HTTP_PORT, ESP_DEVICES_HTTP_TIMEOUT_MS))
  {
    // errno qui non è affidabile (resta EINPROGRESS): conta il tempo trascorso
    uint32_t elapsed = millis() - connectStart;
    if (elapsed + 50 >= ESP_DEVICES_HTTP_TIMEOUT_MS)
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
  while (millis() - start < ESP_DEVICES_HTTP_TIMEOUT_MS)
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
  return httpRequest("GET", ip, path, "", statusCode, body, error);
}

bool espDevicesHttpPost(const String &ip, const char *path, const String &jsonBody, int &statusCode, String &responseBody, String &error)
{
  return httpRequest("POST", ip, path, jsonBody, statusCode, responseBody, error);
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
