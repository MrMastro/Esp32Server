#include "ProgressiveBarUniqueColorEffect.h"

ProgressiveBarUniqueColorEffect::ProgressiveBarUniqueColorEffect()
{
}

String ProgressiveBarUniqueColorEffect::getName()
{
    return "PROGRESSIVE_BAR_UNIQUE_COLOR";
}

int ProgressiveBarUniqueColorEffect::getMaxColorsNumber()
{
    return 5;
}

int ProgressiveBarUniqueColorEffect::getMinColorsNumber()
{
    return 1;
}

void ProgressiveBarUniqueColorEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService)
{
    if(colorsInput.size() < getMinColorsNumber()){
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    RgbColor colorInput = colorsInput[0];
    if (stepInput == STEP_LIFE_LED_EFFECT::BEGIN_STEP)
    {
        for (int pixel = 0; pixel < driver->getMaxNumPixel(type); pixel++)
        {
            driver->sendStriptData(type,colorInput,pixel);
            driver->showData();
            delay(deltaTimeMsInput);
        }
    }
    else if (stepInput == STEP_LIFE_LED_EFFECT::LOOP_STEP)
    {
        for (int pixel = 0; pixel < driver->getMaxNumPixel(type); pixel++)
        {
            driver->sendStriptData(type,colorInput,pixel);
        }
        driver->showData();
    }
    else if (stepInput == STEP_LIFE_LED_EFFECT::END_STEP)
    {
        //???
        // for (int light = 255; light == 0; --light)
        // {
        //     RgbColor darken = ws2811Stript->GetPixelColor(0);
        //     darken.Darken(light);
        //     ws2811Stript->ClearTo(darken);
        //     delay(deltaTimeMsInput);
        //     ws2811Stript->Show();
        // }
        off(driver,type);
    }
}

void ProgressiveBarUniqueColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
