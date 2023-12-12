#ifndef Main_H
#define Main_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

boolean activeLed(bool active, bool toggle);
void println(String msg);

#endif  // Main_H