#include "./Effect.h"
#include "Effect.h"

int DriverLed::getMaxNumPixel(TYPE_STRIP type)
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        return 1;
        break;
    case TYPE_STRIP::WS2811:
        return ws2811Strip->PixelCount();
    default:
        break;
    }
}

void DriverLed::sendStriptData(TYPE_STRIP type, RgbColor colorInput, uint16_t indexPixel) 
{
    switch (type)
    {
    case TYPE_STRIP::RGB:
        rgbStrip->setColor(colorInput.R, colorInput.G, colorInput.B);
        break;
    case TYPE_STRIP::WS2811:
        ws2811Strip->SetPixelColor(indexPixel, colorInput);
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
    default:
        break;
    }
}
