#ifndef LedService_H
#define LedService_H

#include <./services/Service.h>
#include <NeoPixelBus.h>
#include "constants/LedEffects.h"

//numPixel = 32
//pinStrip = 5

class LedService: public Service
{
public:
    LedService(uint16_t countPixels, uint8_t pin);
    LedService(NeoPixelBus<NeoBgrFeature, Neo800KbpsMethod>* ledStriptInput);
    boolean isAvaible() override;
    boolean preparePin();
    // void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;
    //String executeJson(String methodName, String param) override;
    //String executeJson(String methodName, std::vector<String> jsonParams) override;
    boolean changeLed(boolean active, boolean toggle);
    void startEffect(WS2811_EFFECT effect, RgbColor colorRgb, int deltaTmsInput);
    void stopEffect(RgbColor colorRgb, int deltaTms);
    void runEffectWs2811();
private:
    boolean isAttachedLed;
    int ledPin;
    bool isLedOn;
    //NeoPixelBus<NeoBgrFeature, Neo800KbpsMethod> ledStript;
    NeoPixelBus<NeoBgrFeature, Neo800KbpsMethod>* ledStript;
    int ws2811Pin;
    int numLeds;
    WS2811_EFFECT ws2811Effect;
    STEP_LIFE_EFFECT ws2811Step;
    int deltaTmsEffect;
    RgbColor colorEffect;
    void doBeginStep(RgbColor colorRgb, int deltaTms);
    void doLoopStep(RgbColor colorRgb, int deltaTms);
    void doEndStep(RgbColor colorRgb, int deltaTms);
    void colorAll(RgbColor color);
};

#endif  // LedService_H
