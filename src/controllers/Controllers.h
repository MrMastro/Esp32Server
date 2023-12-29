#ifndef Controllers_H
#define Controllers_H

#include <ESPAsyncWebServer.h>
#include "services/ServicesCollector/ServicesCollector.h"
#include "services/ServiceImplementations/CommandService.h"
#include "models/response/BasicResponse.h"
#include "constants/constants.h"
#include "models/DataModelling.h"

void commandController(AsyncWebServerRequest *request);

#endif  // CommandControllers_H