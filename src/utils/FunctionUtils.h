#include <vector>
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <models/LedEffectRequest/LedColorRequest.h>
#include <models/LedPresetModel/LedColor.h>

std::vector<String> splitString(const String& str, char delimiter);
uint32_t getColor(int r, int g, int b);
String rgbColorToString(RgbColor color);
String vectorRgbColorToString(std::vector<RgbColor> s);
String stringPop(std::vector<String> &s);
String vectorStringtoString(std::vector<String> s);
const std::vector<RgbColor> getRgbColorsByRequest(const std::vector<LedColorRequest> inputs);
const std::vector<RgbColor> getRgbColorsByLedColor(const std::vector<LedColor> ledColor);
void normalizeStringVectors(std::vector<String>& vector1, std::vector<String>& vector2);