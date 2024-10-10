#ifndef Controllers_H
#define Controllers_H

#include <ESPAsyncWebServer.h>
#include "services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/InfoService.cpp"
#include "models/response/BasicResponse.h"
#include "constants/Constants.h"
#include "models/DataModelling.h"

//CommandController
void commandController(AsyncWebServerRequest *request);

//InfoController
void getOk(AsyncWebServerRequest *request);
void login(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

//LedController
void setEffectWs2811(AsyncWebServerRequest *request);
void stopEffectWs2811(AsyncWebServerRequest *request);

//SettingsController
void saveSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);

//------------------------------------------------------------------------------------------------------------

//utils

String getBodyString(uint8_t *data, size_t len){
        // Crea una stringa per accumulare il body
    String body = "";

    // Accumula i dati del body
    for (size_t i = 0; i < len; i++) {
        body += (char)data[i];
    }

    return body;
}

#endif  // Controllers_H