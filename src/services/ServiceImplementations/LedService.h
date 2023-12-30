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
    LedService(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod>* ledStriptInput);
    boolean isAvaible() override;
    boolean preparePin();
    boolean changeLed(boolean active, boolean toggle);
    //Service Methods
    void startEffect(WS2811_EFFECT effect, RgbColor colorRgb, int deltaTmsInput);
    void stopEffect(WS2811_EFFECT effect, RgbColor colorRgb, int deltaTms);
    void runEffectWs2811LifeCycle();
private:
    boolean isAttachedLed;
    int ledPin;
    bool isLedOn;
    NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod>* ledStript;
    int ws2811Pin;
    int numLeds;
    WS2811_EFFECT ws2811Effect;
    STEP_LIFE_EFFECT ws2811Step;
    int deltaTmsEffect;
    RgbColor colorEffect;
    void execWs2811Effect(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, RgbColor colorInput, int deltaTimeMsInput);
    // Effect
    void ws2811EffectConstantsUniqueColor(STEP_LIFE_EFFECT ws2811StepInput, RgbColor colorInput, int deltaTimeMsInput);
    void ws2811EffectProgressiveBarUniqueColor(STEP_LIFE_EFFECT ws2811StepInput, RgbColor colorInput, int deltaTimeMsInput);
};

#endif  // LedService_H
