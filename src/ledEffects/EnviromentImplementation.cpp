#include "ledEffects/EnviromentEffect.h"

#include "ledEffects/Effect.h"
#include "ledEffects/EffectImplementations/MulticolorConstantEffect.h"
#include "ledEffects/EffectImplementations/ConstantUniqueColorEffect.h"
#include "ledEffects/EffectImplementations/FadeEffect.h"
#include "ledEffects/EffectImplementations/ProgressiveBarUniqueColorEffect.h"
#include "ledEffects/EffectImplementations/ConsumeColorEffect.h"
#include "EnviromentEffect.h"

const std::vector<Effect *> LED_EFFECT_REFERENCE = {
    new ConstantUniqueColorEffect(),
    new ConsumeColorEffect(),
    new ProgressiveBarUniqueColorEffect(),
    new FadeEffect(),
    new MulticolorConstantEffect()};

const LED_EFFECT_LABEL getEffectLabelByName(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : LED_EFFECT_LABEL_REFERENCE_STRING)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return LED_EFFECT_LABEL::UKNOWN_EFFECT;
}

const String getStringByEffectLabel(LED_EFFECT_LABEL inputEnum)
{
  for (const auto &entry : LED_EFFECT_LABEL_REFERENCE_STRING)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return getStringByEffectLabel(LED_EFFECT_LABEL::UKNOWN_EFFECT);
}

std::vector<String> getAllWS2811EffectNames()
{
  std::vector<String> effectNames;

  // Itera attraverso tutte le coppie e aggiungi il secondo valore (la stringa) al vettore
  for (const auto &effect : LED_EFFECT_REFERENCE)
  {
    effectNames.push_back(effect->getName());
  }

  return effectNames;
}

std::vector<Effect*> getAllWS2811EffectReference()
{
  std::vector<Effect*> effectNames;

  // Itera attraverso tutte le coppie e aggiungi il secondo valore (la stringa) al vettore
  for (const auto &effect : LED_EFFECT_REFERENCE)
  {
    effectNames.push_back(effect);
  }

  return effectNames;
}

boolean isPresentEffect(String nameEffect)
{
  boolean result = false;
  for (const auto &effect : LED_EFFECT_REFERENCE)
  {
    if (effect->getName() == nameEffect)
    {
      return true;
    }
  }
  return result;
}

const String stepLifeEffectEnomToString(STEP_LIFE_LED_EFFECT inputEnum)
{
  for (const auto &entry : STEP_LIFE_LED_EFFECT_PAIR)
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
  for (const auto &entry : LED_EFFECT_REFERENCE)
  {
    if (entry->getName() == inputString)
    {
      return entry;
    }
  }
  return nullptr;
}