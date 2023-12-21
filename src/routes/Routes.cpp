#include "Routes.h"
#include "./services/Service.h"
#include "./services/ServiceImplementations/LedService.h"
#include "models/response/BasicResponse.h"
#include "constants/constants.h"
#include "models/DataModelling.h"

void initRoutes(MastroServer &srv, ServicesCollector &serviceCollector)
{
   srv.setCustomApi("/try", HTTP_GET, [](AsyncWebServerRequest *request)
                    {
                        String message = "toDo"; //serviceCollector->executeMethod("InfoService","jsonPong");
                        request->send(200, "application/json", message); });

   srv.setCustomApi("/api/handleLed", HTTP_POST, [](AsyncWebServerRequest *request)
                    {
                        //todo
                        //LedService ledService = LedService();
                        //boolean statusLed = ledService.changeLed(true, true);
                        boolean statusLed = false;
                        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
                        BasicResponse response = BasicResponse(s,"{'ledOn':false}");
                        String jsonResponse = dtoToJson(response);
                        request->send(200, "application/json", jsonResponse); });

   srv.setCustomApi("/api/ping", HTTP_GET, [](AsyncWebServerRequest *request)
                    {
                        String jsonResponse = "toDo"; //serviceCollector.executeMethod("InfoService","jsonPong");;
                        request->send(200, "application/json", jsonResponse); });
   srv.setCustomApi("/api/sendCommand", HTTP_POST, [](AsyncWebServerRequest *request)
                    {
                        //todo
                        String result = "";
                        request->send(200, "application/json", result); });
}