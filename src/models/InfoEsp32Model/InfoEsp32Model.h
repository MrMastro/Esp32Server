#ifndef INFO_ESP_32_MODEL_H
#define INFO_ESP_32_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class InfoEsp32Model
{
public:
    String deviceName;
    String ip;

    String toJson() const
    {
        StaticJsonDocument<256> doc;

        doc["deviceName"] = deviceName;
        doc["ip"] = ip;

        String output;
        serializeJson(doc, output);
        return output;
    }

    // Deserializzazione da JSON
    bool fromJson(const String &json)
    {
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        deviceName = doc["deviceName"].as<String>();
        ip = doc["ip"].as<String>();

        return true;
    }
};

#endif // INFO_ESP_32_MODEL_H