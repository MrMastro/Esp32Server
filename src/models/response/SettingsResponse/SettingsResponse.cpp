#include "SettingsResponse.h"
#include <utils/FunctionUtils.h>
#include "constants/Constants.h"

SettingsResponse::SettingsResponse() : status(), dataJson() {}

SettingsResponse::SettingsResponse(StatusInfo info, const SettingsModel &dataJsonInput)
{
    status = info;
    dataJson = dataJsonInput;
}

SettingsResponse::SettingsResponse(const int &code, const String &message, const String &description, const SettingsModel &dataJsonInput){
    status = StatusInfo(code, message, description);
    dataJson = dataJsonInput;
}

SettingsResponse::SettingsResponse(HTTP_CODE code)
{
    status = getStatusInfoByHttpCode(code);
}

SettingsResponse::SettingsResponse(HTTP_CODE code, String customDescription)
{
    status = getStatusInfoByHttpCode(code);
    status.setDescription(customDescription);
}

SettingsResponse::SettingsResponse(StatusInfo info)
{
    status = info;
}

StatusInfo SettingsResponse::getStatus()
{
    return status;
}

void SettingsResponse::setStatus(StatusInfo &newStatus)
{
    status = newStatus;
}

SettingsModel SettingsResponse::getDataJson()
{
    return dataJson;
}

void SettingsResponse::setDataJson(const SettingsModel &json)
{
    dataJson = json;
}

String SettingsResponse::toString()
{
    return "{" + status.toString() + ", " + dataJson.toString() + "}";
}

String SettingsResponse::toJson()
{
    // Crea un documento JSON
    StaticJsonDocument<896> doc;

    // Crea un oggetto annidato per "status" e usa il metodo toJson di StatusInfo
    JsonObject statusJson = doc.createNestedObject("status");
    StaticJsonDocument<128> tempStatusDoc;
    DeserializationError statusError = deserializeJson(tempStatusDoc, status.toJson());
    if (!statusError)
    {
        statusJson.set(tempStatusDoc.as<JsonObject>());
    }

    // Crea un oggetto annidato per "dataJson" e usa il metodo toJson di SettingsModel
    JsonObject dataJsonObj = doc.createNestedObject("data");
    StaticJsonDocument<768> tempDataJsonDoc;
    DeserializationError dataJsonError = deserializeJson(tempDataJsonDoc, dataJson.toJson());
    if (!dataJsonError)
    {
        dataJsonObj.set(tempDataJsonDoc.as<JsonObject>());
    }
    else
    {
        doc["dataJson"] = nullptr; // Se c'è un errore, imposta null
        Serial.println("Error deseralizarion SettingsModel");
    }

    // Converte il documento in una stringa JSON
    String output;
    serializeJson(doc, output);
    
    return output;
}

bool SettingsResponse::fromJson(const String &json)
{
    // Crea un documento JSON per la deserializzazione
    StaticJsonDocument<896> doc;
    DeserializationError error = deserializeJson(doc, json);

    // Controlla se la deserializzazione ha avuto successo
    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }

    // Deserializza lo status come oggetto JSON
    JsonObject statusJson = doc["status"].as<JsonObject>();
    if (!statusJson.isNull())
    {
        String statusJsonStr;
        serializeJson(statusJson, statusJsonStr);
        if (!status.fromJson(statusJsonStr))
        {
            return false;
        }
    }
    else
    {
        Serial.println(F("status is null"));
        return false;
    }

    // Deserializza dataJson come oggetto JSON
    JsonObject dataJsonObj = doc["data"].as<JsonObject>();
    if (!dataJsonObj.isNull())
    {
        String dataJsonStr;
        serializeJson(dataJsonObj, dataJsonStr);
        if (!dataJson.fromJson(dataJsonStr))
        {
            return false;
        }
    }
    else
    {
        Serial.println(F("dataJson is null"));
        return false;
    }

    return true;
}