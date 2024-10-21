#ifndef Effect_H
#define Effect_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <LEDStripDriver.h>
#include <vector>
#include "EnviromentEffect.h"
#include "DriverLed.h"
#include <ArduinoJson.h>
#include <services/ServiceImplementations/SerialService.h>

enum class LED_EFFECT_LABEL;
enum class STEP_LIFE_LED_EFFECT;
enum class TYPE_STRIP;

class DriverLed;

// Service interface
class Effect
{
public:
    virtual String getName() = 0;
    virtual int getMinColorsNumber() = 0;
    virtual void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService = nullptr) = 0;
    virtual void off(DriverLed *driver, TYPE_STRIP typeOrchestrator) = 0;
    virtual String toJson()
    {
        StaticJsonDocument<200> doc;
        doc["name"] = getName();
        doc["minColorsNumber"] = getMinColorsNumber();

        String output;
        serializeJson(doc, output);
        return output;
    }
};

#endif // Effect_H