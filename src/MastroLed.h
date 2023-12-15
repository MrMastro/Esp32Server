#include <Arduino.h>
#include <LEDStripDriver.h>
#ifndef MastroLed_h
#define MastroLed_h

class MastroLed
{
public:
    MastroLed();
    void setupLedRgb();
    void loopLedRgb();
    void setModeValue(int value);
private:
    int value;
    int r, g, b;
    int attualspeed;
    void salita(int *speedg);
    void discesa(int *speedg);

};

#endif