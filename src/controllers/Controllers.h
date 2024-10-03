#ifndef Controllers_H
#define Controllers_H

#include <ESPAsyncWebServer.h>
#include "services/ServicesCollector/ServicesCollector.h"
#include "services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/InfoService.cpp"
#include "models/response/BasicResponse.h"
#include "constants/Constants.h"
#include "models/DataModelling.h"

//CommandController
void commandController(AsyncWebServerRequest *request);

//InfoController
void getOk(AsyncWebServerRequest *request);

//LedController
void setEffectWs2811(AsyncWebServerRequest *request);
void stopEffectWs2811(AsyncWebServerRequest *request);

//SettingsController
void saveSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);


#endif  // CommandControllers_H