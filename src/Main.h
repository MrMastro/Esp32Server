#ifndef Main_H
#define Main_H
#include <Arduino.h>

#include "MastroServer.h"
#include "MastroLed.h"

#include "./services/ServicesCollector/ServicesCollector.h"
#include "services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/LedService.h"

#include "routes/Routes.h"


//#include "services/ServiceImplementations/InfoService.cpp"

ServicesCollector servicesCollector;
MastroServer myServer;
MastroLed myRgbStript;
CommandService commandService;
LedService ledService;
//InfoService infoService;
void recvMsgBySerial(String data);
void recvMsgBySerialWeb(uint8_t *data, size_t len);
void logInfo(String msg);
#endif  // Main_H