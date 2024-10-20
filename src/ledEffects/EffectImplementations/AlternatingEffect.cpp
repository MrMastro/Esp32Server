#include "AlternatingEffect.h"

AlternatingEffect::AlternatingEffect() {}

String AlternatingEffect::getName()
{
    return "ALTERNATING_EFFECT";
}

int AlternatingEffect::getColorInputQt()
{
    return 2;
}

void AlternatingEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type)
{

    if(colorsInput.size() < getColorInputQt()){
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
            RgbColor currentColor = (i % 2 == 0) ? colorsInput[0] : colorsInput[1];
            driver->sendStriptData(type, currentColor, i);
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

void AlternatingEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    if (driver == nullptr)
    {
        return;
    }
    driver->clear(type);
}