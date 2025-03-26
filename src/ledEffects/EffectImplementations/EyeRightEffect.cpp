#include "EyeRightEffect.h"

EyeRightEffect::EyeRightEffect() : brightness(255), increasing(false) {} // Inizializza la luminosità massima

String EyeRightEffect::getName()
{
    return "EYE_RIGHT";
}

int EyeRightEffect::getMaxColorsNumber()
{
    return 1;
}

int EyeRightEffect::getMinColorsNumber()
{
    return 1;
}

boolean EyeRightEffect::getCompatibilityRgb(){
    return false;
}

boolean EyeRightEffect::getCompatibilityWs2811(){
    return true;
}

boolean EyeRightEffect::getCompatibilityWs2811Matrix(){
    return true;
}

boolean EyeRightEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
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
            for (int i = 0; i < 64; i++) {
                // Imposta il colore (0, 0, 0) per i LED da 0 a 50
                if (i <= 50) {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
                // Imposta il colore (255, 255, 255) per i LED 51, 52, 59, 60
                else if (i == 51 || i == 52 || i == 59 || i == 60) {
                    driver->sendStripData(type, RgbColor(255, 255, 255), i);
                }
                // Imposta il colore (0, 0, 0) per gli altri LED
                else {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
            }
        }
        if (driver->getMaxNumPixel(type) >= 128) {
            for (int i = 64; i <= 167; i++) {
                // Imposta il colore (0, 0, 0) per i LED da 64 a 166
                if (i <= 166) {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
                // Imposta il colore (255, 255, 255) per i LED 167 e 168
                else if (i == 167 || i == 168) {
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


void EyeRightEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}