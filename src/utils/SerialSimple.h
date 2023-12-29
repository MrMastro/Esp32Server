#ifndef SerialSimple_H
#define SerialSimple_H

#include <Arduino.h>
#include <WebSerial.h>


void differentSerialprintln(const String& msg, String colorMsg, HardwareSerial* serial, WebSerialClass* webSerial = nullptr);
void differentSerialprint(const String& msg, String colorMsg, HardwareSerial* serial, WebSerialClass* webSerial);
int countSubString(String msg, String subString);
String formatMsg(String msg, std::vector<String> args);
#endif  // SerialSimple_H