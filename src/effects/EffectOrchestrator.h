#ifndef EffectoOrchestrator_H
#define EffectoOrchestrator_H

#include "DriverLed.h"
#include "EnviromentEffect.h"
#include "effects/Effect.h"
#include <vector>

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

#endif