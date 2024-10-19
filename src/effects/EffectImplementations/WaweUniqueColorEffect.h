//WAWE_UNIQUE_COLOR

#ifndef WAWE_UNIQUE_COLOR_EFFECT_H
#define WAWE_UNIQUE_COLOR_EFFECT_H

#include "../Effect.h"

class WaweUniqueColorEffect : public Effect {
public:
    WaweUniqueColorEffect();
    String getName() override;
    int getColorInputQt() override;
    void execStep(String effectInput, STEP_LIFE_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type) override;

    void off(DriverLed* driver, TYPE_STRIP type) override;
};

#endif