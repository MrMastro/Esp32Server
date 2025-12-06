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
    String toJson() const
    {
        JsonDocument doc;  // Usa JsonDocument senza dimensione

        doc["effect"] = effect;
        doc["deltaT"] = deltaT;

        JsonArray triggersArray = doc.createNestedArray("triggers");
        for (const auto &t : triggers){
            triggersArray.add(t);
        }

        JsonArray colorsArray = doc.createNestedArray("colors");
        for (const auto &color : colors)
        {
            JsonObject colorObj = colorsArray.createNestedObject();
            color.toJson(colorObj);  // Utilizziamo il metodo toJson di LedColorRequest
        }

        String output;
        serializeJson(doc, output);
        return output;
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
        for (JsonVariant v : obj["triggers"].as<JsonArray>()){
            triggers.push_back(v.as<String>());
        }

        colors.clear();
        JsonArray colorsArray = doc["colors"].as<JsonArray>();
        for (JsonObject colorObj : colorsArray)
        {
            LedColor color;
            if (!color.fromJson(colorObj))
            {
                return false; // Se fallisce la deserializzazione di un colore
            }
            colors.push_back(color); // Aggiungi il colore al vettore
        }

        return true;
    }
};

#endif // LED_PRESET_MODEL_H