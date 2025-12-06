#ifndef LED_PRESET_MODEL_H
#define LED_PRESET_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "LedColor.h"

class LedPresetModel
{
public:
    String effect;
    int deltaT;
    std::vector<String> triggers;
    std::vector<LedColor> colors;

    // ---------------------------------------
    // Serializzazione in oggetto JSON
    // ---------------------------------------
    void toJson(JsonObject &json) const
    {
        json["effect"] = effect;
        json["deltaT"] = deltaT;

        JsonArray triggersArray = json.createNestedArray("triggers");
        for (const auto &t : triggers)
            triggersArray.add(t);

        JsonArray colorsArray = json.createNestedArray("colors");
        for (const auto &c : colors)
        {
            JsonObject o = colorsArray.createNestedObject();
            o["r"] = c.r;
            o["g"] = c.g;
            o["b"] = c.b;
        }
    }

    // ---------------------------------------
    // Deserializzazione da stringa JSON
    // ---------------------------------------
    bool fromJson(const String &json)
    {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        JsonObject obj = doc.as<JsonObject>();

        if (!obj["effect"] || !obj["deltaT"] || !obj["triggers"] || !obj["colors"])
            return false;

        effect = obj["effect"].as<String>();
        deltaT = obj["deltaT"];

        triggers.clear();
        for (JsonVariant v : obj["triggers"].as<JsonArray>())
            triggers.push_back(v.as<String>());

        colors.clear();
        for (JsonObject cObj : obj["colors"].as<JsonArray>())
        {
            LedColor c{cObj["r"], cObj["g"], cObj["b"]};
            colors.push_back(c);
        }

        return true;
    }
};

#endif // LED_PRESET_MODEL_H