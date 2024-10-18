#include "ProgressiveBarUniqueColorEffect.h"

ProgressiveBarUniqueColorEffect::ProgressiveBarUniqueColorEffect()
{
}

String ProgressiveBarUniqueColorEffect::getName()
{
    return "PROGRESSIVE_BAR_UNIQUE_COLOR";
}

void ProgressiveBarUniqueColorEffect::execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput,const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type)
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

void ProgressiveBarUniqueColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}
