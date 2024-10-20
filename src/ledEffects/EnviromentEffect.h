#ifndef EnviroentEffect_H
#define EnviroentEffect_H

#include <Arduino.h>
#include <vector>
#include <map>
#include "ledEffects/Effect.h"

class Effect;

enum class TYPE_STRIP
{
  RGB,
  WS2811
};

enum class STEP_LIFE_LED_EFFECT
{
  BEGIN_STEP,
  LOOP_STEP,
  END_STEP,
  OFF
};

const std::pair<STEP_LIFE_LED_EFFECT, String> STEP_LIFE_LED_EFFECT_PAIR[] = {
    {STEP_LIFE_LED_EFFECT::BEGIN_STEP, "BEGIN_STEP"},
    {STEP_LIFE_LED_EFFECT::LOOP_STEP, "LOOP_STEP"},
    {STEP_LIFE_LED_EFFECT::END_STEP, "END_STEP"},
    {STEP_LIFE_LED_EFFECT::OFF, "OFF"}
};

//LED_EFFECT_LABEL
enum class LED_EFFECT_LABEL
{
    NO_EFFECT, //No effect
    UKNOWN_EFFECT, //I don't know the effect
    ACTUAL_EFFECT, //The effect that run in this moment
};

const std::pair<LED_EFFECT_LABEL, String> LED_EFFECT_LABEL_REFERENCE_STRING[] = {
    {LED_EFFECT_LABEL::NO_EFFECT, "NO_EFFECT"},
    {LED_EFFECT_LABEL::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
    {LED_EFFECT_LABEL::ACTUAL_EFFECT, "ACTUAL_EFFECT"}
};

extern const std::vector<Effect*> LED_EFFECT_REFERENCE;

const LED_EFFECT_LABEL getEffectLabelByName(String inputString);
const String getStringByEffectLabel(LED_EFFECT_LABEL inputEnum);
std::vector<String> getAllWS2811EffectNames();
std::vector<Effect*> getAllWS2811EffectReference();
boolean isPresentEffect(String nameEffect);

const String stepLifeEffectEnomToString(STEP_LIFE_LED_EFFECT inputEnum);
Effect* getEffectByEffectName(String inputString);

#endif //EnviroentEffect_H