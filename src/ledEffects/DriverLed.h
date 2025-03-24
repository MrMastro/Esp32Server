#ifndef DriverLed_H
#define DriverLed_H

#include <Arduino.h>
#include <NeoPixelBus.h>
#include <LEDStripDriver.h>
#include "EnviromentEffect.h"

enum class TYPE_STRIP;

class DriverLed{
    public:
        DriverLed();
        DriverLed(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Matrix, NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip, LEDStripDriver *rgbStrip);
        int getMaxNumPixel(TYPE_STRIP type);
        RgbColor getColorPixel(uint16_t indexPixel, TYPE_STRIP type);
        void sendStripData(TYPE_STRIP type, RgbColor colorInput, uint16_t indexPixel = 0);
        void showData();
        void clear(TYPE_STRIP type);
    private:
        NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip;
        NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Matrix;
        LEDStripDriver *rgbStrip;
        RgbColor colorRgbMemorized;
};

#endif