#ifndef Main_H
#define Main_H

#include <Arduino.h>
#include <WebSerial.h>
void recvMsgBySerial(String data);
void recvMsgBySerialWeb(uint8_t *data, size_t len);
#endif  // Main_H