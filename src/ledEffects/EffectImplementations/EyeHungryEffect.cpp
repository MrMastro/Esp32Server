#include "EyeHungryEffect.h"
#include "../CommonEffectObject.h"

EyeHungryEffect::EyeHungryEffect() : brightness(255), bluHungry(true) {} // Inizializza la luminosità massima

String EyeHungryEffect::getName()
{
    return "EYE_HUNGRY";
}

int EyeHungryEffect::getMaxColorsNumber()
{
    return 1;
}

int EyeHungryEffect::getMinColorsNumber()
{
    return 1;
}

boolean EyeHungryEffect::getCompatibilityRgb(){
    return false;
}

boolean EyeHungryEffect::getCompatibilityWs2811(){
    return true;
}

boolean EyeHungryEffect::getCompatibilityWs2811Matrix(){
    return true;
}

boolean EyeHungryEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
{
    std::vector<std::vector<RgbColor>> hungryBlue = {
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 255),
         RgbColor(0, 0, 255), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255),
         RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 255),
         RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0),
         RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0),
         RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 255),
         RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 255), RgbColor(0, 0, 255),
         RgbColor(0, 0, 255), RgbColor(0, 0, 255), RgbColor(0, 0, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 255),
         RgbColor(0, 0, 255), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0)}
    };
    
    std::vector<std::vector<RgbColor>> hungryYellow = {
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(255, 255, 0),
         RgbColor(255, 255, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0),
         RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0),
         RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0),
         RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0),
         RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0),
         RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(255, 255, 0), RgbColor(255, 255, 0),
         RgbColor(255, 255, 0), RgbColor(255, 255, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0)},
        {RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(255, 255, 0),
         RgbColor(255, 255, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0), RgbColor(0, 0, 0)}
    };

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
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            driver->sendStripData(type, RgbColor(0, 0, 0), i);
        }
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
        // if (driver->getMaxNumPixel(type) >= 64){
        //     if(bluHungry){
        //         displayLedMatrix(hungryBlue, 8, 8, 0, driver, type);
        //     }else{
        //         displayLedMatrix(hungryYellow, 8, 8, 0, driver, type);
        //     }
        // } 
        if (driver->getMaxNumPixel(type) >= 128){
            if(bluHungry){
                displayLedMatrix(hungryBlue, 8, 8, 64, driver, type);
            }else{
                displayLedMatrix(hungryYellow, 8, 8, 64, driver, type);
            }
        } 
        bluHungry = !bluHungry;
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::END_STEP:
        // Alla fine, imposta tutti i LED su spento
        brightness = 0;
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            driver->sendStripData(type, RgbColor(0, 0, 0), i);
        }
        driver->showData(type);
        break;

    case STEP_LIFE_LED_EFFECT::OFF:
        off(driver, type);
        break;

    default:
        break;
    }
    return true;
}


void EyeHungryEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}

void EyeHungryEffect::displayLedMatrix(std::vector<std::vector<RgbColor>>& matrix, int rows, int cols,int offset, DriverLed *driver, TYPE_STRIP type)
{

    int linearIndex = 0;
    int valueMatrix = 0;
    int i=0;
    int j=0;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            linearIndex = LED_MATRIX_POSITIONS[i][j];
            driver->sendStripData(type, matrix[i][j], linearIndex+offset);
        }
        
    }
    
    driver->showData(type);
}