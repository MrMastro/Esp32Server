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
        request->send(400, "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    };

    String error = validateSettings(settingModel);
    if(!error.isEmpty())
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription("Validation fail, bad request: " + error);
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(400, "application/json", jsonResponse);
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
        request->send(400, "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    s.setDescription("Impostazioni salvate con successo, il dispositivo verrà riavviato");
    response = BasicResponse(s);
    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}