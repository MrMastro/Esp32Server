#include "./DriverLed.h"
#include "DriverLed.h"

DriverLed::DriverLed()
{
    ws2811Matrix = nullptr;
    ws2811Strip = nullptr;
    rgbStrip = nullptr;
    colorRgbMemorized = RgbColor(0,0,0);
}

DriverLed::DriverLed(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811MatrixInput, NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811StripInput, LEDStripDriver *rgbStripInput)
{
    ws2811Strip = ws2811StripInput;
    rgbStrip = rgbStripInput;
    ws2811Matrix = ws2811MatrixInput;
    ws2811Matrix->Begin();
    ws2811Strip->Begin();
    ws2811Strip->Show();
}

int DriverLed::getMaxNumPixel(TYPE_STRIP type)
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        return 1;
        break;
    case TYPE_STRIP::WS2811:
        return ws2811Strip->PixelCount();
    case TYPE_STRIP::WS2811_MATRIX:
        return ws2811Matrix->PixelCount();
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
        return colorRgbMemorized;
        break;
    case TYPE_STRIP::WS2811:
        return ws2811Strip->GetPixelColor(indexPixel);
    case TYPE_STRIP::WS2811_MATRIX:
        return ws2811Matrix->GetPixelColor(indexPixel);
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
        colorRgbMemorized = colorInput;
        rgbStrip->setColor(colorInput.R, colorInput.G, colorInput.B);
        break;
    case TYPE_STRIP::WS2811:
        ws2811Strip->SetPixelColor(indexPixel, colorInput);
    case TYPE_STRIP::WS2811_MATRIX:
        ws2811Matrix->SetPixelColor(indexPixel, colorInput);
    default:
        break;
    }
}

void DriverLed::showData(){
    ws2811Strip->Show();
}

void DriverLed::clear(TYPE_STRIP type)
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        rgbStrip->setColor(0, 0, 0);
        break;
    case TYPE_STRIP::WS2811:
        ws2811Strip->ClearTo(RgbColor(0));
        ws2811Strip->Show();
        break;
    case TYPE_STRIP::WS2811_MATRIX:
        ws2811Strip->ClearTo(RgbColor(0));
        ws2811Strip->Show();
    default:
        break;
    }
}
