#ifndef Effect_H
#define Effect_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <LEDStripDriver.h>
#include <vector>
// #include "./effects/EffectImplementations/AlternatingEffect.h"
// #include "./effects/EffectImplementations/ConstantUniqueColorEffect.h"
// #include "./effects/EffectImplementations/FadeEffect.h"
// #include "./effects/EffectImplementations/ProgressiveBarUniqueColorEffect.h"
// #include "./effects/EffectImplementations/WaweUniqueColorEffect.h"

class Effect;
class ConstantUniqueColorEffect;
class WaweUniqueColorEffect;
class ProgressiveBarUniqueColorEffect;
class FadeEffect;
class EffectOrchestrator;
class DriverLed;
enum class TYPE_STRIP;

enum class STEP_LIFE_EFFECT
{
  BEGIN_STEP,
  LOOP_STEP,
  END_STEP,
  OFF
};

const std::pair<STEP_LIFE_EFFECT, String> STEP_LIFE_EFFECT_PAIR[] = {
    {STEP_LIFE_EFFECT::BEGIN_STEP, "BEGIN_STEP"},
    {STEP_LIFE_EFFECT::LOOP_STEP, "LOOP_STEP"},
    {STEP_LIFE_EFFECT::END_STEP, "END_STEP"},
    {STEP_LIFE_EFFECT::OFF, "OFF"}
};

//EFFECT_LABEL

enum class EFFECT_LABEL
{
    NO_EFFECT, //No effect
    UKNOWN_EFFECT, //I don't know the effect
    ACTUAL_EFFECT, //The effect that run in this moment
    CONSTANT_UNIQUE_COLOR, // Effect of constant color in all pixel of stript
    WAWE_UNIQUE_COLOR, //Effect of wawe color
    PROGRESSIVE_BAR_UNIQUE_COLOR, //Effect of progressive color
    FADE_EFFECT, //Effect of fade color
    ALTERNATING_EFFECT //Effect that alternate two or more color at same
};

const std::pair<EFFECT_LABEL, String> EFFECT_LABEL_REFERENCE_STRING[] = {
    {EFFECT_LABEL::NO_EFFECT, "NO_EFFECT"},
    {EFFECT_LABEL::UKNOWN_EFFECT, "UKNOWN_EFFECT"},
    {EFFECT_LABEL::ACTUAL_EFFECT, "ACTUAL_EFFECT"},
    {EFFECT_LABEL::CONSTANT_UNIQUE_COLOR, "CONSTANT_UNIQUE_COLOR"},
    {EFFECT_LABEL::WAWE_UNIQUE_COLOR, "WAWE_UNIQUE_COLOR"},
    {EFFECT_LABEL::PROGRESSIVE_BAR_UNIQUE_COLOR, "PROGRESSIVE_BAR_UNIQUE_COLOR"},
    {EFFECT_LABEL::FADE_EFFECT, "FADE_EFFECT"},
    {EFFECT_LABEL::ALTERNATING_EFFECT, "ALTERNATING_EFFECT"}
    // Add more effect codes and information as needed
};

// const std::pair<EFFECT_LABEL, Effect*> EFFECT_REFERENCE[] = {
//     {EFFECT_LABEL::CONSTANT_UNIQUE_COLOR, new ConstantUniqueColorEffect()},
//     {EFFECT_LABEL::WAWE_UNIQUE_COLOR, new WaweUniqueColorEffect()},
//     {EFFECT_LABEL::PROGRESSIVE_BAR_UNIQUE_COLOR, new ProgressiveBarUniqueColorEffect()},
//     {EFFECT_LABEL::FADE_EFFECT, new FadeEffect()}
// };

const EFFECT_LABEL LabelEffectStringToEnum(String inputString);
const String LabelEffectEnumToString(EFFECT_LABEL inputEnum);
// Funzione per ottenere un vettore con i nomi degli effetti (le stringhe)
std::vector<String> getAllWS2811EffectNames();
const String stepLifeEffectEnomToString(STEP_LIFE_EFFECT inputEnum);

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
        EffectOrchestrator(DriverLed* driver, TYPE_STRIP typeled, boolean operative);

        //Method for lifeCycle
        void runLifeCycle();

        //Methods for start and stop lifeCycle
        void startEffect(EFFECT_LABEL effectInput,const  std::vector<RgbColor> &colorsRgb, int deltaTms);
        void stopEffect(EFFECT_LABEL effectInput,const std::vector<RgbColor> &colorsRgb, int deltaTms);

        //Set and get
        boolean isOperative();
        void setOperative(boolean enable);
        void setEffect(EFFECT_LABEL effect);
        void setDeltaTms(int ms);
        void setColors(std::vector<RgbColor> colors);
        void addEffect(Effect* e);
        void setDriver(DriverLed* driver);
    private:
        boolean operative;
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