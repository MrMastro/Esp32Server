#ifndef INFO_ESP_32_MODEL_H
#define INFO_ESP_32_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class InfoEsp32Model
{
public:
    String deviceName;
    String ip;
    String macAdress;

    String toJson() const
    {
        JsonDocument doc;

        doc["deviceName"] = deviceName;
        doc["ip"] = ip;
        doc["macAdress"] = macAdress;

        String output;

        doc.shrinkToFit();  // optional

        serializeJson(doc, output);
        return output;
    }

    // Deserializzazione da JSON
    bool fromJson(const String &json)
    {
        JsonDocument doc;

        DeserializationError error = deserializeJson(doc, json);

        if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return false;
        }

        deviceName = doc["deviceName"].as<String>();
        ip = doc["ip"].as<String>();
        macAdress = doc["macAdress"].as<String>();

        return true;
    }
};

#endif // INFO_ESP_32_MODEL_H