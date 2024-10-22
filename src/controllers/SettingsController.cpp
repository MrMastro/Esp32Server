#include "Controllers.h"

void saveSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    servicesCollector.takeExclusiveExecution();
    BasicResponse response;

    // Crea una stringa per accumulare il body
    String body = "";

    // Accumula i dati del body
    for (size_t i = 0; i < len; i++) {
        body += (char)data[i];
    }

    SettingsModel settingModel;

    if(!settingModel.fromJson(body)){
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription("Errore Serializzazione");
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }

    String error = validateSettings(settingModel);
    if(!error.isEmpty())
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription("Validation fail, bad request: " + error);
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }
    boolean result = ((SettingService *)servicesCollector.getService("SettingService"))->saveSettings(SETTINGS_FILE_LOCATION_PATH,settingModel);
    ((CommandService *)servicesCollector.getService("CommandService"))->insertDelayedCommand(CMD::REBOOT, {}, 1000);
    if (!result)
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
        s.setDescription("Errore di salvataggio");
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    s.setDescription("Impostazioni salvate con successo, il dispositivo verrà riavviato");
    response = BasicResponse(s);
    String jsonResponse = dtoToJson(response);
    request->send(s.getCode(), "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}

void saveInitialSettings(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    servicesCollector.takeExclusiveExecution();
    BasicResponse response;

    // Get body by HttpUtils, getBodyString
    String body = getBodyString(data, len);

    InitialSettingSaveModel is;

    //Serial.println("Received:\n" + body);

    if(!is.fromJson(body)){
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription("Errore Serializzazione");
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }

    String error = validateInitialSettings(is);
    if(!error.isEmpty())
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription("Validation fail, bad request: " + error);
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }
    boolean result = ((SettingService *)servicesCollector.getService("SettingService"))->saveInitialSettings(SETTINGS_FILE_LOCATION_PATH,is);
    if (!result)
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
        s.setDescription("Errore di salvataggio");
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    s.setDescription("Impostazioni sull'effetto iniziale salvate, saranno disponibili al prossimo riavvio");
    response = BasicResponse(s);
    String jsonResponse = dtoToJson(response);
    request->send(s.getCode(), "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}