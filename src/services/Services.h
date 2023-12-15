#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#ifndef Services_H
#define Services_H

String pong();
String handleTextRequest(AsyncWebServerRequest *request);

#endif  // Services_H