#ifndef CommandControllers_H
#define CommandControllers_H
#include "./services/Service.h"
#include "./services/ServiceImplementations/LedService.h"
#include "services/ServicesCollector/ServicesCollector.h"
#include "models/response/BasicResponse.h"
#include "constants/constants.h"
#include "models/DataModelling.h"

void commandController(AsyncWebServerRequest *request);

#endif  // CommandControllers_H