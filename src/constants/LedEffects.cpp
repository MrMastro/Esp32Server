#include "LedEffects.h"

const WS2811_EFFECT WS2811EffectStringToEnum(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : EFFECT_PAIR)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return WS2811_EFFECT::UKNOWN_EFFECT;
}

const String WS2811EffectEnomToString(WS2811_EFFECT inputString)
{
  for (const auto &entry : EFFECT_PAIR)
  {
    if (entry.first == inputString)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return "UKNOWN_EFFECT";
}
