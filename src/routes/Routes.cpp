#include "Routes.h"
#include "services/Services.h"
#include "services/LedService.h"
#include "model/response/BasicResponse.h"
#include "constants/constants.h"
#include "model/DataModelling.h"

void initRoutes(MastroServer &srv)
{
   srv.setCustomApi("/try", HTTP_GET, [](AsyncWebServerRequest *request)
                    {
                        String message = handleTextRequest(request);
                        request->send(200, "application/json", message); });

   srv.setCustomApi("/api/handleLed", HTTP_POST, [](AsyncWebServerRequest *request)
                    {
                        LedService ledService = LedService();
                        boolean statusLed = ledService.changeLed(true, true);
                        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
                        BasicResponse response = BasicResponse(s,"{'ledOn':false}");
                        String jsonResponse = dtoToJson(response);
                        request->send(200, "application/json", jsonResponse); });

   srv.setCustomApi("/api/ping", HTTP_GET, [](AsyncWebServerRequest *request)
                    {
                        String jsonResponse = pong();
                        request->send(200, "application/json", jsonResponse); });
   srv.setCustomApi("/api/sendCommand", HTTP_POST, [](AsyncWebServerRequest *request)
                    {
                     String result = "";
                     request->send(200, "application/json", result); });
}