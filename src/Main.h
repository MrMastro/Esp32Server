#ifndef Main_H
#define Main_H

#include <Arduino.h>

// void initWebServer(const char * ssid, const char * password, const char * deviceName, const char * devicePassword);
// void initProcessUpdateSketchOta(const char * deviceName, const char * devicePassword);
// void handleOta();
// String getName();
// void setRoutes();
// void recvMsg(uint8_t *data, size_t len);
boolean activeLed(bool active, bool toggle);
void println(String msg);

#endif  // Main_H