#include "Controllers.h"
#include <models/loginModel/LoginModel.h>
#include <models/response/SettingsResponse/SettingsResponse.h>

void getOk(AsyncWebServerRequest *request)
{
    BasicResponse response;
    StatusInfo result = ((InfoService *)servicesCollector.getService("InfoService"))->infoSuccess();
    response.setStatus(result);
    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
}

void login(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    servicesCollector.takeExclusiveExecution();
    String body = getBodyString(data, len);
    StatusInfo res;
    LoginModel l;

    if (!l.fromJson(body))
    {
        res = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        res.setDescription("Errore Serializzazione");
        // response = BasicResponse(s);
        // String jsonResponse = dtoToJson(response);
        request->send(res.getCode(), "application/json", res.toJson());
        servicesCollector.freeExclusiveExecution();
        return;
    };
    boolean logged = ((InfoService *)servicesCollector.getService("InfoService"))->loginValidate(l.deviceName, l.devicePassword);

    res = logged ? getStatusInfoByHttpCode(HTTP_CODE::OK) : getStatusInfoByHttpCode(HTTP_CODE::Unauthorized);

    request->send(res.getCode(), "application/json", res.toJson());
    servicesCollector.freeExclusiveExecution();
}

void getJsonSettings(AsyncWebServerRequest *request)
{
    servicesCollector.takeExclusiveExecution();
    InfoService *infoService = ((InfoService *)servicesCollector.getService("InfoService"));
    SettingService *settingService = ((SettingService *)servicesCollector.getService("SettingService"));
    SettingsResponse res;
    StatusInfo info;
    if (!infoService->checkAuthorization(""))
    {
        info = getStatusInfoByHttpCode(HTTP_CODE::Unauthorized);
        res = SettingsResponse(info);
        request->send(res.getStatus().getCode(), "application/json", res.toJson());
        servicesCollector.freeExclusiveExecution();
        return;
    }

    SettingsModel settings = settingService->getSettings();
    info = getStatusInfoByHttpCode(HTTP_CODE::OK);
    res = SettingsResponse(info);
    res.setDataJson(settings);
    request->send(res.getStatus().getCode(), "application/json", res.toJson());
    servicesCollector.freeExclusiveExecution();
}