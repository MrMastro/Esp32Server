#ifndef LedService_H
#define LedService_H

#include <./services/Service.h>
#include <Adafruit_NeoPixel.h>
#include "constants/LedEffects.h"

class LedService: public Service
{
public:
    LedService();
    boolean isAvaible() override;
    boolean preparePin();
    // void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;
    //String executeJson(String methodName, String param) override;
    //String executeJson(String methodName, std::vector<String> jsonParams) override;
    boolean changeLed(boolean active, boolean toggle);
    void startEffect(WS2811_EFFECT effect, uint32_t colorRgb, int deltaTmsInput);
    void stopEffect(uint32_t colorRgb, int deltaTms);
    void runEffectWs2811();
private:
    boolean isAttachedLed;
    int ledPin;
    bool isLedOn;

    Adafruit_NeoPixel ledStript;
    int ws2811Pin;
    int numLeds;
    WS2811_EFFECT ws2811Effect;
    STEP_LIFE_EFFECT ws2811Step;
    int deltaTmsEffect;
    uint32_t colorEffect;
    void doBeginStep(uint32_t colorRgb, int deltaTms);
    void doLoopStep(uint32_t colorRgb, int deltaTms);
    void doEndStep(uint32_t colorRgb, int deltaTms);
};

#endif  // LedService_H
