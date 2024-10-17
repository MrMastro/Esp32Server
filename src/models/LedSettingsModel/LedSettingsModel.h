#ifndef LED_SETTINGS_H
#define LED_SETTINGS_H

#include <Arduino.h>
#include <ArduinoJson.h>

class LedSettingsModel
{

public:
    //---- WS2811 SETTINGS -------
    boolean enableStripWs2811;
    int numLedWs2811;
    int pinLedWs2811;

    //---- SIMPLE STRIPT RGB SETTINGS -------
    boolean enableStripRgb;
    int pinLedDinRgb;
    int pinLedCinRgb;


    // Serializzazione in JSON
    void toJson(JsonObject &json) const
    {
        json["enableStripWs2811"] = enableStripWs2811;
        json["numLedWs2811"] = numLedWs2811;
        json["pinLedWs2811"] = pinLedWs2811;
        json["enableStripRgb"] = enableStripRgb;
        json["pinLedDinRgb"] = pinLedDinRgb;
        json["pinLedCinRgb"] = pinLedCinRgb;
    }

    // Deserializzazione da JSON
    bool fromJson(const String& json) {
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        enableStripWs2811 = doc["enableStripWs2811"];
        numLedWs2811 = doc["numLedWs2811"];
        pinLedWs2811 = doc["pinLedWs2811"];
        enableStripRgb = doc["enableStripRgb"];
        pinLedDinRgb = doc["pinLedDinRgb"];
        pinLedCinRgb = doc["pinLedCinRgb"];
        return true;
    }

    // Metodo statico per ottenere un'istanza di LedSettingsModel con valori di default
    static LedSettingsModel getDefault() {
        LedSettingsModel defaultLedSettings;
        defaultLedSettings.enableStripWs2811 = false;
        defaultLedSettings.numLedWs2811 = 32;
        defaultLedSettings.pinLedWs2811 = 5;
        defaultLedSettings.enableStripRgb = false;
        defaultLedSettings.pinLedDinRgb = 19;
        defaultLedSettings.pinLedCinRgb = 18;
        return defaultLedSettings;
    }
};

#endif // LED_SETTINGS_H