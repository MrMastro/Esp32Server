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

const WS2811_EFFECT WS2811EffectStringToEnum(String inputString)
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
  return WS2811_EFFECT::UKNOWN_EFFECT;
}

const String WS2811EffectEnomToString(WS2811_EFFECT inputEnum)
{
  for (const auto &entry : EFFECT_WS2811_PAIR)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return "UKNOWN_EFFECT";
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