#ifndef LED_EFFECT
#define LED_EFFECT

#include <vector>
#include <Arduino.h>

//RGB Deprecated -------------------------------------------------------------------------------------------------------------------
enum class RGB_EFFECT
{
    NO_EFFECT, //No effect
    UKNOWN_EFFECT, //I don't know the effect
    ACTUAL_EFFECT, //The effect that run in this moment
    CONSTANT_UNIQUE_COLOR, //Effect of constant color in all pixel of stript
    WAWE_UNIQUE_COLOR //Effect of wawe color
};

const String rgbEffectEnomToString(RGB_EFFECT inputEnum);
const RGB_EFFECT rgbEffectStringToEnum(String inputString);

#endif //LED_EFFECT