#include "Services.h"
#include <model/response/BasicResponse.h>
#include <constants/constants.h>
#include <model/DataModelling.h>

// ################################################################################ //
//                               Custom Service Method                              //
// ################################################################################ //

// Function to handle /text route
String handleTextRequest(AsyncWebServerRequest *request)
{
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    BasicResponse response = BasicResponse(s);
    return dtoToJson(response);
}

//to eliminate
// void handleMainLed(AsyncWebServerRequest *request)
// {
//   DynamicJsonDocument doc(1024);
//   bool result = activeLed(true,true);
//   String json = myServer.getOneElementJsonString("lightLed", result?"true":"false");
//   request->send(200, "application/json", json);
// }

// void handleSuccessResponse(AsyncWebServerRequest *request)
// {
//   String response = sendOk();
//   request->send(200, "application/json", response);
// }