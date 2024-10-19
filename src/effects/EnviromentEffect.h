#ifndef EnviroentEffect_H
#define EnviroentEffect_H

#include <Arduino.h>
#include <vector>

enum class TYPE_STRIP
{
  RGB,
  WS2811
};

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
    PROGRESSIVE_BAR_UNIQUE_COLOR, //Effect of progressive color
    FADE_EFFECT, //Effect of fade color
    ALTERNATING_EFFECT //Effect that alternate two or more color at same
};


const std::pair<EFFECT_LABEL, String> EFFECT_LABEL_REFERENCE_STRING[] = {
    {EFFECT_LABEL::NO_EFFECT, "NO_EFFECT"},
    {EFFECT_LABEL::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
    {EFFECT_LABEL::ACTUAL_EFFECT, "ACTUAL_EFFECT"},
    {EFFECT_LABEL::CONSTANT_UNIQUE_COLOR, "CONSTANT_UNIQUE_COLOR"},
    {EFFECT_LABEL::WAWE_UNIQUE_COLOR, "WAWE_UNIQUE_COLOR"},
    {EFFECT_LABEL::PROGRESSIVE_BAR_UNIQUE_COLOR, "PROGRESSIVE_BAR_UNIQUE_COLOR"},
    {EFFECT_LABEL::FADE_EFFECT, "FADE_EFFECT"},
    {EFFECT_LABEL::ALTERNATING_EFFECT, "ALTERNATING_EFFECT"}
    // Add more effect codes and information as needed
};

// const std::pair<EFFECT_LABEL, Effect*> EFFECT_REFERENCE[] = {
//     {EFFECT_LABEL::CONSTANT_UNIQUE_COLOR, new ConstantUniqueColorEffect()},
//     {EFFECT_LABEL::WAWE_UNIQUE_COLOR, new WaweUniqueColorEffect()},
//     {EFFECT_LABEL::PROGRESSIVE_BAR_UNIQUE_COLOR, new ProgressiveBarUniqueColorEffect()},
//     {EFFECT_LABEL::FADE_EFFECT, new FadeEffect()}
// };

const EFFECT_LABEL LabelEffectStringToEnum(String inputString);
const String LabelEffectEnumToString(EFFECT_LABEL inputEnum);
// Funzione per ottenere un vettore con i nomi degli effetti (le stringhe)
std::vector<String> getAllWS2811EffectNames();
const String stepLifeEffectEnomToString(STEP_LIFE_EFFECT inputEnum);

#endif