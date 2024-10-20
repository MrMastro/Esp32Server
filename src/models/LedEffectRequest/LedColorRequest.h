#ifndef LED_COLOR_REQUEST_H
#define LED_COLOR_REQUEST_H

#include <Arduino.h>
#include <ArduinoJson.h>

class LedColorRequest
{
public:
    int r;
    int g;
    int b;

    LedColorRequest(){
        r = 0;
        g = 0;
        b = 0;
    }

    // Serializzazione in JSON
    void toJson(JsonObject &jsonObj) const
    {
        jsonObj["r"] = r;
        jsonObj["g"] = g;
        jsonObj["b"] = b;
    }

    // Deserializzazione da JSON
    bool fromJson(const JsonObject &jsonObj)
    {
        if (!jsonObj.containsKey("r") || !jsonObj.containsKey("g") || !jsonObj.containsKey("b"))
        {
            return false; // Chiave mancante
        }

        r = jsonObj["r"];
        g = jsonObj["g"];
        b = jsonObj["b"];

        return true;
    }
};

#endif // LED_COLOR_REQUEST_H