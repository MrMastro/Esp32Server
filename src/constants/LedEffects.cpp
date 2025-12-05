#include "LedEffects.h"

const std::pair<RGB_EFFECT, String> EFFECT_RGB_PAIR[] = {
  {RGB_EFFECT::NO_EFFECT, "NO_EFFECT"},
  {RGB_EFFECT::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
  {RGB_EFFECT::ACTUAL_EFFECT, "ACTUAL_EFFECT"},
  {RGB_EFFECT::CONSTANT_UNIQUE_COLOR, "CONSTANT_UNIQUE_COLOR"},
  {RGB_EFFECT::WAWE_UNIQUE_COLOR, "WAWE_UNIQUE_COLOR"}
  // Add more effect codes and information as needed
};

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