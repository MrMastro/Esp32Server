#ifndef DriverLed_H
#define DriverLed_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <LEDStripDriver.h>
#include "EnviromentEffect.h"

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

#endif