#include "EyeUpEffect.h"
#include "../CommonEffectObject.h"

EyeUpEffect::EyeUpEffect() : brightness(255), increasing(false) {} // Inizializza la luminosità massima

String EyeUpEffect::getName()
{
    return "EYE_UP";
}

int EyeUpEffect::getMaxColorsNumber()
{
    return 1;
}

int EyeUpEffect::getMinColorsNumber()
{
    return 1;
}

boolean EyeUpEffect::getCompatibilityRgb(){
    return false;
}

boolean EyeUpEffect::getCompatibilityWs2811(){
    return true;
}

boolean EyeUpEffect::getCompatibilityWs2811Matrix(){
    return true;
}

boolean EyeUpEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
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

    std::vector<std::vector<int>> ledMatrix = {
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 1, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0}
    };

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


void EyeUpEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}

void EyeUpEffect::displayLedMatrix(std::vector<std::vector<int>>& matrix, int rows, int cols,int offset, DriverLed *driver, TYPE_STRIP type)
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
            valueMatrix = matrix[i][j];
            if(valueMatrix == 1){
                driver->sendStripData(type, RgbColor(255, 255, 255), linearIndex+offset);
            }else{
                driver->sendStripData(type, RgbColor(0, 0, 0), linearIndex+offset);
            }
        }
        
    }
    

    driver->showData(type);
}

// int EyeUpEffect::getValore(std::vector<std::vector<int>>& matrix, int x, int y) {
//     // Calcoliamo l'indice nell'array unidimensionale
//     int index = x * 8 + y;
//     return *(matrix + index); // Restituisce il valore corrispondente all'indice
// }
