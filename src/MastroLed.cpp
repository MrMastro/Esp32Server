#include "MastroLed.h"

//                                  Din,Cin
LEDStripDriver led = LEDStripDriver(19, 18);

MastroLed::MastroLed()
{
    setupLedRgb();
}

void MastroLed::setupLedRgb()
{
    value = 0;
    r = 0;
    g = 0;
    b = 255;
    attualspeed = 10;
    led.setColor(0, g, b); // RGB
}

void MastroLed::setModeValue(int value)
{
    if (value == 1)
    {
        attualspeed = 2300;
        Serial.println("caso veloce");
        Serial.print(value);
    }
    if (value == 2)
    {
        attualspeed = 4000;
        Serial.println(value);
    }
    if (value == 3)
    {
        attualspeed = 8000;
        Serial.println(value);
    }
    if (value == 4)
    {
        attualspeed = 16000; // nerfato con aggiunta delay microsec
        Serial.println("caso lento-");
        Serial.print(value);
    }
}

void MastroLed::salita(int *speedg)
{
    for (; g < 80; g = g + 1)
    {

        led.setColor(r, g, b); // RGB
        if (value > 3)
        {
            break;
        }
        delayMicroseconds(attualspeed);
    }
}

void MastroLed::discesa(int *speedg)
{
    for (; g > 10; g = g - 1)
    {
        led.setColor(r, g, b); // RGB
        if (value > 3)
        {
            break;
        }
        led.setColor(r, g, b); // RGB
                               // if(g==50)
        //  delay(10);
        if (value == 3)
        {
            delayMicroseconds(attualspeed);
            delayMicroseconds(9000);
        }
        else
            delayMicroseconds(attualspeed);
    }
}
void MastroLed::loopLedRgb()
{
    if (value < 4)
    {
        led.setColor(r, g, b); // RGB
        salita(&attualspeed);
        discesa(&attualspeed);
    }
    if (value == 4)
    {
        led.setColor(r, 50, 255); // RGB
    }
    if (value == 5)
    {
        led.setColor(0, 10, 255); // RGB
    }
}
