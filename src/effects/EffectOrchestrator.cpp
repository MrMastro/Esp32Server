#include "./EffectOrchestrator.h"

EffectOrchestrator::EffectOrchestrator(): driver(nullptr), typeLed(), operative(false) {}
EffectOrchestrator::EffectOrchestrator(DriverLed *driver, TYPE_STRIP typeled): driver(driver), typeLed(typeLed), operative(false) {}
EffectOrchestrator::EffectOrchestrator(DriverLed *driver, TYPE_STRIP typeled, boolean enable): driver(driver), typeLed(typeLed), operative(enable) {}

void EffectOrchestrator::runLifeCycle()
{

if(!isOperative()){
  return;
}

String nameEffect =  LabelEffectEnumToString(effect);

Serial.println(nameEffect);

Effect* e = getEffectByName(nameEffect);

if(e == nullptr){
  Serial.println("ERROR Effetto non trovato");
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
  case STEP_LIFE_EFFECT::END_STEP:
    e->execStep(effect, STEP_LIFE_EFFECT::END_STEP, colorsEffect, deltaTmsEffect, driver, typeLed);
    actualStep = STEP_LIFE_EFFECT::OFF;
    break;
  default:
    e->off(driver, typeLed);
    break;
  }

}

void EffectOrchestrator::startEffect(EFFECT_LABEL effectInput, const std::vector<RgbColor> &colorsRgb, int deltaTms)
{
  actualStep = STEP_LIFE_EFFECT::BEGIN_STEP;
  effect = effectInput;
  deltaTmsEffect = deltaTms;
  colorsEffect = colorsRgb;

}
void EffectOrchestrator::stopEffect(EFFECT_LABEL effectInput, const std::vector<RgbColor> &colorsRgb, int deltaTms)
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

void EffectOrchestrator::setEffect(EFFECT_LABEL effect)
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

void EffectOrchestrator::addEffect(Effect* e)
{
    allEffects.push_back(e);
}

void EffectOrchestrator::setDriver(DriverLed *d)
{
    this->driver = d;
}

Effect* EffectOrchestrator::getEffectByName(String name)
{
    for (size_t i = 0; i < allEffects.size(); ++i) {
        if(allEffects[i]->getName() == name){
            return allEffects[i];
        }
    }
    return nullptr;
}
