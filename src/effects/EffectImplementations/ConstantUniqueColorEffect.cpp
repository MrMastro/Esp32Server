#include "ConstantUniqueColorEffect.h"

ConstantUniqueColorEffect::ConstantUniqueColorEffect()
{
}

String ConstantUniqueColorEffect::getName()
{
    return "CONSTANT_UNIQUE_COLOR";
}

int ConstantUniqueColorEffect::getColorInputQt()
{
    return 1;
}

void ConstantUniqueColorEffect::execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type)
{

    if(colorsInput.size() < getColorInputQt()){
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    for (int i = 0; i < driver->getMaxNumPixel(type); i++)
    {
        driver->sendStriptData(type, colorsInput[0], i);
    }
    driver->showData();

    if (ws2811StepInput == STEP_LIFE_EFFECT::OFF)
    {
        off(driver, type);
    }
}

void ConstantUniqueColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
