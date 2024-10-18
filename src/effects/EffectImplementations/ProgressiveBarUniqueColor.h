//PROGRESSIVE_BAR_UNIQUE_COLOR
#ifndef PROGRESSIVE_BAR_UNIQUE_COLOR_EFFECT_H
#define PROGRESSIVE_BAR_UNIQUE_COLOR_EFFECT_H

#include "../Effect.h"

class ProgressiveBarUniqueColor : public Effect {
public:
    ProgressiveBarUniqueColor();
    String getName() override;
    
    void execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, 
                  std::vector<RgbColor> colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type) override;

    void off(DriverLed* driver, TYPE_STRIP type) override;
};

#endif