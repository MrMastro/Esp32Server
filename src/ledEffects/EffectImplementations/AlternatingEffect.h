#ifndef ALTERNATING_EFFECT_H
#define ALTERNATING_EFFECT_H

#include <Arduino.h>
#include <ledEffects/Effect.h>

enum class LED_EFFECT_LABEL;
enum class STEP_LIFE_LED_EFFECT;
enum class TYPE_STRIP;
class DriverLed;

class AlternatingEffect : public Effect 
{
public:
    AlternatingEffect();
    String getName() override;
    int getColorInputQt() override;
    void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService = nullptr) override;

    void off(DriverLed* driver, TYPE_STRIP type) override;
};

#endif