#include "SettingsResponse.h"
#include <utils/FunctionUtils.h>
#include "constants/Constants.h"

SettingsResponse::SettingsResponse() : status(), data() {}

SettingsResponse::SettingsResponse(StatusInfo info)
{
    status = info;
}

// SettingsResponse::SettingsResponse(StatusInfo info, const SettingsModel &dataInput)
// {
//     status = info;
//     data = dataInput;
// }

// SettingsResponse::SettingsResponse(const int &code, const String &message, const String &description, const SettingsModel &dataInput)
// {
//     BasicResponse(code, message, description);
//     data = dataInput;
// }

// SettingsResponse::SettingsResponse(HTTP_CODE code)
// {
//     status = getStatusInfoByHttpCode(code);
// }

// SettingsResponse::SettingsResponse(HTTP_CODE code, String customDescription)
// {
//     status = getStatusInfoByHttpCode(code);
//     status.setDescription(customDescription);
// }

// SettingsResponse::SettingsResponse(StatusInfo info)
// {
//     status = info;
// }

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
    return data;
}

void SettingsResponse::setDataJson(const SettingsModel &json)
{
    data = json;
}

String SettingsResponse::toString()
{
    return "{" + status.toString() + ", " + data.toString() + "}";
}

String SettingsResponse::toJson()
{
    // Crea un nuovo documento JSON per combinare le informazioni
    DynamicJsonDocument doc(2560);
    deserializeJson(doc, status.toJson()); // Deserializza il JSON della classe base

    // Serializza il modello di impostazioni
    JsonObject dataJson = doc.createNestedObject("data");
    StaticJsonDocument<2048> dataDoc;
    DeserializationError statusDataError = deserializeJson(dataDoc, data.toJson());
    if (!statusDataError)
    {
        dataJson.set(dataDoc.as<JsonObject>());
    }
    else
    {
        Serial.println("Error serializing status");

    }
    String output;
    serializeJson(doc, output);
    return output;
}

bool SettingsResponse::fromJson(const String &json)
{
    // Crea un documento JSON per la deserializzazione
    StaticJsonDocument<2048> doc;
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
    JsonObject dataObj = doc["data"].as<JsonObject>();
    if (!dataObj.isNull())
    {
        String dataStr;
        serializeJson(dataObj, dataStr);
        if (!data.fromJson(dataStr))
        {
            return false;
        }
    }
    else
    {
        Serial.println(F("data is null"));
        return false;
    }

    return true;
}