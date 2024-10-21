#ifndef FADE_EFFECT_H
#define FADE_EFFECT_H

#include "../Effect.h"
#include <Arduino.h>

class FadeEffect : public Effect {
public:
    FadeEffect();
    String getName() override;
    int getMinColorsNumber() override;
    void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService = nullptr) override;
    void off(DriverLed* driver, TYPE_STRIP type) override;

private:
    float brightness;  // Livello di luminosità per il fading
    boolean increasing;
};

#endif