#include "EyeMidEffect.h"
//#include "../CommonEffectObject.h"

EyeMidEffect::EyeMidEffect() : brightness(255), increasing(false) {} // Inizializza la luminosità massima

String EyeMidEffect::getName()
{
    return "EYE_MID";
}

int EyeMidEffect::getMaxColorsNumber()
{
    return 1;
}

int EyeMidEffect::getMinColorsNumber()
{
    return 1;
}

boolean EyeMidEffect::getCompatibilityRgb(){
    return false;
}

boolean EyeMidEffect::getCompatibilityWs2811(){
    return true;
}

boolean EyeMidEffect::getCompatibilityWs2811Matrix(){
    return true;
}

boolean EyeMidEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
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
            driver->sendStripData(type, RgbColor(0,0,0),0);
            driver->sendStripData(type, RgbColor(0,0,0),1);
            driver->sendStripData(type, RgbColor(0,0,0),2);
            driver->sendStripData(type, RgbColor(0,0,0),3);
            driver->sendStripData(type, RgbColor(0,0,0),4);
            driver->sendStripData(type, RgbColor(0,0,0),5);
            driver->sendStripData(type, RgbColor(0,0,0),6);
            driver->sendStripData(type, RgbColor(0,0,0),7);
            driver->sendStripData(type, RgbColor(0,0,0),8);
            driver->sendStripData(type, RgbColor(0,0,0),9);
            driver->sendStripData(type, RgbColor(0,0,0),10);
            driver->sendStripData(type, RgbColor(0,0,0),11);
            driver->sendStripData(type, RgbColor(0,0,0),12);
            driver->sendStripData(type, RgbColor(0,0,0),13);
            driver->sendStripData(type, RgbColor(0,0,0),14);
            driver->sendStripData(type, RgbColor(0,0,0),15);
            driver->sendStripData(type, RgbColor(0,0,0),16);
            driver->sendStripData(type, RgbColor(0,0,0),17);
            driver->sendStripData(type, RgbColor(0,0,0),18);
            driver->sendStripData(type, RgbColor(0,0,0),19);
            driver->sendStripData(type, RgbColor(0,0,0),20);
            driver->sendStripData(type, RgbColor(0,0,0),21);
            driver->sendStripData(type, RgbColor(0,0,0),22);
            driver->sendStripData(type, RgbColor(0,0,0),23);
            driver->sendStripData(type, RgbColor(0,0,0),24);
            driver->sendStripData(type, RgbColor(0,0,0),25);
            driver->sendStripData(type, RgbColor(0,0,0),26);
            driver->sendStripData(type, RgbColor(255, 255, 255),27);
            driver->sendStripData(type, RgbColor(255, 255, 255),28);
            driver->sendStripData(type, RgbColor(0,0,0),29);
            driver->sendStripData(type, RgbColor(0,0,0),30);
            driver->sendStripData(type, RgbColor(0,0,0),31);
            driver->sendStripData(type, RgbColor(0,0,0),32);
            driver->sendStripData(type, RgbColor(0,0,0),33);
            driver->sendStripData(type, RgbColor(0,0,0),34);
            driver->sendStripData(type, RgbColor(255, 255, 255),35);
            driver->sendStripData(type, RgbColor(255, 255, 255),36);
            driver->sendStripData(type, RgbColor(0,0,0),37);
            driver->sendStripData(type, RgbColor(0,0,0),38);
            driver->sendStripData(type, RgbColor(0,0,0),39);
            driver->sendStripData(type, RgbColor(0,0,0),40);
            driver->sendStripData(type, RgbColor(0,0,0),41);
            driver->sendStripData(type, RgbColor(0,0,0),42);
            driver->sendStripData(type, RgbColor(0,0,0),43);
            driver->sendStripData(type, RgbColor(0,0,0),44);
            driver->sendStripData(type, RgbColor(0,0,0),45);
            driver->sendStripData(type, RgbColor(0,0,0),46);
            driver->sendStripData(type, RgbColor(0,0,0),47);
            driver->sendStripData(type, RgbColor(0,0,0),48);
            driver->sendStripData(type, RgbColor(0,0,0),49);
            driver->sendStripData(type, RgbColor(0,0,0),50);
            driver->sendStripData(type, RgbColor(0,0,0),51);
            driver->sendStripData(type, RgbColor(0,0,0),52);
            driver->sendStripData(type, RgbColor(0,0,0),53);
            driver->sendStripData(type, RgbColor(0,0,0),54);
            driver->sendStripData(type, RgbColor(0,0,0),55);
            driver->sendStripData(type, RgbColor(0,0,0),56);
            driver->sendStripData(type, RgbColor(0,0,0),57);
            driver->sendStripData(type, RgbColor(0,0,0),58);
            driver->sendStripData(type, RgbColor(0,0,0),59);
            driver->sendStripData(type, RgbColor(0,0,0),60);
            driver->sendStripData(type, RgbColor(0,0,0),61);
            driver->sendStripData(type, RgbColor(0,0,0),62);
            driver->sendStripData(type, RgbColor(0,0,0),63);
        }
        if(driver->getMaxNumPixel(type) >= 128){
            driver->sendStripData(type, RgbColor(0,0,0),0+64);
            driver->sendStripData(type, RgbColor(0,0,0),1+64);
            driver->sendStripData(type, RgbColor(0,0,0),2+64);
            driver->sendStripData(type, RgbColor(0,0,0),3+64);
            driver->sendStripData(type, RgbColor(0,0,0),4+64);
            driver->sendStripData(type, RgbColor(0,0,0),5+64);
            driver->sendStripData(type, RgbColor(0,0,0),6+64);
            driver->sendStripData(type, RgbColor(0,0,0),7+64);
            driver->sendStripData(type, RgbColor(0,0,0),8+64);
            driver->sendStripData(type, RgbColor(0,0,0),9+64);
            driver->sendStripData(type, RgbColor(0,0,0),10+64);
            driver->sendStripData(type, RgbColor(0,0,0),11+64);
            driver->sendStripData(type, RgbColor(0,0,0),12+64);
            driver->sendStripData(type, RgbColor(0,0,0),13+64);
            driver->sendStripData(type, RgbColor(0,0,0),14+64);
            driver->sendStripData(type, RgbColor(0,0,0),15+64);
            driver->sendStripData(type, RgbColor(0,0,0),16+64);
            driver->sendStripData(type, RgbColor(0,0,0),17+64);
            driver->sendStripData(type, RgbColor(0,0,0),18+64);
            driver->sendStripData(type, RgbColor(0,0,0),19+64);
            driver->sendStripData(type, RgbColor(0,0,0),20+64);
            driver->sendStripData(type, RgbColor(0,0,0),21+64);
            driver->sendStripData(type, RgbColor(0,0,0),22+64);
            driver->sendStripData(type, RgbColor(0,0,0),23+64);
            driver->sendStripData(type, RgbColor(0,0,0),24+64);
            driver->sendStripData(type, RgbColor(0,0,0),25+64);
            driver->sendStripData(type, RgbColor(0,0,0),26+64);
            driver->sendStripData(type, RgbColor(255, 255, 255),27+64);
            driver->sendStripData(type, RgbColor(255, 255, 255),28+64);
            driver->sendStripData(type, RgbColor(0,0,0),29+64);
            driver->sendStripData(type, RgbColor(0,0,0),30+64);
            driver->sendStripData(type, RgbColor(0,0,0),31+64);
            driver->sendStripData(type, RgbColor(0,0,0),32+64);
            driver->sendStripData(type, RgbColor(0,0,0),33+64);
            driver->sendStripData(type, RgbColor(0,0,0),34+64);
            driver->sendStripData(type, RgbColor(255, 255, 255),35+64);
            driver->sendStripData(type, RgbColor(255, 255, 255),36+64);
            driver->sendStripData(type, RgbColor(0,0,0),37+64);
            driver->sendStripData(type, RgbColor(0,0,0),38+64);
            driver->sendStripData(type, RgbColor(0,0,0),39+64);
            driver->sendStripData(type, RgbColor(0,0,0),40+64);
            driver->sendStripData(type, RgbColor(0,0,0),41+64);
            driver->sendStripData(type, RgbColor(0,0,0),42+64);
            driver->sendStripData(type, RgbColor(0,0,0),43+64);
            driver->sendStripData(type, RgbColor(0,0,0),44+64);
            driver->sendStripData(type, RgbColor(0,0,0),45+64);
            driver->sendStripData(type, RgbColor(0,0,0),46+64);
            driver->sendStripData(type, RgbColor(0,0,0),47+64);
            driver->sendStripData(type, RgbColor(0,0,0),48+64);
            driver->sendStripData(type, RgbColor(0,0,0),49+64);
            driver->sendStripData(type, RgbColor(0,0,0),50+64);
            driver->sendStripData(type, RgbColor(0,0,0),51+64);
            driver->sendStripData(type, RgbColor(0,0,0),52+64);
            driver->sendStripData(type, RgbColor(0,0,0),53+64);
            driver->sendStripData(type, RgbColor(0,0,0),54+64);
            driver->sendStripData(type, RgbColor(0,0,0),55+64);
            driver->sendStripData(type, RgbColor(0,0,0),56+64);
            driver->sendStripData(type, RgbColor(0,0,0),57+64);
            driver->sendStripData(type, RgbColor(0,0,0),58+64);
            driver->sendStripData(type, RgbColor(0,0,0),59+64);
            driver->sendStripData(type, RgbColor(0,0,0),60+64);
            driver->sendStripData(type, RgbColor(0,0,0),61+64);
            driver->sendStripData(type, RgbColor(0,0,0),62+64);
            driver->sendStripData(type, RgbColor(0,0,0),63+64);
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


void EyeMidEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}