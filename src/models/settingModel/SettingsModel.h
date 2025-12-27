#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include <models/LedSettingsModel/LedSettingsModel.h>
#include <models/LedEffectRequest/LedColorRequest.h>
#include <models/JoystickSettingsModel/JoystickSettingsModel.h>

class SettingsModel
{
public:
    void setSettingModel(const SettingsModel &other)
    {
        this->deviceName = other.deviceName;
        this->devicePassword = other.devicePassword;
        this->communicationMode = other.communicationMode;
        this->debug = other.debug;
        this->initialEffect = other.initialEffect;
        this->initialDeltaT = other.initialDeltaT;
        this->initialColors = other.initialColors;
        this->ssidAP = other.ssidAP;
        this->passwordAP = other.passwordAP;
        this->ssidWIFI = other.ssidWIFI;
        this->passwordWIFI = other.passwordWIFI;
        this->ledSettings = other.ledSettings;
        this->joystickSettings = other.joystickSettings;
    };

    String deviceName;
    String devicePassword;
    String communicationMode;
    boolean debug;
    String initialEffect;
    int initialDeltaT;
    std::vector<LedColorRequest> initialColors;
    String ssidAP;
    String passwordAP;
    String ssidWIFI;
    String passwordWIFI;
    LedSettingsModel ledSettings;
    JoystickSettingsModel joystickSettings;

    String toJson() const
    {
        JsonDocument doc;

        doc["deviceName"] = deviceName;
        doc["devicePassword"] = devicePassword;
        doc["communicationMode"] = communicationMode;
        doc["debug"] = debug;
        doc["initialEffect"] = initialEffect;
        doc["initialDeltaT"] = initialDeltaT;

        JsonArray colorsArray = doc["initialColors"].to<JsonArray>();
        for (const auto &color : initialColors)
        {
            JsonObject colorObj = colorsArray.add<JsonObject>();
            colorObj["r"] = color.r;
            colorObj["g"] = color.g;
            colorObj["b"] = color.b;
        }

        doc["ssidAP"] = ssidAP;
        doc["passwordAP"] = passwordAP;
        doc["ssidWIFI"] = ssidWIFI;
        doc["passwordWIFI"] = passwordWIFI;

        JsonObject ledSettingsJson = doc["ledSettings"].to<JsonObject>();
        ledSettings.toJson(ledSettingsJson);

        JsonObject joystickSettingsJson = doc["joystickSetings"].to<JsonObject>();
        joystickSettings.toJson(joystickSettingsJson);

        String output;
        doc.shrinkToFit();
        serializeJson(doc, output);
        return output;
    }

    bool fromJson(const String &json)
    {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        deviceName = doc["deviceName"].as<String>();
        devicePassword = doc["devicePassword"].as<String>();
        communicationMode = doc["communicationMode"].as<String>();
        debug = doc["debug"];
        initialEffect = doc["initialEffect"].as<String>();
        initialDeltaT = doc["initialDeltaT"];

        initialColors.clear();
        JsonArray colorsArray = doc["initialColors"].as<JsonArray>();
        for (JsonObject colorObj : colorsArray)
        {
            LedColorRequest color;
            color.r = colorObj["r"];
            color.g = colorObj["g"];
            color.b = colorObj["b"];
            initialColors.push_back(color);
        }

        ssidAP = doc["ssidAP"].as<String>();
        passwordAP = doc["passwordAP"].as<String>();
        ssidWIFI = doc["ssidWIFI"].as<String>();
        passwordWIFI = doc["passwordWIFI"].as<String>();

        if (!ledSettings.fromJson(doc["ledSettings"].as<String>()))
        {
            return false;
        }

        if (!joystickSettings.fromJson(doc["joystickSettings"].as<String>()))
        {
            return false;
        }

        return true;
    }

    String toString()
    {
        return toJson();
    }

    static SettingsModel getDefault()
    {
        SettingsModel defaultSettings;
        defaultSettings.deviceName = "ESP32_Device";
        defaultSettings.devicePassword = "esp32password";
        defaultSettings.communicationMode = "AP_MODE";
        defaultSettings.debug = false;
        defaultSettings.initialEffect = "NO_EFFECT";
        defaultSettings.initialDeltaT = 100;

        defaultSettings.initialColors.push_back(LedColorRequest());

        defaultSettings.ssidAP = "ESP32_AP";
        defaultSettings.passwordAP = "ap_password";
        defaultSettings.ssidWIFI = "Home_Network";
        defaultSettings.passwordWIFI = "wifi_password";
        defaultSettings.ledSettings = LedSettingsModel::getDefault();

        return defaultSettings;
    }
};

#endif // SETTINGS_MODEL_H