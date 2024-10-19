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

void AlternatingEffect::execStep(EFFECT_LABEL ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput,
                                 const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type)
{

    if(colorsInput.size() < getColorInputQt()){
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    if (driver == nullptr)
    {
        return;
    }

    switch (ws2811StepInput)
    {
    case STEP_LIFE_EFFECT::BEGIN_STEP:
        // Esegui eventuali inizializzazioni necessarie all'inizio dell'effetto
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            driver->sendStriptData(type, RgbColor(255, 0, 0), i); // Imposta tutti i LED a rosso
            driver->showData();
        }
        break;

    case STEP_LIFE_EFFECT::LOOP_STEP:
        // Alterna i colori dei LED durante il loop dell'effetto
        for (size_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            RgbColor currentColor = (i % 2 == 0) ? colorsInput[0] : colorsInput[1];
            driver->sendStriptData(type, currentColor, i);
        }
        driver->showData();
        break;

    case STEP_LIFE_EFFECT::END_STEP:
        // Esegui eventuali azioni alla fine dell'effetto
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            driver->sendStriptData(type, RgbColor(0, 255, 0), i); // Imposta tutti i LED a verde per fine
        }
        driver->showData();
        break;

    case STEP_LIFE_EFFECT::OFF:
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