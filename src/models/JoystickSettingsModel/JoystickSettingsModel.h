#ifndef JOYSTICK_SETTINGS_MODEL_H
#define JOYSTICK_SETTINGS_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class JoystickSettingsModel
{
public:
    bool enableJoystick;
    int pinSwitch;
    int pinAnalogX;
    int pinAnalogY;
    // true: comandi dal telecomando wireless (ESP-NOW); il joystick fisico e' disabilitato e i suoi pin non vengono toccati
    bool remoteControlWireless = false;

    // Joystick fisico attivo solo se abilitato e non in modalita' wireless
    bool usePhysicalJoystick() const
    {
        return enableJoystick && !remoteControlWireless;
    }

    // Serializzazione nello stesso formato di LedSettings
    void toJson(JsonObject &json) const
    {
        json["enableJoystick"] = enableJoystick;
        json["pinSwitch"] = pinSwitch;
        json["pinAnalogX"] = pinAnalogX;
        json["pinAnalogY"] = pinAnalogY;
        json["remoteControlWireless"] = remoteControlWireless;
    }

    // Deserializzazione dallo stesso stile di LedSettings
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

        enableJoystick = doc["enableJoystick"];
        pinSwitch = doc["pinSwitch"];
        pinAnalogX = doc["pinAnalogX"];
        pinAnalogY = doc["pinAnalogY"];
        remoteControlWireless = doc["remoteControlWireless"] | false; // assente nei vecchi file -> false

        return true;
    }
};

#endif // JOYSTICK_SETTINGS_MODEL_H