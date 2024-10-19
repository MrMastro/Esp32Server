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
        void startEffect(String effectInput,const  std::vector<RgbColor> &colorsRgb, int deltaTms);
        void stopEffect(String effectInput,const std::vector<RgbColor> &colorsRgb, int deltaTms);

        //Set and get
        boolean isOperative();
        void setOperative(boolean enable);
        void setEffect(String effect);
        void setDeltaTms(int ms);
        void setColors(std::vector<RgbColor> colors);
        //void addEffect(Effect* e);
        void setDriver(DriverLed* driver);
    private:
        boolean operative;
        boolean running;
        TYPE_STRIP typeLed;
        DriverLed* driver;

        STEP_LIFE_EFFECT actualStep;

        //Params
        String effect;
        int deltaTmsEffect;
        std::vector<RgbColor> colorsEffect;

};

#endif