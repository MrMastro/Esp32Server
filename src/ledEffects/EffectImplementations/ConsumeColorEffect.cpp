#include "ConsumeColorEffect.h"

ConsumeColorEffect::ConsumeColorEffect()
{
    inverted = false;
}

String ConsumeColorEffect::getName()
{
    return "CONSUME_COLOR";
}

int ConsumeColorEffect::getMaxColorsNumber()
{
    return 1;
}

int ConsumeColorEffect::getMinColorsNumber()
{
    return 1;
}

void ConsumeColorEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService)
{
    if (colorsInput.size() < getMinColorsNumber())
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
            // String s = "[" + String(colorVariable.R) + String(colorVariable.G) + String(colorVariable.B) + "]";
            // serialService->logInfoFixed(s, formatMsg("Effect - {}", {getName()}));
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
                String s = "[" + String(colorVariable.R) +" , "+ String(colorVariable.G) +" , "+ String(colorVariable.B) + "]";
                serialService->logInfoFixed(s, formatMsg("Effect - {}", {getName()}));
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

void ConsumeColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
