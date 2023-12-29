#include "CommandControllers.h"
#include "services/ServiceImplementations/CommandService.h"

void commandController(AsyncWebServerRequest *request)
{
    String command = request->arg("command");
    ((CommandService*) servicesCollector.getService("CommandService"))->recvMsgAndExecute(command);
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    BasicResponse response = BasicResponse(s);
    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
}