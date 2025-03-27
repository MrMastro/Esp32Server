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
    std::vector<std::vector<int>> ledMatrix = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 1, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
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
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
        if(driver->getMaxNumPixel(type) >= 64){
            displayLedMatrix(ledMatrix, 8, 8, 0, driver, type);
        }
        if (driver->getMaxNumPixel(type) >= 128) {
            displayLedMatrix(ledMatrix, 8, 8, 64, driver, type);
        }
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

void EyeLeftEffect::displayLedMatrix(std::vector<std::vector<int>>& matrix, int rows, int cols,int offset, DriverLed *driver, TYPE_STRIP type)
{

    std::vector<std::vector<int>> positions = {
        {56, 55, 40, 39, 24, 23, 8, 7}, 
        {57, 54, 41, 38, 25, 22, 9, 6}, 
        {58, 53, 42, 37, 26, 21, 10, 5}, 
        {59, 52, 43, 36, 27, 20, 11, 4}, 
        {60, 51, 44, 35, 28, 19, 12, 3}, 
        {61, 50, 45, 34, 29, 18, 13, 2}, 
        {62, 49, 46, 33, 30, 17, 14, 1}, 
        {63, 48, 47, 32, 31, 16, 15, 0}
    };

    int linearIndex = 0;
    int valueMatrix = 0;
    int i=0;
    int j=0;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++)
        {
            linearIndex = positions[i][j];
            valueMatrix = matrix[i][j];
            if(valueMatrix == 1){
                driver->sendStripData(type, RgbColor(255, 255, 255), linearIndex+offset);
            }else{
                driver->sendStripData(type, RgbColor(0, 0, 0), linearIndex+offset);
            }
        }
        
    }
    
    driver->showData();
}