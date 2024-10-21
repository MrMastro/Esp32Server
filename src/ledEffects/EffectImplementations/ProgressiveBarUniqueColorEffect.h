//PROGRESSIVE_BAR_UNIQUE_COLOR
#ifndef PROGRESSIVE_BAR_UNIQUE_COLOR_EFFECT_H
#define PROGRESSIVE_BAR_UNIQUE_COLOR_EFFECT_H

#include "../Effect.h"

class ProgressiveBarUniqueColorEffect : public Effect {
public:
    ProgressiveBarUniqueColorEffect();
    String getName() override;
    int getMaxColorsNumber() override;
    int getMinColorsNumber() override;
    void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService = nullptr) override;

    void off(DriverLed* driver, TYPE_STRIP type) override;
};

#endif