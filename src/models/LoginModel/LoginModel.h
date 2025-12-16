#ifndef LOGIN_MODEL_H
#define LOGIN_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class LoginModel
{
public:
    String deviceName;
    String devicePassword;

    String toJson() const
    {
        JsonDocument doc;

        doc["deviceName"] = deviceName;
        doc["devicePassword"] = devicePassword;

        String output;
        serializeJson(doc, output);
        return output;
    }

    // Deserializzazione da JSON
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

        return true;
    }
};

#endif // LOGIN_MODEL_H