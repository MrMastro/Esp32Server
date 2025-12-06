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

    // Serializzazione nello stesso formato di LedSettings
    void toJson(JsonObject &json) const
    {
        json["enableJoystick"] = enableJoystick;
        json["pinSwitch"] = pinSwitch;
        json["pinAnalogX"] = pinAnalogX;
        json["pinAnalogY"] = pinAnalogY;
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

        return true;
    }
};

#endif // JOYSTICK_SETTINGS_MODEL_H