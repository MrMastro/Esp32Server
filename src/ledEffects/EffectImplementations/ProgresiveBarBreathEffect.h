#ifndef PROGRESSI_BAR_BREATH_EFFECT_H
#define PROGRESSI_BAR_BREATH_EFFECT_H

#include "../Effect.h"
#include <Arduino.h>

class ProgresiveBarBreathEffect : public Effect
{
public:
    ProgresiveBarBreathEffect();
    String getName() override;
    int getMaxColorsNumber() override;
    int getMinColorsNumber() override;
    boolean getCompatibilityRgb();
    boolean getCompatibilityWs2811();
    boolean execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService = nullptr) override;
    void off(DriverLed *driver, TYPE_STRIP type) override;

private:
    float brightness; // Livello di luminosità per il fading
    boolean increasing;
    float minBrightnessThresholdPercent;
    int timeAccumulator; 
    int positionProgressBar;
};

#endif