#include "effects/EnviromentEffect.h"

#include "effects/Effect.h"
#include "effects/EffectImplementations/AlternatingEffect.h"
#include "effects/EffectImplementations/ConstantUniqueColorEffect.h"
#include "effects/EffectImplementations/FadeEffect.h"
#include "effects/EffectImplementations/ProgressiveBarUniqueColorEffect.h"
#include "effects/EffectImplementations/WaweUniqueColorEffect.h"
#include "EnviromentEffect.h"

const std::vector<Effect *> EFFECT_REFERENCE = {
    new ConstantUniqueColorEffect(),
    new WaweUniqueColorEffect(),
    new ProgressiveBarUniqueColorEffect(),
    new FadeEffect(),
    new AlternatingEffect()};

const EFFECT_LABEL getEffectLabelByName(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : EFFECT_LABEL_REFERENCE_STRING)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return EFFECT_LABEL::UKNOWN_EFFECT;
}

// const String LabelEffectEnumToString(EFFECT_LABEL inputEnum)
// {
//   for (const auto &entry : EFFECT_LABEL_REFERENCE_STRING)
//   {
//     if (entry.first == inputEnum)
//     {
//       return entry.second;
//     }
//   }
//   // Return a special value or throw an exception if the string is not found
//   return LabelEffectEnumToString(EFFECT_LABEL::UKNOWN_EFFECT);
// }

std::vector<String> getAllWS2811EffectNames()
{
  std::vector<String> effectNames;

  // Itera attraverso tutte le coppie e aggiungi il secondo valore (la stringa) al vettore
  for (const auto &effect : EFFECT_REFERENCE)
  {
    effectNames.push_back(effect->getName());
  }

  return effectNames;
}

boolean isPresentEffect(String nameEffect)
{
  boolean result = false;
  for (const auto &effect : EFFECT_REFERENCE)
  {
    if (effect->getName() == nameEffect)
    {
      return true;
    }
  }
  return result;
}

const String stepLifeEffectEnomToString(STEP_LIFE_EFFECT inputEnum)
{
  for (const auto &entry : STEP_LIFE_EFFECT_PAIR)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return "OFF";
}

Effect *getEffectByEffectName(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : EFFECT_REFERENCE)
  {
    if (entry->getName() == inputString)
    {
      return entry;
    }
  }
  return nullptr;
}