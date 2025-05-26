#ifndef LED_SETTINGS_H
#define LED_SETTINGS_H

#include <Arduino.h>
#include <ArduinoJson.h>

class LedSettingsModel
{
public:
    //---- WS2811_MATRIX SETTINGS -------
    boolean enableStripWs2811Matrix;
    int numLedWs2811Matrix;
    int pinLedWs2811Matrix;

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
        json["enableStripWs2811Matrix"] = enableStripWs2811Matrix;
        json["numLedWs2811Matrix"] = numLedWs2811Matrix;
        json["pinLedWs2811Matrix"] = pinLedWs2811Matrix;
        json["enableStripWs2811"] = enableStripWs2811;
        json["numLedWs2811"] = numLedWs2811;
        json["pinLedWs2811"] = pinLedWs2811;
        json["enableStripRgb"] = enableStripRgb;
        json["pinLedDinRgb"] = pinLedDinRgb;
        json["pinLedCinRgb"] = pinLedCinRgb;
    }

    // Deserializzazione da JSON
    bool fromJson(const String& json) {
        // Usa JsonDocument per la deserializzazione
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error) {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        // Assegna i valori deserializzati ai membri della classe
        enableStripWs2811 = doc["enableStripWs2811"];
        numLedWs2811 = doc["numLedWs2811"];
        pinLedWs2811 = doc["pinLedWs2811"];
        enableStripRgb = doc["enableStripRgb"];
        pinLedDinRgb = doc["pinLedDinRgb"];
        pinLedCinRgb = doc["pinLedCinRgb"];
        enableStripWs2811Matrix = doc["enableStripWs2811Matrix"];
        numLedWs2811Matrix = doc["numLedWs2811Matrix"];
        pinLedWs2811Matrix = doc["pinLedWs2811Matrix"];
        
        return true;
    }

    // Metodo statico per ottenere un'istanza di LedSettingsModel con valori di default
    static LedSettingsModel getDefault()
    {
        LedSettingsModel defaultLedSettings;
        defaultLedSettings.enableStripWs2811Matrix = false;
        defaultLedSettings.numLedWs2811Matrix = 128;
        defaultLedSettings.pinLedWs2811Matrix = 21;
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