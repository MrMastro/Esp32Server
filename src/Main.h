#ifndef Main_H
#define Main_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>

boolean activeLed(bool active, bool toggle);
void println(String msg);
void handleTextRequest(AsyncWebServerRequest *request);

#endif  // Main_H