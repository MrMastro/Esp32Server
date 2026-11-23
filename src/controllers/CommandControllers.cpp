#include "Controllers.h"
#include <ArduinoJson.h>
#include <models/response/BasicResponseString/BasicResponseString.h>

static const size_t COMMAND_MAX_BODY = 512;

// Body di POST /api/v1/sendCommand: arriva a pezzi (index/total), si ricompone in request->_tempObject
// (liberato con free() dalla libreria alla fine della richiesta). Body oltre COMMAND_MAX_BODY: ignorato.
void commandControllerBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (total == 0 || total > COMMAND_MAX_BODY)
        return;
    if (index == 0)
    {
        if (request->_tempObject != NULL)
            free(request->_tempObject);
        request->_tempObject = calloc(total + 1, 1); // +1: terminatore della stringa
    }
    if (request->_tempObject == NULL || index + len > total)
        return;
    memcpy((uint8_t *)request->_tempObject + index, data, len);
}

// Comando dal body JSON {"command": "..."} se presente, altrimenti dal parametro "command" (?command=... o form)
static String commandFromRequest(AsyncWebServerRequest *request)
{
    if (request->_tempObject != NULL)
    {
        JsonDocument doc;
        if (!deserializeJson(doc, (const char *)request->_tempObject) && doc["command"].is<const char *>())
            return doc["command"].as<String>();
    }
    return request->arg("command");
}

void commandController(AsyncWebServerRequest *request)
{
    servicesCollector.takeExclusiveExecution();
    servicesCollector.sleepyJoystick();
    BasicResponseString response;
    String command = commandFromRequest(request);
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