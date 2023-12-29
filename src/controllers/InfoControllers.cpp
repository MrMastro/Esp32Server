#include "Controllers.h"

void getOk(AsyncWebServerRequest *request)
{
    BasicResponse response;
    StatusInfo result = ((InfoService *) servicesCollector.getService("InfoService"))->infoSuccess();
    response.setStatus(result);
    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
}