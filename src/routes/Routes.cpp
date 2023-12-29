#include "Routes.h"

void initRoutes(MastroServer &srv)
{
   //  srv.setCustomApi("/try", HTTP_GET, [](AsyncWebServerRequest *request)
   //                   {
   //                      String message = "toDo"; 
   //                      request->send(200, "application/json", message); });

   //  srv.setCustomApi("/api/handleLed", HTTP_POST, [](AsyncWebServerRequest *request)
   //                   {
   //                      //todo
   //                      //boolean statusLed = ledService.changeLed(true, true);
   //                      boolean statusLed = false;
   //                      StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
   //                      BasicResponse response = BasicResponse(s,"{'ledOn':false}");
   //                      String jsonResponse = dtoToJson(response);
   //                      request->send(200, "application/json", jsonResponse); });

   //  srv.setCustomApi("/api/ping", HTTP_GET, [](AsyncWebServerRequest *request)
   //                   {
   //                      String jsonResponse = "toDo"; //serviceCollector.executeMethod("InfoService","jsonPong");
   //                      request->send(200, "application/json", jsonResponse); });
   //  srv.setCustomApi("/api/sendCommand", HTTP_POST, [](AsyncWebServerRequest *request)
   //                   {
   //                      //todo
   //                      String result = "";
   //                      request->send(200, "application/json", result); });
     srv.setCustomApi("/api/sendCommand", HTTP_POST, commandController);
     
}

