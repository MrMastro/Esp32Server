#include <Arduino.h>
#ifndef LedService_H
#define LedService_H

class LedService
{
public:
    LedService();
    LedService(int ledPin);
    boolean attach(int ledPin);
    boolean changeLed(bool active, bool toggle);
private:
    boolean isAttachedLed;
    int ledPin;
    bool isLedOn;
};

#endif  // LedService_H

