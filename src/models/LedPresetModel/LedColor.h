#ifndef LED_COLOR_H
#define LED_COLOR_H

struct LedColor
{
    int r;
    int g;
    int b;

    LedColor(){
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
        if (!jsonObj["r"].is<int>() || !jsonObj["g"].is<int>() || !jsonObj["b"].is<int>())
        {
            return false; // Chiave mancante
        }

        r = jsonObj["r"];
        g = jsonObj["g"];
        b = jsonObj["b"];

        return true;
    }
};

#endif // LED_COLOR_H