#include "WifiConnection.h"
#include "system/Log.h"

#include <WiFi.h>

static const uint32_t WIFI_CONNECT_TIMEOUT_MS = 10000;

static WifiStatus startAp(const SettingsModel &settings)
{
  WifiStatus status;
  logInfo("[WiFi] avvio access point: " + settings.ssidAP);

  if (settings.passwordAP.length() > 0 && settings.passwordAP.length() < 8)
  {
    logWarning("[WiFi] ATTENZIONE: passwordAP deve avere almeno 8 caratteri, l'AP potrebbe non partire");
  }

  // AP + STA come nel progetto principale (lascia la possibilità di cercare reti)
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPsetHostname(settings.deviceName.c_str());
  if (!WiFi.softAP(settings.ssidAP.c_str(), settings.passwordAP.c_str()))
  {
    logWarning("[WiFi] softAP() fallito");
  }

  status.activeMode = COMMUNICATION_MODE::AP_MODE;
  status.ssid = settings.ssidAP;
  status.ip = WiFi.softAPIP().toString();
  logInfo("[WiFi] AP attivo, IP: " + status.ip);
  return status;
}

static bool connectStation(const SettingsModel &settings, WifiStatus &status)
{
  logInfo("[WiFi] connessione a: " + settings.ssidWIFI);

  WiFi.mode(WIFI_STA);
  // Niente modem sleep (risparmio energetico WiFi): con il modem sleep il C6 non riusciva a
  // comunicare direttamente con gli altri ESP32 della rete (anch'essi in modem sleep), mentre
  // dal PC funzionava. Costa qualche decina di mA in più.
  WiFi.setSleep(false);
  WiFi.setHostname(settings.deviceName.c_str());
  WiFi.begin(settings.ssidWIFI.c_str(), settings.passwordWIFI.c_str());

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    if (millis() - start >= WIFI_CONNECT_TIMEOUT_MS)
    {
      logWarning("\n[WiFi] timeout di connessione");
      WiFi.disconnect(true);
      return false;
    }
    delay(250);
    logInfoRaw(".");
  }

  status.activeMode = COMMUNICATION_MODE::WIFI_MODE;
  status.ssid = settings.ssidWIFI;
  status.ip = WiFi.localIP().toString();
  logInfo("\n[WiFi] connesso, IP: " + status.ip);
  return true;
}

NETWORK_STATE wifiCurrentState(const WifiStatus &status)
{
  switch (status.activeMode)
  {
  case COMMUNICATION_MODE::AP_MODE:
    return NETWORK_STATE::AP;
  case COMMUNICATION_MODE::WIFI_MODE:
    return WiFi.status() == WL_CONNECTED ? NETWORK_STATE::WIFI_CONNECTED : NETWORK_STATE::WIFI_DISCONNECTED;
  default:
    return NETWORK_STATE::OFF;
  }
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
    logWarning("[WiFi] WIFI_MODE fallito: passo in AP_MODE");
    status = startAp(settings);
    status.fallbackToAp = true;
    return status;
  }

  case COMMUNICATION_MODE::AP_MODE:
    return startAp(settings);

  default:
    logWarning("[WiFi] communicationMode sconosciuta (" + settings.communicationMode + "): uso AP_MODE");
    return startAp(settings);
  }
}
