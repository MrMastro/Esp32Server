#include "./Effect.h"

const EFFECT_LABEL LabelEffectStringToEnum(String inputString)
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

const String LabelEffectEnumToString(EFFECT_LABEL inputEnum)
{
  for (const auto &entry : EFFECT_LABEL_REFERENCE_STRING)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return LabelEffectEnumToString(EFFECT_LABEL::UKNOWN_EFFECT);
}

std::vector<String> getAllWS2811EffectNames()
{
    std::vector<String> effectNames;
    
    // Itera attraverso tutte le coppie e aggiungi il secondo valore (la stringa) al vettore
    for (const auto& effect : EFFECT_LABEL_REFERENCE_STRING) {
        if(!(effect.first == EFFECT_LABEL::NO_EFFECT || effect.first == EFFECT_LABEL::ACTUAL_EFFECT || effect.first == EFFECT_LABEL::UKNOWN_EFFECT)){
          effectNames.push_back(effect.second);
        }
    }
    
    return effectNames;
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
