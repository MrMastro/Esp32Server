#ifndef Effect_H
#define Effect_H
#include <Arduino.h>
#include <constants/LedEffects.h>
#include <NeoPixelBus.h>
#include <LEDStripDriver.h>



class DriverLed;
class Effect;
class EffectOrchestrator;
enum class TYPE_STRIP;

enum class TYPE_STRIP
{
  RGB,
  WS2811
};
// Service interface
class Effect {
public:
    virtual String getName() = 0;
    virtual int getColorInputQt() = 0;
    virtual void execStep(EFFECT_LABEL ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput,const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP typeOrchestrator) = 0;
    virtual void off(DriverLed* driver, TYPE_STRIP typeOrchestrator) =0;
};

class EffectOrchestrator{
    public:
        EffectOrchestrator();
        EffectOrchestrator(DriverLed* driver, TYPE_STRIP typeled);

        //Method for lifeCycle
        void runLifeCycle();

        //Methods for start and stop lifeCycle
        void startEffect(EFFECT_LABEL effectInput,const  std::vector<RgbColor> &colorsRgb, int deltaTms);
        void stopEffect(EFFECT_LABEL effectInput,const std::vector<RgbColor> &colorsRgb, int deltaTms);

        //Set and get
        void setEffect(EFFECT_LABEL effect);
        void setDeltaTms(int ms);
        void setColors(std::vector<RgbColor> colors);
        void addEffect(Effect* e);
        void setDriver(DriverLed* driver);
    private:
        TYPE_STRIP typeLed;
        DriverLed* driver;

        STEP_LIFE_EFFECT actualStep;
        std::vector<Effect*> allEffects;

        //Params
        EFFECT_LABEL effect;
        int deltaTmsEffect;
        std::vector<RgbColor> colorsEffect;

        //utils
        Effect* getEffectByName(String name);


};

class DriverLed{
    public:
        DriverLed() : ws2811Strip(nullptr), rgbStrip(nullptr) {}
        DriverLed(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip, LEDStripDriver *rgbStrip)
            : ws2811Strip(ws2811Strip), rgbStrip(rgbStrip) {}
        int getMaxNumPixel(TYPE_STRIP type);
        void sendStriptData(TYPE_STRIP type, RgbColor colorInput, uint16_t indexPixel = 0);
        void showData();
        void clear(TYPE_STRIP type);
    private:
        NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip;
        LEDStripDriver *rgbStrip;
};



#endif  // Effect_H