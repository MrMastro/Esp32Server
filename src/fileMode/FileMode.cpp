#include "FileMode.h"

#include <ESPmDNS.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <WebServer.h>
#include <WiFi.h>
#include "FsWebDav.h"

static const char *NVS_NAMESPACE = "filemode";
static const char *KEY_REQUESTED = "requested";
static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 10000;

static bool active = false;
static String ip = "";
static String mdnsHost = "";
static WebServer *davServer = nullptr;

void fileModeRequest()
{
  Preferences prefs;
  if (prefs.begin(NVS_NAMESPACE, false))
  {
    prefs.putBool(KEY_REQUESTED, true);
    prefs.end();
  }
}

bool fileModeConsumeRequest()
{
  Preferences prefs;
  if (!prefs.begin(NVS_NAMESPACE, false))
    return false;
  bool requested = prefs.getBool(KEY_REQUESTED, false);
  if (requested)
    prefs.remove(KEY_REQUESTED); // vale per un solo avvio
  prefs.end();
  return requested;
}

bool fileModeActive()
{
  return active;
}

String fileModeIp()
{
  return ip;
}

String fileModeAddress()
{
  return mdnsHost.length() > 0 ? "http://" + mdnsHost + ".local/" : "http://" + ip + "/";
}

// Nome mDNS dal deviceName (come mdnsHostnameFromDeviceName del telecomando):
// solo lettere minuscole, cifre e '-' ("ESP32_Server" -> "esp32-server")
static String mdnsHostnameFromDeviceName(const String &deviceName)
{
  String name;
  for (size_t i = 0; i < deviceName.length() && name.length() < 63; i++)
  {
    char c = deviceName[i];
    if (isalnum((unsigned char)c))
      name += (char)tolower((unsigned char)c);
    else if ((c == '_' || c == '-' || c == ' ' || c == '.') && name.length() > 0 && !name.endsWith("-"))
      name += '-';
  }
  while (name.endsWith("-"))
    name.remove(name.length() - 1);
  return name.length() > 0 ? name : String("esp32server");
}

static bool connectStation(const String &ssid, const String &password)
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());
  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - start >= WIFI_CONNECT_TIMEOUT_MS)
    {
      WiFi.disconnect(true);
      return false;
    }
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  ip = WiFi.localIP().toString();
  return true;
}

static void startAccessPoint(const String &ssid, const String &password)
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid.c_str(), password.c_str());
  ip = WiFi.softAPIP().toString();
}

static void webDavTask(void *pvParameters)
{
  while (true)
  {
    davServer->handleClient();
    vTaskDelay(5 / portTICK_PERIOD_MS);
  }
}

void fileModeRun(const String &communicationMode, const String &ssidWIFI, const String &passwordWIFI,
                 const String &ssidAP, const String &passwordAP, const String &deviceName)
{
  active = true;
  Serial.println("[FileMode] modalita' file: memoria flash (LittleFS) come unita' di rete WiFi (WebDAV)");

  // Rete come in modalità normale: WIFI_MODE con ripiego in AP, altrimenti AP
  bool connected = false;
  if (communicationMode == "WIFI_MODE")
  {
    Serial.println("[FileMode] connessione a: " + ssidWIFI);
    connected = connectStation(ssidWIFI, passwordWIFI);
    if (!connected)
      Serial.println("[FileMode] WiFi non raggiungibile: ripiego in AP");
  }
  if (!connected)
    startAccessPoint(ssidAP, passwordAP);
  WiFi.setSleep(false); // trasferimenti più veloci; la modalità dura solo fino al riavvio

  if (!LittleFS.begin(false))
    Serial.println("[FileMode] ATTENZIONE: LittleFS non montato (manca l'immagine del file system?)");

  String hostname = mdnsHostnameFromDeviceName(deviceName);
  if (MDNS.begin(hostname.c_str()))
  {
    mdnsHost = hostname;
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("webdav", "tcp", 80);
  }

  davServer = new WebServer(80);
  fsWebDavBegin(*davServer, LittleFS, []() { return (uint64_t)LittleFS.totalBytes(); }, []() { return (uint64_t)LittleFS.usedBytes(); });
  davServer->begin();
  xTaskCreate(webDavTask, "WebDavTaskExecution", 8192, NULL, 2, NULL);

  Serial.println("[FileMode] in Esplora risorse \"Connetti unita' di rete\" -> " + fileModeAddress());
  if (mdnsHost.length() > 0)
    Serial.println("[FileMode] se il nome non viene trovato usa l'IP: http://" + ip + "/");
  Serial.println("[FileMode] per uscire: reboot o stacca la corrente");
}
