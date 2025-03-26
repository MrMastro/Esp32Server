#ifndef EYE_LEFT_EFFECT_H
#define EYE_LEFT_EFFECT_H

#include "../Effect.h"
#include <Arduino.h>

class EyeLeftEffect : public Effect
{
public:
    EyeLeftEffect();
    String getName() override;
    int getMaxColorsNumber() override;
    int getMinColorsNumber() override;
    boolean getCompatibilityRgb();
    boolean getCompatibilityWs2811();
    boolean getCompatibilityWs2811Matrix();
    boolean execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService = nullptr) override;
    void off(DriverLed *driver, TYPE_STRIP type) override;

private:
    float brightness; // Livello di luminosità per il fading
    boolean increasing;
};

#endif