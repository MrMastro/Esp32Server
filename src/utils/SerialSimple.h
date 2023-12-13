#ifndef SerialSimple_H
#define SerialSimple_H

#include <Arduino.h>
#include <WebSerial.h>


void differentSerialprintln(const String& msg, HardwareSerial* serial = nullptr, WebSerialClass* webSerial = nullptr);

#endif  // Main_H