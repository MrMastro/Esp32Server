#ifndef LED_EFFECT
#define LED_EFFECT

#include <vector>
#include <Arduino.h>

enum class STEP_LIFE_EFFECT
{
  BEGIN_STEP,
  LOOP_STEP,
  END_STEP,
  OFF
};

const std::pair<STEP_LIFE_EFFECT, String> STEP_LIFE_EFFECT_PAIR[] = {
    {STEP_LIFE_EFFECT::BEGIN_STEP, "BEGIN_STEP"},
    {STEP_LIFE_EFFECT::LOOP_STEP, "LOOP_STEP"},
    {STEP_LIFE_EFFECT::END_STEP, "END_STEP"},
    {STEP_LIFE_EFFECT::OFF, "OFF"}
};


enum class WS2811_EFFECT
{
    NO_EFFECT, //No effect
    UKNOWN_EFFECT, //I don't know the effect
    ACTUAL_EFFECT, //The effect that run in this moment
    PROGRESSIVE_BAR_UNIQUE_COLOR, //Effect of progressive color
    CONSTANT_UNIQUE_COLOR // Effect of constant color in all pixel of stript
};

const std::pair<WS2811_EFFECT, String> EFFECT_PAIR[] = {
    {WS2811_EFFECT::NO_EFFECT, "NO_EFFECT"},
    {WS2811_EFFECT::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
    {WS2811_EFFECT::ACTUAL_EFFECT, "ACTUAL_EFFECT"},
    {WS2811_EFFECT::PROGRESSIVE_BAR_UNIQUE_COLOR, "PROGRESSIVE_BAR_UNIQUE_COLOR"},
    {WS2811_EFFECT::CONSTANT_UNIQUE_COLOR, "CONSTANT_UNIQUE_COLOR"}
    // Add more effect codes and information as needed
};

const WS2811_EFFECT WS2811EffectStringToEnum(String inputString);
const String WS2811EffectEnomToString(WS2811_EFFECT inputEnum);

const String stepLifeEffectEnomToString(STEP_LIFE_EFFECT inputEnum);

#endif //LED_EFFECT