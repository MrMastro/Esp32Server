#include "WaweUniqueColorEffect.h"

WaweUniqueColorEffect::WaweUniqueColorEffect()
{
    inverted = false;
}

String WaweUniqueColorEffect::getName()
{
    return "WAWE_UNIQUE_COLOR";
}

int WaweUniqueColorEffect::getColorInputQt()
{
    return 1;
}

void WaweUniqueColorEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type)
{
    if (colorsInput.size() < getColorInputQt())
    {
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    RgbColor colorVariable = colorsInput[0];

    if (stepInput == STEP_LIFE_LED_EFFECT::BEGIN_STEP)
    {
        while (colorVariable.CalculateBrightness() < 0)
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
    else if (stepInput == STEP_LIFE_LED_EFFECT::LOOP_STEP)
    {
        //colorVariable = driver->getColorPixel(0, type);
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

            while (colorVariable.CalculateBrightness() < 0)
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
    else if (stepInput == STEP_LIFE_LED_EFFECT::END_STEP)
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
        off(driver, type);
    }
}

void WaweUniqueColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
