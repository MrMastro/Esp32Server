#include "MulticolorConstantEffect.h"

MulticolorConstantEffect::MulticolorConstantEffect() {}

String MulticolorConstantEffect::getName()
{
    return "MULTICOLOR_CONSTANT_EFFECT";
}

int MulticolorConstantEffect::getMaxColorsNumber()
{
    return 5;
}

int MulticolorConstantEffect::getMinColorsNumber()
{
    return 2;
}

boolean MulticolorConstantEffect::getCompatibilityRgb(){
    return false;
}

boolean MulticolorConstantEffect::getCompatibilityWs2811(){
    return true;
}

void MulticolorConstantEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type, SerialService* serialService)
{

    if(colorsInput.size() < getMinColorsNumber()){
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    if (driver == nullptr)
    {
        return;
    }

    switch (stepInput)
    {
    case STEP_LIFE_LED_EFFECT::BEGIN_STEP:
    break;

    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
        // Alterna i colori dei LED durante il loop dell'effetto
        for (size_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            RgbColor currentColor = colorsInput[i % colorsInput.size()];
            driver->sendStripData(type, currentColor, i);
        }
        driver->showData();
        break;

    case STEP_LIFE_LED_EFFECT::END_STEP:
    break;

    case STEP_LIFE_LED_EFFECT::OFF:
        // Spegne tutti i LED
        off(driver, type);
        break;

    default:
        break;
    }
}

void MulticolorConstantEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    if (driver == nullptr)
    {
        return;
    }
    driver->clear(type);
}