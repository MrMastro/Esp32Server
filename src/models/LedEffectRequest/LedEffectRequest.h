#ifndef LED_EFFECT_REQUEST_H
#define LED_EFFECT_REQUEST_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <vector>
#include "LedColorRequest.h"

class LedEffectRequest
{
public:
    String effect;
    std::vector<LedColorRequest> colors;
    int ms;
    boolean rgbAction;
    boolean ws2811Action;
    boolean ws2811MatrixAction;  // Aggiunto ws2811MatrixAction

    // Serializzazione in JSON
    String toJson() const
    {
        JsonDocument doc;  // Usa JsonDocument senza dimensione

        doc["effect"] = effect;
        doc["ms"] = ms;
        doc["rgbAction"] = rgbAction;
        doc["ws2811Action"] = ws2811Action;
        doc["ws2811MatrixAction"] = ws2811MatrixAction;  // Aggiungi ws2811MatrixAction

        // Creiamo un array JSON per i colori
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

    // Deserializzazione da JSON
    bool fromJson(const String &json)
    {
        JsonDocument doc;  // Usa JsonDocument senza dimensione
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        if (!doc.containsKey("effect") || !doc.containsKey("ms") || !doc.containsKey("colors") ||
            !doc.containsKey("rgbAction") || !doc.containsKey("ws2811Action") || !doc.containsKey("ws2811MatrixAction"))
        {
            return false; // Chiave mancante
        }

        effect = doc["effect"].as<String>();
        ms = doc["ms"];
        rgbAction = doc["rgbAction"];
        ws2811Action = doc["ws2811Action"];
        ws2811MatrixAction = doc["ws2811MatrixAction"];  // Aggiungi ws2811MatrixAction

        // Deserializziamo il vettore dei colori
        colors.clear(); // Pulisci il vettore prima di aggiungere nuovi elementi
        JsonArray colorsArray = doc["colors"].as<JsonArray>();
        for (JsonObject colorObj : colorsArray)
        {
            LedColorRequest color;
            if (!color.fromJson(colorObj))
            {
                return false; // Se fallisce la deserializzazione di un colore
            }
            colors.push_back(color); // Aggiungi il colore al vettore
        }

        return true;
    }
};

#endif // LED_EFFECT_REQUEST_H