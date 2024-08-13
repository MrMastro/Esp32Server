#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class SettingsModel
{
public:
    //---- DEVICE SETTINGS -------
    String deviceName;
    String devicePassword;
    boolean debug;

    //---- Initial Operation -------
    String initialEffect;
    String initialDeltaT;
    String initialR;
    String initialG;
    String initialB;

    //---- WIRELESS SETTINGS -------
    String wirelessMode; // AP or WIFI

    //-- SETTINGS FOR AP MODE
    String ssidAP;
    String passwordAP;

    //-- SETTINGS FOR WIFI MODE
    String ssidWIFI;
    String passwordWIFI;

    String toJson() const
    {
        StaticJsonDocument<512> doc;

        doc["deviceName"] = deviceName;
        doc["devicePassword"] = devicePassword;
        doc["debug"] = debug;
        doc["initialEffect"] = initialEffect;
        doc["wirelessMode"] = wirelessMode;
        doc["ssidAP"] = ssidAP;
        doc["passwordAP"] = passwordAP;
        doc["ssidWIFI"] = ssidWIFI;
        doc["passwordWIFI"] = passwordWIFI;
        doc["initialDeltaT"] = initialDeltaT;
        doc["initialR"] = initialR;
        doc["initialG"] = initialG;
        doc["initialB"] = initialB;

        String output;
        serializeJson(doc, output);
        return output;
    }

    bool fromJson(const String &json)
    {
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        deviceName = doc["deviceName"].as<String>();
        devicePassword = doc["devicePassword"].as<String>();
        debug = doc["debug"];
        initialEffect = doc["initialEffect"].as<String>();
        wirelessMode = doc["wirelessMode"].as<String>();
        ssidAP = doc["ssidAP"].as<String>();
        passwordAP = doc["passwordAP"].as<String>();
        ssidWIFI = doc["ssidWIFI"].as<String>();
        passwordWIFI = doc["passwordWIFI"].as<String>();
        initialDeltaT = doc["initialDeltaT"].as<String>();
        initialR = doc["initialR"].as<String>();
        initialG = doc["initialG"].as<String>();
        initialB = doc["initialB"].as<String>();
        return true;
    }

    private:
    
};

#endif // SETTINGS_MODEL_H