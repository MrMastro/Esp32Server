#ifndef ALTERNATING_EFFECT_H
#define ALTERNATING_EFFECT_H

#include <Arduino.h>
#include <ledEffects/Effect.h>

enum class LED_EFFECT_LABEL;
enum class STEP_LIFE_LED_EFFECT;
enum class TYPE_STRIP;
class DriverLed;

class MulticolorConstantEffect : public Effect
{
public:
    MulticolorConstantEffect();
    String getName() override;
    int getMaxColorsNumber() override;
    int getMinColorsNumber() override;
    boolean getCompatibilityRgb();
    boolean getCompatibilityWs2811();
    boolean getCompatibilityWs2811Matrix();
    boolean execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService = nullptr) override;

    void off(DriverLed *driver, TYPE_STRIP type) override;
};

#endif