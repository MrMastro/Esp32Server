#ifndef LedService_H
#define LedService_H

#pragma once
#include <./services/Service.h>
#include <Adafruit_NeoPixel.h>

class LedService: public Service
{
public:
    LedService();
    LedService(int ledPin);
    boolean isAvaible() override;
    boolean preparePin() override;
    String executeJson(String methodName, String param) override;
    String executeJson(String methodName, std::vector<String> jsonParams) override;
    boolean changeLed(boolean active, boolean toggle);
    //void changeColorBar(int r, int g, int b, int deltaT);
    void effectPrograssiveBar(uint32_t colorRgb, int deltaTms);
    void shotdownLed();
private:
    boolean isAttachedLed;
    int ledPin;
    int ws2811Pin;
    int numLeds;
    bool isLedOn;
    Adafruit_NeoPixel ledStript;
};

#endif  // LedService_H
