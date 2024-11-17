#include "BreathEffect.h"

BreathEffect::BreathEffect() : brightness(255), increasing(false), minBrightnessThresholdPercent(0) {} // Inizializza la luminosità massima

String BreathEffect::getName()
{
    return "BREATH_EFFECT";
}

int BreathEffect::getMaxColorsNumber()
{
    return 5;
}

int BreathEffect::getMinColorsNumber()
{
    return 1;
}

boolean BreathEffect::getCompatibilityRgb(){
    return true;
}

boolean BreathEffect::getCompatibilityWs2811(){
    return true;
}


boolean BreathEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
{

    if (driver == nullptr)
    {
        return false;
    }

    if (colorsInput.size() < getMinColorsNumber())
    {
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return false;
    }

    switch (stepInput)
    {
    case STEP_LIFE_LED_EFFECT::BEGIN_STEP:
        increasing = true;
        brightness = 0;
        minBrightnessThresholdPercent = 25;
        // Inizia con la dissolvenza dal nero (tutti spenti)
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            RgbColor color = RgbColor(0, 0, 0);
            driver->sendStripData(type, color, i);
        }
        driver->showData();
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
        // Fai variare la luminosità dei LED
        for (size_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            RgbColor baseColor = colorsInput[i % colorsInput.size()];
            // Applica il fattore di luminosità al colore base
            RgbColor fadedColor = RgbColor(uint8_t(baseColor.R * brightness),
                                           uint8_t(baseColor.G * brightness),
                                           uint8_t(baseColor.B * brightness));
            driver->sendStripData(type, fadedColor, i);
        }
        driver->showData();

        // Aggiorna il fattore di luminosità
        if (increasing)
        {
            brightness += (0.01f / (deltaTimeMsInput / 1000.0f));
            if (brightness >= 1.0f)
            {
                brightness = 1.0f;
                increasing = false; // Inizia a decrescere
            }
        }
        else
        {
            brightness -= (0.01f / (deltaTimeMsInput / 1000.0f));
            //if (brightness <= 0.0f)
            if (brightness <= minBrightnessThresholdPercent / 100)
            {
                brightness = minBrightnessThresholdPercent / 100;
                increasing = true; // Inizia a crescere
            }
        }
        break;

    case STEP_LIFE_LED_EFFECT::END_STEP:
        // Alla fine, imposta tutti i LED su spento
        increasing = false;
        brightness = 0;
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            driver->sendStripData(type, RgbColor(0, 0, 0), i);
        }
        driver->showData();
        break;

    case STEP_LIFE_LED_EFFECT::OFF:
        off(driver, type);
        break;

    default:
        break;
    }
    return true;
}


void BreathEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}