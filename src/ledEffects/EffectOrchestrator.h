#ifndef EffectoOrchestrator_H
#define EffectoOrchestrator_H

#include "DriverLed.h"
#include "EnviromentEffect.h"
#include "ledEffects/Effect.h"
#include <vector>
#include <services/ServiceImplementations/SerialService.h>

class EffectOrchestrator{
    public:
        EffectOrchestrator();
        EffectOrchestrator(String name, DriverLed* driver, TYPE_STRIP typeled, SerialService* serialService);
        EffectOrchestrator(String name, DriverLed* driver, TYPE_STRIP typeled, boolean operative, SerialService* serialService);

        //Method for lifeCycle
        void runLifeCycle();

        //Methods for start and stop lifeCycle
        void startEffect(String effectInput,const  std::vector<RgbColor> &colorsRgb, int deltaTms);
        void stopEffect(String effectInput,const std::vector<RgbColor> &colorsRgb, int deltaTms);

        //Set and get
        void attachSerialService(SerialService* s);
        boolean isOperative();
        void setOperative(boolean enable);
        void setEffect(String effect);
        void setDeltaTms(int ms);
        void setColors(std::vector<RgbColor> colors);
        //void addEffect(Effect* e);
        void setDriver(DriverLed* driver);
    private:
        String name;
        boolean operative;
        boolean running;
        TYPE_STRIP typeLed;
        DriverLed* driver;
        SerialService* serialService;

        STEP_LIFE_LED_EFFECT actualStep;

        //Params
        String effect;
        int deltaTmsEffect;
        std::vector<RgbColor> colorsEffect;

};

#endif