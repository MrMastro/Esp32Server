#include "Controllers.h"

void commandController(AsyncWebServerRequest *request)
{
    BasicResponse response;
    String command = request->arg("command");
    StatusInfo result = ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(command);
    if (result.getMessage() == getStatusInfoByHttpCode(HTTP_CODE::BadRequest).getMessage())
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription(formatMsg(UKNOWN_COMMAND, {command}));
        response = BasicResponse(s);
    }
    else
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
        response = BasicResponse(s, formatMsg(SUCCESS_COMMAND, {command, s.getDescription()}) );
    }
    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
}