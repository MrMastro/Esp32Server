#ifndef Effect_H
#define Effect_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <LEDStripDriver.h>
#include <vector>
#include "EnviromentEffect.h"
#include "DriverLed.h"

// Service interface
class Effect {
public:
    virtual String getName() = 0;
    virtual int getColorInputQt() = 0;
    virtual void execStep(EFFECT_LABEL ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput,const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP typeOrchestrator) = 0;
    virtual void off(DriverLed* driver, TYPE_STRIP typeOrchestrator) =0;
};

#endif  // Effect_H