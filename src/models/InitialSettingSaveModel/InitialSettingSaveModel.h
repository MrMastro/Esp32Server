#ifndef INITIAL_SETTINGS_SAVE_MODEL_H
#define INITIAL_SETTINGS_SAVE_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

class InitialSettingSaveModel
{
public:
    //---- Initial Operation -------
    String initialEffect;
    int initialDeltaT;
    std::vector<LedColorRequest> initialColors;

    // Serializzazione in JSON
    String toJson() const
    {
        DynamicJsonDocument doc(1024); // Usa DynamicJsonDocument per allocare dinamicamente la memoria

        doc["initialEffect"] = initialEffect;
        doc["initialDeltaT"] = initialDeltaT;

        // Serializzazione del vettore di colori initialColors
        JsonArray colorsArray = doc.createNestedArray("initialColors");
        for (const auto &color : initialColors)
        {
            JsonObject colorObj = colorsArray.createNestedObject();
            colorObj["r"] = color.r;
            colorObj["g"] = color.g;
            colorObj["b"] = color.b;
        }

        String output;
        serializeJson(doc, output);
        return output;
    }

    // Deserializzazione da JSON
    bool fromJson(const String &json)
    {
        DynamicJsonDocument doc(1024); // Usa DynamicJsonDocument per la deserializzazione
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        initialEffect = doc["initialEffect"].as<String>();
        initialDeltaT = doc["initialDeltaT"];

        // Deserializzazione del vettore di colori initialColors
        initialColors.clear(); // Pulisce il vettore prima di aggiungere nuovi colori
        JsonArray colorsArray = doc["initialColors"].as<JsonArray>();
        for (JsonObject colorObj : colorsArray)
        {
            LedColorRequest color;
            color.r = colorObj["r"];
            color.g = colorObj["g"];
            color.b = colorObj["b"];
            initialColors.push_back(color); // Aggiungi il colore al vettore
        }

        return true;
    }

    String toString()
    {
        return toJson();
    }
};

#endif // INITIAL_SETTINGS_SAVE_MODEL_H