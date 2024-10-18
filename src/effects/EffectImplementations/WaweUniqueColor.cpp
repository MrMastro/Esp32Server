#include "WaweUniqueColor.h"

WaweUniqueColorEffect::WaweUniqueColorEffect()
{
}

String WaweUniqueColorEffect::getName()
{
    return "WAWE_UNIQUE_COLOR";
}

void WaweUniqueColorEffect::execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, std::vector<RgbColor> colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type)
{
    RgbColor initialColor = colorsInput[0];
    RgbColor colorVariable = colorsInput[0];

    if (ws2811StepInput == STEP_LIFE_EFFECT::BEGIN_STEP)
    {
        while (colorVariable.CalculateBrightness() > 0)
        {
            colorVariable.Darken(10);
            for (int i = 0; i < driver->getMaxNumPixel(type); i++)
            {
                driver->sendStriptData(type, colorVariable, i);
            }
            driver->showData();
            delay(deltaTimeMsInput);
        }
    }
    else if (ws2811StepInput == STEP_LIFE_EFFECT::LOOP_STEP)
    {
        while (colorVariable.CalculateBrightness() < initialColor.CalculateBrightness())
        {
            colorVariable.Lighten(10);
            for (int i = 0; i < driver->getMaxNumPixel(type); i++)
            {
                driver->sendStriptData(type, colorVariable, i);
            }
            driver->showData();
            delay(deltaTimeMsInput);
        }
    }
    else if (ws2811StepInput == STEP_LIFE_EFFECT::END_STEP)
    {
        off(driver, type);
    }
}

void WaweUniqueColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
