#include "ProgressiveBarUniqueColor.h"

ProgressiveBarUniqueColor::ProgressiveBarUniqueColor()
{
}

String ProgressiveBarUniqueColor::getName()
{
    return "PROGRESSIVE_BAR_UNIQUE_COLOR";
}

void ProgressiveBarUniqueColor::execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, std::vector<RgbColor> colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type)
{
    RgbColor colorInput = colorsInput[0];
    if (ws2811StepInput == STEP_LIFE_EFFECT::BEGIN_STEP)
    {
        for (int pixel = 0; pixel < driver->getMaxNumPixel(type); pixel++)
        {
            driver->sendStriptData(type,colorInput,pixel);
            driver->showData();
            delay(deltaTimeMsInput);
        }
    }
    else if (ws2811StepInput == STEP_LIFE_EFFECT::LOOP_STEP)
    {
        for (int pixel = 0; pixel < driver->getMaxNumPixel(type); pixel++)
        {
            driver->sendStriptData(type,colorInput,pixel);
        }
        driver->showData();
    }
    else if (ws2811StepInput == STEP_LIFE_EFFECT::END_STEP)
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

void ProgressiveBarUniqueColor::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
