#ifndef Controllers_H
#define Controllers_H

#include <ESPAsyncWebServer.h>
#include "services/ServicesCollector/ServicesCollector.h"
#include "services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/InfoService.cpp"
#include "models/response/BasicResponse.h"
#include "constants/constants.h"
#include "models/DataModelling.h"

void commandController(AsyncWebServerRequest *request);
void getOk(AsyncWebServerRequest *request);

#endif  // CommandControllers_H