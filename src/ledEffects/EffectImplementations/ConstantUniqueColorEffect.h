//CONSTANT_UNIQUE_COLOR
#ifndef CONSTANT_UNIQUE_COLOR_EFFECT_H
#define CONSTANT_UNIQUE_COLOR_EFFECT_H

#include "../Effect.h"

class ConstantUniqueColorEffect : public Effect {
public:
    ConstantUniqueColorEffect();
    String getName() override;
    
    int getMaxColorsNumber() override;
    int getMinColorsNumber() override;
    boolean getCompatibilityRgb();
    boolean getCompatibilityWs2811();
    void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService = nullptr) override;

    void off(DriverLed* driver, TYPE_STRIP type) override;
};

#endif