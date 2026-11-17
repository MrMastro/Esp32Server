#include "WirelessPairing.h"
#include "WirelessLink.h"
#include <ArduinoJson.h>
#include <WiFi.h>

static std::map<String, WirelessSession> sessions;

PAIR_RESULT wirelessPair(const EspDevice &device, WirelessSession &session, String &error)
{
  if (device.devicePassword.length() == 0)
  {
    error = "devicePassword mancante in espDevices.json";
    return PAIR_RESULT::NOT_CONFIGURED;
  }

  uint8_t remoteMac[WIRELESS_MAC_LEN];
  uint8_t remoteNonce[WIRELESS_NONCE_LEN];
  WiFi.macAddress(remoteMac);
  wirelessRandomBytes(remoteNonce, WIRELESS_NONCE_LEN);

  JsonDocument req;
  req["deviceName"] = device.deviceName;
  req["devicePassword"] = device.devicePassword;
  req["remoteMac"] = wirelessMacToString(remoteMac);
  req["remoteNonce"] = wirelessToHex(remoteNonce, WIRELESS_NONCE_LEN);
  req["protocolVersion"] = WIRELESS_PROTOCOL_VERSION;
  String body;
  serializeJson(req, body);

  int statusCode = 0;
  String response;
  if (!espDevicesHttpPost(device.ip, WIRELESS_PAIR_PATH, body, statusCode, response, error))
    return PAIR_RESULT::UNREACHABLE;

  if (statusCode != 200)
  {
    JsonDocument err;
    String message = deserializeJson(err, response) ? String("") : String(err["message"] | "");
    switch (statusCode)
    {
    case 401:
      error = "deviceName o devicePassword errati (401)";
      break;
    case 403:
      error = "remoteControlWireless disattivato sul dispositivo (403)";
      break;
    case 404:
      error = "il firmware del dispositivo non ha " WIRELESS_PAIR_PATH " (404)";
      break;
    case 409:
      error = "versione di protocollo non supportata (409)";
      break;
    default:
      error = "HTTP " + String(statusCode);
      break;
    }
    if (message.length() > 0)
      error += ": " + message;
    return PAIR_RESULT::REJECTED;
  }

  JsonDocument res;
  if (deserializeJson(res, response))
  {
    error = "risposta JSON non valida";
    return PAIR_RESULT::REJECTED;
  }
  uint8_t deviceNonce[WIRELESS_NONCE_LEN];
  if (!wirelessFromHex(res["deviceNonce"] | "", deviceNonce, WIRELESS_NONCE_LEN) ||
      !wirelessParseMac(res["macAdress"] | "", session.deviceMac))
  {
    error = "deviceNonce o macAdress non validi nella risposta";
    return PAIR_RESULT::REJECTED;
  }
  // all'ip indicato deve rispondere proprio quel dispositivo
  if (!wirelessMacToString(session.deviceMac).equalsIgnoreCase(device.macAdress))
  {
    error = "macAdress diverso: " + wirelessMacToString(session.deviceMac);
    return PAIR_RESULT::REJECTED;
  }
  if ((res["protocolVersion"] | 0) != WIRELESS_PROTOCOL_VERSION)
  {
    error = "protocolVersion della risposta non supportata";
    return PAIR_RESULT::REJECTED;
  }

  if (!wirelessDeriveSessionKey(device.devicePassword, remoteNonce, deviceNonce, remoteMac, session.deviceMac, session.sessionKey))
  {
    error = "calcolo della chiave di sessione fallito";
    return PAIR_RESULT::REJECTED;
  }
  session.deviceName = device.deviceName;
  session.channel = res["channel"] | 0;
  session.pairedMs = millis();
  return PAIR_RESULT::PAIRED;
}

void wirelessSessionSet(const WirelessSession &session)
{
  sessions[wirelessMacToString(session.deviceMac)] = session; // un nuovo accoppiamento sostituisce il precedente
  wirelessLinkAddPeer(session.deviceMac);
}

const std::map<String, WirelessSession> &wirelessSessions()
{
  return sessions;
}

std::map<String, WirelessSession> &wirelessSessionsMutable()
{
  return sessions;
}

void wirelessSessionRemove(const String &deviceMac)
{
  auto it = sessions.find(deviceMac);
  if (it == sessions.end())
    return;
  wirelessLinkRemovePeer(it->second.deviceMac);
  sessions.erase(it);
}
