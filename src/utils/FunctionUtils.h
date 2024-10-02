#include <vector>
#include <Arduino.h>
#include <NeoPixelBus.h>

std::vector<String> splitString(const String& str, char delimiter);
uint32_t getColor(int r, int g, int b);
String rgbColorToString(RgbColor color);
String stringPop(std::vector<String> &s);
String vectorStringtoString(std::vector<String> s);