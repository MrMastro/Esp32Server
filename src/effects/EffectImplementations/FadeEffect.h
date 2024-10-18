#ifndef FADE_EFFECT_H
#define FADE_EFFECT_H

#include "../Effect.h"
#include <Arduino.h>

class FadeEffect : public Effect {
public:
    FadeEffect();
    String getName() override;
    int getColorInputQt() override;
    void execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type) override;
    void off(DriverLed* driver, TYPE_STRIP type) override;

private:
    float brightness;  // Livello di luminosità per il fading
    boolean increasing;
};

#endif