#include "EyeLeftEffect.h"

EyeLeftEffect::EyeLeftEffect() : brightness(255), increasing(false) {} // Inizializza la luminosità massima

String EyeLeftEffect::getName()
{
    return "EYE_LEFT";
}

int EyeLeftEffect::getMaxColorsNumber()
{
    return 1;
}

int EyeLeftEffect::getMinColorsNumber()
{
    return 1;
}

boolean EyeLeftEffect::getCompatibilityRgb(){
    return false;
}

boolean EyeLeftEffect::getCompatibilityWs2811(){
    return true;
}

boolean EyeLeftEffect::getCompatibilityWs2811Matrix(){
    return true;
}

boolean EyeLeftEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
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
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
        if(driver->getMaxNumPixel(type) >= 64){
            // Ciclo per i LED da 0 a 63
            for (int i = 0; i < 64; i++) {
                // Imposta il colore (0, 0, 0) per i LED da 0 a 62
                if (i <= 62) {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
                // Imposta il colore (255, 255, 255) per i LED 63
                else if (i == 63) {
                    driver->sendStripData(type, RgbColor(255, 255, 255), i);
                }
            }
        }
        if (driver->getMaxNumPixel(type) >= 128) {
            // Ciclo per i LED da 64 a 167
            for (int i = 64; i <= 167; i++) {
                // Imposta il colore (0, 0, 0) per i LED da 64 a 126
                if (i <= 126) {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
                // Imposta il colore (255, 255, 255) per i LED 127 e 128
                else if (i == 127 || i == 128) {
                    driver->sendStripData(type, RgbColor(255, 255, 255), i);
                }
                // Imposta il colore (0, 0, 0) per gli altri LED
                else {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
            }
        }
        driver->showData();
        break;

    case STEP_LIFE_LED_EFFECT::END_STEP:
        // Alla fine, imposta tutti i LED su spento
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


void EyeLeftEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}