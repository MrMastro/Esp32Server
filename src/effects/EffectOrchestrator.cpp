#include "Effect.h"

void EffectOrchestrator::runLifeCycle()
{

Effect* e = getEffectByName("");

switch (actualStep)
  {
  case STEP_LIFE_EFFECT::BEGIN_STEP:
    e->execStep(effect, STEP_LIFE_EFFECT::BEGIN_STEP, colorsEffect, deltaTmsEffect, driver); //execWs2811Effect(ws2811Effect, STEP_LIFE_EFFECT::BEGIN_STEP, colorsEffect, deltaTmsEffect);
    actualStep = STEP_LIFE_EFFECT::LOOP_STEP;
    break;
  case STEP_LIFE_EFFECT::LOOP_STEP:
    e->execStep(effect, STEP_LIFE_EFFECT::LOOP_STEP, colorsEffect, deltaTmsEffect, driver); //execWs2811Effect(ws2811Effect, STEP_LIFE_EFFECT::LOOP_STEP, colorsEffect, deltaTmsEffect);
    break;
  case STEP_LIFE_EFFECT::END_STEP:
    e->execStep(effect, STEP_LIFE_EFFECT::END_STEP, colorsEffect, deltaTmsEffect, driver); //execWs2811Effect(ws2811Effect, STEP_LIFE_EFFECT::END_STEP, colorsEffect, deltaTmsEffect);
    actualStep = STEP_LIFE_EFFECT::OFF;
    break;
  default:
    e->off();
    break;
  }

}

void EffectOrchestrator::setEffect(WS2811_EFFECT effect)
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
