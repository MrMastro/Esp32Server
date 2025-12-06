#ifndef LED_PRESET_LIST_MODEL_H
#define LED_PRESET_LIST_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "LedPresetModel.h"

class LedPresetListModel
{
public:
    std::vector<LedPresetModel> presets;

    // ---------------------------------------
    // Deserializzazione da stringa JSON (array)
    // ---------------------------------------
    bool fromJson(const String &json)
    {
        presets.clear(); 

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        JsonArray arr = doc.as<JsonArray>();

        for (JsonObject obj : arr)
        {
            String elementJson;
            serializeJson(obj, elementJson);

            LedPresetModel p;
            if (!p.fromJson(elementJson))
                return false;

            presets.push_back(p);
        }

        return true;
    }

    // ---------------------------------------
    // Serializzazione lista preset
    // ---------------------------------------
    String toJson() const
    {
        JsonDocument doc; // dimensione sufficiente per tutta la lista
        JsonArray arr = doc.to<JsonArray>();

        for (const auto &p : presets)
        {
            JsonObject obj = arr.createNestedObject();

            obj["effect"] = p.effect;
            obj["deltaT"] = p.deltaT;

            JsonArray triggersArray = obj.createNestedArray("triggers");
            for (const auto &t : p.triggers)
                triggersArray.add(t);

            JsonArray colorsArray = obj.createNestedArray("colors");
            for (const auto &c : p.colors)
            {
                JsonObject colorObj = colorsArray.createNestedObject();
                colorObj["r"] = c.r;
                colorObj["g"] = c.g;
                colorObj["b"] = c.b;
            }
        }

        String output;
        serializeJson(arr, output);
        return output;
    }
    // ---------------------------------------
    // Ottieni singolo preset per trigger
    // ---------------------------------------
    const LedPresetModel* getByTrigger(const String &trigger) const
    {
        for (const auto &p : presets)
        {
            for (const auto &t : p.triggers)
            {
                if (t == trigger)
                    return &p;
            }
        }
        return nullptr;
    }

    // ---------------------------------------
    // Ottieni singolo preset per effetto
    // ---------------------------------------
    const LedPresetModel* getByEffect(const String &effect) const
    {
        for (const auto &p : presets)
        {
            if (p.effect == effect)
                return &p;
        }
        return nullptr;
    }
};

#endif // LED_PRESET_LIST_MODEL_H