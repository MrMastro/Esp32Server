//WAWE_UNIQUE_COLOR

#ifndef WAWE_UNIQUE_COLOR_EFFECT_H
#define WAWE_UNIQUE_COLOR_EFFECT_H

#include "../Effect.h"

class ConsumeColorEffect : public Effect {
public:
    ConsumeColorEffect();
    String getName() override;
    int getMinColorsNumber() override;
    void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService = nullptr) override;
    void off(DriverLed* driver, TYPE_STRIP type) override;
private:
    boolean inverted;
};

#endif