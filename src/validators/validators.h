#ifndef VALIDATORS_H
#define VALIDATORS_H
#include <Arduino.h>
#include <constants/Commands.h>
#include <constants/CommunicationMode.h>

String validateCmd(CMD cmd, std::vector<String> params);
String validateChangeCommunicationMode(std::vector<String> params);
String validateReboot(std::vector<String> params);
bool isUnsignedLong(const String& str);

#endif //VALIDATORS_H