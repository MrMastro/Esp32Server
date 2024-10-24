#ifndef FADE_EFFECT_H
#define FADE_EFFECT_H

#include "../Effect.h"
#include <Arduino.h>

class BreathEffect : public Effect
{
public:
    BreathEffect();
    String getName() override;
    int getMaxColorsNumber() override;
    int getMinColorsNumber() override;
    boolean getCompatibilityRgb();
    boolean getCompatibilityWs2811();
    void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService = nullptr) override;
    void off(DriverLed *driver, TYPE_STRIP type) override;

private:
    float brightness; // Livello di luminosità per il fading
    boolean increasing;
    float minBrightnessThresholdPercent;
};

#endif