#ifndef VALIDATORS_H
#define VALIDATORS_H
#include <Arduino.h>
#include <constants/Commands.h>
#include <constants/CommunicationMode.h>
#include "./models/settingModel/SettingsModel.h"
#include "./models/InitialSettingSaveModel/InitialSettingSaveModel.h"
#include "models/LedEffectRequest/LedColorRequest.h"

String validateCmd(CMD cmd, std::vector<String> params);
String validateChangeCommunicationMode(std::vector<String> params);
String validateReboot(std::vector<String> params);
String validateSettings(const SettingsModel &s);
String validateInitialSettings(const InitialSettingSaveModel &s);
String validateColors(std::vector<LedColorRequest> colors);
bool isUnsignedLong(const String& str);

#endif //VALIDATORS_H