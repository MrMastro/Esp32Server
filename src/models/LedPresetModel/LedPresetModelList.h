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
        DynamicJsonDocument doc(4096);
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        JsonArray arr = doc.as<JsonArray>();
        presets.clear();

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
    void toJson(JsonObject &json) const
    {
        JsonArray arr = json.createNestedArray("ledPresets");
        for (const auto &p : presets)
        {
            JsonObject o = arr.createNestedObject();
            p.toJson(o);
        }
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

    // ------------------------------------------------------------
    // Ritorna una stringa JSON contenente tutti i preset presenti
    // ------------------------------------------------------------
    String getPresetList() const
    {
        DynamicJsonDocument doc(4096); // dimensione sufficiente per tutti i preset
        JsonArray arr = doc.to<JsonArray>();

        for (const auto &p : presets)
        {
            JsonObject obj = arr.createNestedObject();
            p.toJson(obj);
        }

        String output;
        serializeJson(arr, output);
        return output;
    }
};

#endif // LED_PRESET_LIST_MODEL_H