#include "LedEffects.h"

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