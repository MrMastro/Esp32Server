#include "Controllers.h"
#include <models/response/BasicResponseString/BasicResponseString.h>

void commandController(AsyncWebServerRequest *request)
{
    servicesCollector.takeExclusiveExecution();
    BasicResponseString response;
    String command = request->arg("command");
    StatusInfo result = ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(command);
    if (result.getMessage() == getStatusInfoByHttpCode(HTTP_CODE::BadRequest).getMessage())
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription(formatMsg(ERROR_COMMAND, {command, result.getDescription()}));
        response = BasicResponseString(s);
    }
    else
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
        response = BasicResponseString(s);
        response.setDataJson(formatMsg(SUCCESS_COMMAND, {command, s.getDescription()}));
    }
    
    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}