#ifndef Controllers_H
#define Controllers_H

#include <ESPAsyncWebServer.h>
#include "services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/InfoService.h"
#include <models/response/BasicResponse/BasicResponse.h>
#include <models/InitialSettingSaveModel/InitialSettingSaveModel.h>
#include "constants/Constants.h"
#include "models/DataModelling.h"

//CommandController
void commandController(AsyncWebServerRequest *request);

//InfoController
void getOk(AsyncWebServerRequest *request);
void login(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void getJsonSettings(AsyncWebServerRequest *request);

//LedController
void setEffectWs2811(AsyncWebServerRequest *request);
void stopEffectWs2811(AsyncWebServerRequest *request);

//SettingsController
void saveSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void saveInitialSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

//------------------------------------------------------------------------------------------------------------

//utils
String getBodyString(uint8_t *data, size_t len);

#endif  // Controllers_H