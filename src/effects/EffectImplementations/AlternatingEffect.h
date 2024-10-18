#ifndef ALTERNATING_EFFECT_H
#define ALTERNATING_EFFECT_H

#include "../Effect.h"

class AlternatingEffect : public Effect {
public:
    AlternatingEffect();
    String getName() override;
    int getColorInputQt() override;
    void execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, 
                  const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type) override;

    void off(DriverLed* driver, TYPE_STRIP type) override;
};

#endif