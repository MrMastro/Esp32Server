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

const std::pair<RGB_EFFECT, String> EFFECT_RGB_PAIR[] = {
    {RGB_EFFECT::NO_EFFECT, "NO_EFFECT"},
    {RGB_EFFECT::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
    {RGB_EFFECT::ACTUAL_EFFECT, "ACTUAL_EFFECT"},
    {RGB_EFFECT::CONSTANT_UNIQUE_COLOR, "CONSTANT_UNIQUE_COLOR"},
    {RGB_EFFECT::WAWE_UNIQUE_COLOR, "WAWE_UNIQUE_COLOR"}
    // Add more effect codes and information as needed
};

const String rgbEffectEnomToString(RGB_EFFECT inputEnum);
const RGB_EFFECT rgbEffectStringToEnum(String inputString);

#endif //LED_EFFECT