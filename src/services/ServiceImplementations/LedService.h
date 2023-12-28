#ifndef LedService_H
#define LedService_H

#include <./services/Service.h>
#include <Adafruit_NeoPixel.h>

class LedService: public Service
{
public:
    LedService();
    boolean isAvaible() override;
    boolean preparePin();
    // void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;
    String executeJson(String methodName, String param) override;
    String executeJson(String methodName, std::vector<String> jsonParams) override;
    boolean changeLed(boolean active, boolean toggle);
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
