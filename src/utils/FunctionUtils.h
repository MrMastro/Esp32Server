#include <vector>
#include <Arduino.h>
#include <NeoPixelBus.h>

std::vector<String> splitString(const String& str, char delimiter);
uint32_t getColor(int r, int g, int b);
String rgbColorToString(RgbColor color);