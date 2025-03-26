#include "EyeDownEffect.h"

EyeDownEffect::EyeDownEffect() : brightness(255), increasing(false) {} // Inizializza la luminosità massima

String EyeDownEffect::getName()
{
    return "EYE_DOWN";
}

int EyeDownEffect::getMaxColorsNumber()
{
    return 1;
}

int EyeDownEffect::getMinColorsNumber()
{
    return 1;
}

boolean EyeDownEffect::getCompatibilityRgb(){
    return false;
}

boolean EyeDownEffect::getCompatibilityWs2811(){
    return true;
}

boolean EyeDownEffect::getCompatibilityWs2811Matrix(){
    return true;
}

boolean EyeDownEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
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
                if (i <= 60) {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
                else if (i >= 61 && i <= 63) {
                    driver->sendStripData(type, RgbColor(255, 255, 255), i);
                }
            }
        }
    
        if (driver->getMaxNumPixel(type) >= 128) {
            for (int i = 64; i <= 167; i++) {
                if (i <= 126) {
                    driver->sendStripData(type, RgbColor(0, 0, 0), i);
                }
                else if (i >= 127 && i <= 130) {
                    driver->sendStripData(type, RgbColor(255, 255, 255), i);
                }
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


void EyeDownEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}