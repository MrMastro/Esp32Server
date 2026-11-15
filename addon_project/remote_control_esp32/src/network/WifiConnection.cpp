#include "WifiConnection.h"

#include <WiFi.h>

static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 10000;

static WifiStatus startAp(const SettingsModel &settings)
{
  WifiStatus status;
  Serial.println("[WiFi] avvio access point: " + settings.ssidAP);

  if (settings.passwordAP.length() > 0 && settings.passwordAP.length() < 8)
  {
    Serial.println("[WiFi] ATTENZIONE: passwordAP deve avere almeno 8 caratteri, l'AP potrebbe non partire");
  }

  // AP + STA come nel progetto principale (lascia la possibilità di cercare reti)
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPsetHostname(settings.deviceName.c_str());
  if (!WiFi.softAP(settings.ssidAP.c_str(), settings.passwordAP.c_str()))
  {
    Serial.println("[WiFi] softAP() fallito");
  }

  status.activeMode = COMMUNICATION_MODE::AP_MODE;
  status.ssid = settings.ssidAP;
  status.ip = WiFi.softAPIP().toString();
  Serial.println("[WiFi] AP attivo, IP: " + status.ip);
  return status;
}

static bool connectStation(const SettingsModel &settings, WifiStatus &status)
{
  Serial.println("[WiFi] connessione a: " + settings.ssidWIFI);

  WiFi.mode(WIFI_STA);
  WiFi.setHostname(settings.deviceName.c_str());
  WiFi.begin(settings.ssidWIFI.c_str(), settings.passwordWIFI.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - start >= WIFI_CONNECT_TIMEOUT_MS)
    {
      Serial.println("\n[WiFi] timeout di connessione");
      WiFi.disconnect(true);
      return false;
    }
    delay(250);
    Serial.print(".");
  }

  status.activeMode = COMMUNICATION_MODE::WIFI_MODE;
  status.ssid = settings.ssidWIFI;
  status.ip = WiFi.localIP().toString();
  Serial.println("\n[WiFi] connesso, IP: " + status.ip);
  return true;
}

WifiStatus wifiBegin(const SettingsModel &settings)
{
  switch (communicationModeStringToEnum(settings.communicationMode))
  {
  case COMMUNICATION_MODE::WIFI_MODE:
  {
    WifiStatus status;
    if (connectStation(settings, status))
    {
      return status;
    }
    Serial.println("[WiFi] WIFI_MODE fallito: passo in AP_MODE");
    status = startAp(settings);
    status.fallbackToAp = true;
    return status;
  }

  case COMMUNICATION_MODE::AP_MODE:
    return startAp(settings);

  default:
    Serial.println("[WiFi] communicationMode sconosciuta (" + settings.communicationMode + "): uso AP_MODE");
    return startAp(settings);
  }
}
