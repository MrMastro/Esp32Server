#include "ledEffects/EffectOrchestrator.h"
#include "EffectOrchestrator.h"

EffectOrchestrator::EffectOrchestrator()
{
  driver = nullptr;
  operative = false;
  serialService = nullptr;
  name = String();
}

EffectOrchestrator::EffectOrchestrator(String nameI, DriverLed *driverInput, TYPE_STRIP typeledInput, SerialService *serialServiceI)
{
  driver = driverInput;
  typeLed = typeledInput;
  serialService = serialServiceI;
  name = nameI;
}

EffectOrchestrator::EffectOrchestrator(String nameI, DriverLed *driverInput, TYPE_STRIP typeledInput, boolean enable, SerialService *serialServiceI)
{
  driver = driverInput;
  typeLed = typeledInput;
  operative = enable;
  serialService = serialServiceI;
  name = nameI;
}

boolean EffectOrchestrator::runLifeCycle()
{
  if (serialService != nullptr)
  {
    if(actualStep != STEP_LIFE_LED_EFFECT::OFF && actualStep != STEP_LIFE_LED_EFFECT::LOOP_STEP)
    {
      String s = "\nPlays\nEffect {},\nStep: {},\nRgbColors: {},\ndeltaTimesMs: {}\n";
      String sEffect = effect;
      String sLifeStep = stepLifeEffectEnomToString(actualStep);
      String colorString = vectorRgbColorToString(colorsEffect);
      String msg = formatMsg(s, {sEffect, sLifeStep, colorString, String(deltaTmsEffect)});
      String subject = formatMsg("EffectOrchestrator - {}", {name});
      serialService->logInfoln(msg, subject);
    }
  }

  if (!isOperative())
  {
    return false;
  }

  if (!running)
  {
    return false;
  }

  Effect *e = getEffectByEffectName(effect);

  if (e == nullptr)
  {
    Serial.println("ERROR Effetto non trovato: " + effect);
    return false;
  }

  boolean isExecutedCorrectly = false;

  switch (actualStep)
  {
  case STEP_LIFE_LED_EFFECT::BEGIN_STEP:
    isExecutedCorrectly = e->execStep(effect, STEP_LIFE_LED_EFFECT::BEGIN_STEP, colorsEffect, deltaTmsEffect, driver, typeLed, serialService);
    actualStep = STEP_LIFE_LED_EFFECT::LOOP_STEP;
    break;
  case STEP_LIFE_LED_EFFECT::LOOP_STEP:
    isExecutedCorrectly = e->execStep(effect, STEP_LIFE_LED_EFFECT::LOOP_STEP, colorsEffect, deltaTmsEffect, driver, typeLed, serialService);
    break;
  case STEP_LIFE_LED_EFFECT::END_STEP:
    isExecutedCorrectly = e->execStep(effect, STEP_LIFE_LED_EFFECT::END_STEP, colorsEffect, deltaTmsEffect, driver, typeLed, serialService);
    actualStep = STEP_LIFE_LED_EFFECT::OFF;
    break;
  default:
    e->off(driver, typeLed);
    isExecutedCorrectly = true;
    running = false;
    break;
  }
  return isExecutedCorrectly;
}

void EffectOrchestrator::startEffect(String effectInput, const std::vector<RgbColor> &colorsRgb, int deltaTms)
{
  if (serialService != nullptr)
  {
    String colorString = vectorRgbColorToString(colorsRgb);
    String msg = formatMsg("start: {}, colorsRgb: {}, deltaTms: {}", {effectInput, colorString, String(deltaTms)});
    String subject = formatMsg("EffectOrchestrator - {}", {name});
    serialService->logInfoln(msg, subject);
  }

  actualStep = STEP_LIFE_LED_EFFECT::BEGIN_STEP;
  effect = effectInput;
  deltaTmsEffect = deltaTms;
  colorsEffect = colorsRgb;
  running = true;
}
void EffectOrchestrator::stopEffect(String effectInput, const std::vector<RgbColor> &colorsRgb, int deltaTms)
{
  if (serialService != nullptr)
  {
    String colorString = vectorRgbColorToString(colorsRgb);
    String msg = formatMsg("stop: {}, colorsRgb: {}, deltaTms: {}", {effectInput, colorString, String(deltaTms)});
    String subject = formatMsg("EffectOrchestrator - {}", {name});
    serialService->logInfoln(msg, subject);
  }
  actualStep = STEP_LIFE_LED_EFFECT::END_STEP;
  effect = effectInput;
  deltaTmsEffect = deltaTms;
  colorsEffect = colorsRgb;
}

boolean EffectOrchestrator::isOperative()
{
  return operative;
}
void EffectOrchestrator::attachSerialService(SerialService *s)
{
  serialService = s;
}

void EffectOrchestrator::setOperative(boolean enable)
{
  operative = enable;
}

void EffectOrchestrator::setEffect(String effect)
{
  this->effect = effect;
}

void EffectOrchestrator::setDeltaTms(int ms)
{
  deltaTmsEffect = ms;
}

void EffectOrchestrator::setColors(std::vector<RgbColor> colors)
{
  colorsEffect = colors;
}

void EffectOrchestrator::setDriver(DriverLed *d)
{
  this->driver = d;
}
