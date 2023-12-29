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

enum class WS2811_EFFECT
{
    NO_EFFECT,
    UKNOWN_EFFECT,
    PROGRESSIVE_BAR_UNIQUE_COLOR
};

const std::pair<WS2811_EFFECT, String> EFFECT_PAIR[] = {
    {WS2811_EFFECT::NO_EFFECT, "NO_EFFECT"},
    {WS2811_EFFECT::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
    {WS2811_EFFECT::PROGRESSIVE_BAR_UNIQUE_COLOR, "PROGRESSIVE_BAR_UNIQUE_COLOR"}
    // Add more effect codes and information as needed
};

const WS2811_EFFECT WS2811EffectStringToEnum(String inputString);
const String WS2811EffectEnomToString(WS2811_EFFECT inputString);

#endif //LED_EFFECT