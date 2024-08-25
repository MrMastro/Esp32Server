#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <models/LedSettingsModel/LedSettingsModel.h>

class SettingsModel
{
public:
    //---- DEVICE SETTINGS -------
    String deviceName;
    String devicePassword;
    boolean debug;

    //---- Initial Operation -------
    String initialEffect;
    int initialDeltaT;
    int initialR;
    int initialG;
    int initialB;

    //---- WIRELESS SETTINGS -------
    String wirelessMode; // AP or WIFI

    //-- SETTINGS FOR AP MODE
    String ssidAP;
    String passwordAP;

    //-- SETTINGS FOR WIFI MODE
    String ssidWIFI;
    String passwordWIFI;

    //-- SETTINGS LED PIN
    LedSettingsModel ledSettings;

    // Serializzazione in JSON
    String toJson() const {
        StaticJsonDocument<768> doc;

        doc["deviceName"] = deviceName;
        doc["devicePassword"] = devicePassword;
        doc["debug"] = debug;
        doc["initialEffect"] = initialEffect;
        doc["initialDeltaT"] = initialDeltaT;
        doc["initialR"] = initialR;
        doc["initialG"] = initialG;
        doc["initialB"] = initialB;
        doc["wirelessMode"] = wirelessMode;
        doc["ssidAP"] = ssidAP;
        doc["passwordAP"] = passwordAP;
        doc["ssidWIFI"] = ssidWIFI;
        doc["passwordWIFI"] = passwordWIFI;

        // Serializzazione diretta delle impostazioni LED nel documento JSON
        JsonObject ledSettingsJson = doc.createNestedObject("ledSettings");
        ledSettings.toJson(ledSettingsJson);

        String output;
        serializeJson(doc, output);
        return output;
    }

    // Deserializzazione da JSON
    bool fromJson(const String& json) {
        StaticJsonDocument<768> doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        deviceName = doc["deviceName"].as<String>();
        devicePassword = doc["devicePassword"].as<String>();
        debug = doc["debug"];
        initialEffect = doc["initialEffect"].as<String>();
        initialDeltaT = doc["initialDeltaT"];
        initialR = doc["initialR"];
        initialG = doc["initialG"];
        initialB = doc["initialB"];
        wirelessMode = doc["wirelessMode"].as<String>();
        ssidAP = doc["ssidAP"].as<String>();
        passwordAP = doc["passwordAP"].as<String>();
        ssidWIFI = doc["ssidWIFI"].as<String>();
        passwordWIFI = doc["passwordWIFI"].as<String>();

        // Deserializzazione delle impostazioni LED
        if (!ledSettings.fromJson(doc["ledSettings"].as<String>())) {
            return false;
        }

        return true;
    }

    // Metodo statico per ottenere un'istanza di SettingsModel con valori di default
    static SettingsModel getDefault() {
        SettingsModel defaultSettings;
        defaultSettings.deviceName = "ESP32_Device";
        defaultSettings.devicePassword = "esp32password";
        defaultSettings.debug = false;
        defaultSettings.initialEffect = "NO_EFFECT";
        defaultSettings.initialDeltaT = 100;
        defaultSettings.initialR = 0;
        defaultSettings.initialG = 0;
        defaultSettings.initialB = 0;
        defaultSettings.wirelessMode = "AP";
        defaultSettings.ssidAP = "ESP32_AP";
        defaultSettings.passwordAP = "ap_password";
        defaultSettings.ssidWIFI = "Home_Network";
        defaultSettings.passwordWIFI = "wifi_password";
        defaultSettings.ledSettings = LedSettingsModel::getDefault();

        return defaultSettings;
    }

    
};

#endif // SETTINGS_MODEL_H