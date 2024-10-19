#ifndef LED_EFFECT
#define LED_EFFECT

#include <vector>
#include <Arduino.h>

//Setep for lyfecycle --------------------------------------------------------------------------------
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

//EFFECT_LABEL

enum class EFFECT_LABEL
{
    NO_EFFECT, //No effect
    UKNOWN_EFFECT, //I don't know the effect
    ACTUAL_EFFECT, //The effect that run in this moment
    CONSTANT_UNIQUE_COLOR, // Effect of constant color in all pixel of stript
    WAWE_UNIQUE_COLOR, //Effect of wawe color
    PROGRESSIVE_BAR_UNIQUE_COLOR //Effect of progressive color
};

const std::pair<EFFECT_LABEL, String> EFFECT_WS2811_PAIR[] = {
    {EFFECT_LABEL::NO_EFFECT, "NO_EFFECT"},
    {EFFECT_LABEL::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
    {EFFECT_LABEL::ACTUAL_EFFECT, "ACTUAL_EFFECT"},
    {EFFECT_LABEL::CONSTANT_UNIQUE_COLOR, "CONSTANT_UNIQUE_COLOR"},
    {EFFECT_LABEL::WAWE_UNIQUE_COLOR, "WAWE_UNIQUE_COLOR"},
    {EFFECT_LABEL::PROGRESSIVE_BAR_UNIQUE_COLOR, "PROGRESSIVE_BAR_UNIQUE_COLOR"}
    // Add more effect codes and information as needed
};

const EFFECT_LABEL WS2811EffectStringToEnum(String inputString);
const String WS2811EffectEnomToString(EFFECT_LABEL inputEnum);
// Funzione per ottenere un vettore con i nomi degli effetti (le stringhe)
std::vector<String> getAllWS2811EffectNames();
const String stepLifeEffectEnomToString(STEP_LIFE_EFFECT inputEnum);



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