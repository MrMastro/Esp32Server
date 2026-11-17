#include "Controllers.h"
#include "services/ServiceImplementations/SettingService.h"
#include "wireless/WirelessProtocol.h"
#include "wireless/RemotePairing.h"
#include <WiFi.h>
#include <esp_wifi.h>

static const size_t PAIR_MAX_BODY = 512;

static void sendPairResponse(AsyncWebServerRequest *request, int code, const String &message)
{
    JsonDocument doc;
    doc["code"] = code;
    doc["message"] = message;
    String body;
    serializeJson(doc, body);
    request->send(code, "application/json", body);
}

// POST /api/v1/remote/pair (fase 1 del joystick wireless)
// Richiesta: { "deviceName", "devicePassword", "remoteMac", "remoteNonce" (32 hex), "protocolVersion" }
// Risposta 200: { "deviceNonce" (32 hex), "channel", "macAdress", "protocolVersion" }
// Errori: 400 richiesta non valida, 401 credenziali errate, 403 remoteControlWireless disattivato,
//         409 versione di protocollo non supportata
void remotePair(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    // il body arriva in un solo pezzo (richiesta piccola): niente ricomposizione
    if (index != 0 || len != total || total > PAIR_MAX_BODY)
    {
        sendPairResponse(request, 400, "body troppo grande o frammentato");
        return;
    }

    servicesCollector.takeExclusiveExecution();
    SettingsModel settings = ((SettingService *)servicesCollector.getService("SettingService"))->getSettings();

    if (!settings.joystickSettings.remoteControlWireless)
    {
        sendPairResponse(request, 403, "remoteControlWireless disattivato su questo dispositivo");
        servicesCollector.freeExclusiveExecution();
        return;
    }

    JsonDocument req;
    if (deserializeJson(req, data, len))
    {
        sendPairResponse(request, 400, "JSON non valido");
        servicesCollector.freeExclusiveExecution();
        return;
    }

    if (!req["protocolVersion"].is<int>() || !req["deviceName"].is<const char *>() || !req["devicePassword"].is<const char *>() ||
        !req["remoteMac"].is<const char *>() || !req["remoteNonce"].is<const char *>())
    {
        sendPairResponse(request, 400, "campi mancanti: deviceName, devicePassword, remoteMac, remoteNonce, protocolVersion");
        servicesCollector.freeExclusiveExecution();
        return;
    }

    int version = req["protocolVersion"];
    if (version != WIRELESS_PROTOCOL_VERSION)
    {
        sendPairResponse(request, 409, "protocolVersion " + String(version) + " non supportata (supportata: " + String(WIRELESS_PROTOCOL_VERSION) + ")");
        servicesCollector.freeExclusiveExecution();
        return;
    }

    // stessa verifica del login
    boolean authorized = ((InfoService *)servicesCollector.getService("InfoService"))->loginValidate(req["deviceName"].as<String>(), req["devicePassword"].as<String>());
    if (!authorized)
    {
        if (settings.debug)
            Serial.println("[Wireless] accoppiamento rifiutato: credenziali errate (" + req["remoteMac"].as<String>() + ")");
        sendPairResponse(request, 401, "deviceName o devicePassword errati");
        servicesCollector.freeExclusiveExecution();
        return;
    }

    RemotePairing pairing;
    uint8_t remoteNonce[WIRELESS_NONCE_LEN];
    uint8_t deviceNonce[WIRELESS_NONCE_LEN];
    uint8_t deviceMac[WIRELESS_MAC_LEN];
    if (!wirelessParseMac(req["remoteMac"].as<String>(), pairing.remoteMac) ||
        !wirelessFromHex(req["remoteNonce"].as<String>(), remoteNonce, WIRELESS_NONCE_LEN))
    {
        sendPairResponse(request, 400, "remoteMac (AA:BB:CC:DD:EE:FF) o remoteNonce (32 hex) non validi");
        servicesCollector.freeExclusiveExecution();
        return;
    }

    WiFi.macAddress(deviceMac);
    wirelessRandomBytes(deviceNonce, WIRELESS_NONCE_LEN);
    if (!wirelessDeriveSessionKey(settings.devicePassword, remoteNonce, deviceNonce, pairing.remoteMac, deviceMac, pairing.sessionKey))
    {
        sendPairResponse(request, 500, "calcolo della chiave di sessione fallito");
        servicesCollector.freeExclusiveExecution();
        return;
    }
    pairing.valid = true;
    pairing.pairedMs = millis();
    remotePairingSet(pairing);

    uint8_t channel = 0;
    wifi_second_chan_t second;
    esp_wifi_get_channel(&channel, &second);

    JsonDocument res;
    res["deviceNonce"] = wirelessToHex(deviceNonce, WIRELESS_NONCE_LEN);
    res["channel"] = channel;
    res["macAdress"] = wirelessMacToString(deviceMac);
    res["protocolVersion"] = WIRELESS_PROTOCOL_VERSION;
    String body;
    serializeJson(res, body);
    request->send(200, "application/json", body);

    if (settings.debug)
        Serial.println("[Wireless] telecomando " + wirelessMacToString(pairing.remoteMac) + " accoppiato, canale " + String(channel) +
                   ", impronta chiave " + wirelessKeyFingerprint(pairing.sessionKey));
    servicesCollector.freeExclusiveExecution();
}
