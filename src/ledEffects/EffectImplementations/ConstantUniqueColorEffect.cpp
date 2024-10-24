#include "ConstantUniqueColorEffect.h"

ConstantUniqueColorEffect::ConstantUniqueColorEffect()
{
}

String ConstantUniqueColorEffect::getName()
{
    return "CONSTANT_UNIQUE_COLOR";
}

int ConstantUniqueColorEffect::getMaxColorsNumber()
{
    return 1;
}

int ConstantUniqueColorEffect::getMinColorsNumber()
{
    return 1;
}

boolean ConstantUniqueColorEffect::getCompatibilityRgb(){
    return true;
}

boolean ConstantUniqueColorEffect::getCompatibilityWs2811(){
    return true;
}

void ConstantUniqueColorEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService)
{

    if(colorsInput.size() < getMinColorsNumber()){
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    for (int i = 0; i < driver->getMaxNumPixel(type); i++)
    {
        driver->sendStripData(type, colorsInput[0], i);
    }
    driver->showData();

    if (stepInput == STEP_LIFE_LED_EFFECT::OFF)
    {
        off(driver, type);
    }
}

void ConstantUniqueColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
