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
    int initialR;
    int initialG;
    int initialB;

    // Serializzazione in JSON
    String toJson() const
    {
        StaticJsonDocument<768> doc;

        doc["initialEffect"] = initialEffect;
        doc["initialDeltaT"] = initialDeltaT;
        doc["initialR"] = initialR;
        doc["initialG"] = initialG;
        doc["initialB"] = initialB;

        String output;
        serializeJson(doc, output);
        return output;
    }

    // Deserializzazione da JSON
    bool fromJson(const String &json)
    {
        StaticJsonDocument<192> doc;
        DeserializationError error = deserializeJson(doc, json);

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return false;
        }

        initialEffect = doc["initialEffect"].as<String>();
        initialDeltaT = doc["initialDeltaT"];
        initialR = doc["initialR"];
        initialG = doc["initialG"];
        initialB = doc["initialB"];

        return true;
    }

    String toString()
    {
        return toJson();
    }

};

#endif // INITIAL_SETTINGS_SAVE_MODEL_H