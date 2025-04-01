#include "./DriverLed.h"
#include "DriverLed.h"

DriverLed::DriverLed()
{
    enableStripRgb = false;
    enableStripWs2811 = false;
    enableStripWs2811Matrix = false;
    ws2811Matrix = nullptr;
    ws2811Strip = nullptr;
    rgbStrip = nullptr;
    colorRgbMemorized = RgbColor(0,0,0);
}
//no rgb, no brg,
DriverLed::DriverLed(NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *ws2811MatrixInput, NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811StripInput, LEDStripDriver *rgbStripInput)
{
    ws2811Strip = ws2811StripInput;
    rgbStrip = rgbStripInput;
    ws2811Matrix = ws2811MatrixInput;

    if(ws2811Strip != nullptr){
        ws2811Strip->Begin();
        delay(100);
        ws2811Strip->Show();
        delay(100);
    }

    if(ws2811Matrix != nullptr){
        ws2811Matrix->Begin();
        delay(100);
        ws2811Matrix->Show();
        delay(100);
    }
}

int DriverLed::getMaxNumPixel(TYPE_STRIP type)
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        if(rgbStrip != nullptr){
            return 1;
        }
        break;
    case TYPE_STRIP::WS2811:
        if(ws2811Strip != nullptr){
            return ws2811Strip->PixelCount();
        }
        break;
    case TYPE_STRIP::WS2811_MATRIX:
        if(ws2811Matrix != nullptr){
            return ws2811Matrix->PixelCount();
        }
        break;
    default:
        break;
    }
    return 0;
}

RgbColor DriverLed::getColorPixel(uint16_t indexPixel, TYPE_STRIP type)
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        if(rgbStrip != nullptr){
            return colorRgbMemorized;
        }
        break;
    case TYPE_STRIP::WS2811:
        if(ws2811Strip != nullptr){
            return ws2811Strip->GetPixelColor(indexPixel);
        }
        break;
    case TYPE_STRIP::WS2811_MATRIX:
        if(ws2811Matrix != nullptr){
            return ws2811Matrix->GetPixelColor(indexPixel);
        }
        break;
    default:
        break;
    }
    return RgbColor();
}
void DriverLed::sendStripData(TYPE_STRIP type, RgbColor colorInput, uint16_t indexPixel) 
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        if(rgbStrip != nullptr){
            colorRgbMemorized = colorInput;
            rgbStrip->setColor(colorInput.R, colorInput.G, colorInput.B);
        }
        break;
    case TYPE_STRIP::WS2811:
        if(ws2811Strip != nullptr){
            ws2811Strip->SetPixelColor(indexPixel, colorInput);
        }
        break;
    case TYPE_STRIP::WS2811_MATRIX:
        if(ws2811Matrix != nullptr){
            ws2811Matrix->SetPixelColor(indexPixel, colorInput);
        }
        break;
    default:
        break;
    }
}

void DriverLed::showData(TYPE_STRIP type){
    switch (type)
    {
    case TYPE_STRIP::RGB:
        break;
    case TYPE_STRIP::WS2811:
        if(ws2811Strip != nullptr){
            ws2811Strip->Show();
        }
        break;
    case TYPE_STRIP::WS2811_MATRIX:
        if(ws2811Matrix != nullptr){
            ws2811Matrix->Show();
        }
    default:
        break;
    }
    //delay(100);
}

void DriverLed::clear(TYPE_STRIP type)
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        if(rgbStrip != nullptr){
            rgbStrip->setColor(0, 0, 0);
        }
        break;
    case TYPE_STRIP::WS2811:
        if(ws2811Strip != nullptr){
            ws2811Strip->ClearTo(RgbColor(0));
            ws2811Strip->Show();
        }
        break;
    case TYPE_STRIP::WS2811_MATRIX:
        if(ws2811Matrix != nullptr){
            ws2811Matrix->ClearTo(RgbColor(0));
            ws2811Matrix->Show();
        }
        break;
    default:
        break;
    }
}
