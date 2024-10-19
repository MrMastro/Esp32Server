#include "./EffectOrchestrator.h"

EffectOrchestrator::EffectOrchestrator()
{
  driver = nullptr;
  operative = false;

}

EffectOrchestrator::EffectOrchestrator(DriverLed *driverInput, TYPE_STRIP typeledInput)
{
  driver = driverInput;
  typeLed = typeledInput;
}

EffectOrchestrator::EffectOrchestrator(DriverLed *driverInput, TYPE_STRIP typeledInput, boolean enable)
{
  driver = driverInput;
  typeLed = typeledInput;
  operative = enable;

}

void EffectOrchestrator::runLifeCycle()
{

if(!isOperative()){
  return;
}

if(!running)
{
  return;
}

Effect* e = getEffectByEffectName(effect);

if(e == nullptr){
  Serial.println("ERROR Effetto non trovato: " + effect);
  delay(1000);
  return;
}

switch (actualStep)
  {
  case STEP_LIFE_EFFECT::BEGIN_STEP:
    e->execStep(effect, STEP_LIFE_EFFECT::BEGIN_STEP, colorsEffect, deltaTmsEffect, driver, typeLed);
    actualStep = STEP_LIFE_EFFECT::LOOP_STEP;
    break;
  case STEP_LIFE_EFFECT::LOOP_STEP:
    e->execStep(effect, STEP_LIFE_EFFECT::LOOP_STEP, colorsEffect, deltaTmsEffect, driver, typeLed);
    break;
  case STEP_LIFE_EFFECT::END_STEP:
    e->execStep(effect, STEP_LIFE_EFFECT::END_STEP, colorsEffect, deltaTmsEffect, driver, typeLed);
    actualStep = STEP_LIFE_EFFECT::OFF;
    break;
  default:
    e->off(driver, typeLed);
    running = false;
    break;
  }

}

void EffectOrchestrator::startEffect(String effectInput, const std::vector<RgbColor> &colorsRgb, int deltaTms)
{
  actualStep = STEP_LIFE_EFFECT::BEGIN_STEP;
  effect = effectInput;
  deltaTmsEffect = deltaTms;
  colorsEffect = colorsRgb;
  running = true;
}
void EffectOrchestrator::stopEffect(String effectInput, const std::vector<RgbColor> &colorsRgb, int deltaTms)
{
  actualStep = STEP_LIFE_EFFECT::END_STEP;
  effect = effectInput;
  deltaTmsEffect = deltaTms;
  colorsEffect = colorsRgb;
}

boolean EffectOrchestrator::isOperative()
{
return operative;
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
