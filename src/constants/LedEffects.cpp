#include "LedEffects.h"



const String rgbEffectEnomToString(RGB_EFFECT inputEnum)
{
  for (const auto &entry : EFFECT_RGB_PAIR)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return "UKNOWN_EFFECT";
}

const EFFECT_LABEL WS2811EffectStringToEnum(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : EFFECT_WS2811_PAIR)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return EFFECT_LABEL::UKNOWN_EFFECT;
}

const String WS2811EffectEnomToString(EFFECT_LABEL inputEnum)
{
  for (const auto &entry : EFFECT_WS2811_PAIR)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return WS2811EffectEnomToString(EFFECT_LABEL::UKNOWN_EFFECT);
}

std::vector<String> getAllWS2811EffectNames()
{
    std::vector<String> effectNames;
    
    // Itera attraverso tutte le coppie e aggiungi il secondo valore (la stringa) al vettore
    for (const auto& effect : EFFECT_WS2811_PAIR) {
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


const RGB_EFFECT rgbEffectStringToEnum(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : EFFECT_RGB_PAIR)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return RGB_EFFECT::UKNOWN_EFFECT;
}