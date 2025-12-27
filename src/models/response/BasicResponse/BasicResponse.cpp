#include "BasicResponse.h"
#include <utils/FunctionUtils.h>
#include "constants/Constants.h"

BasicResponse::BasicResponse() : status() {}

BasicResponse::BasicResponse(StatusInfo info)
{
    status = info;
}

BasicResponse::BasicResponse(const int &code, const String &message, const String &description){
    status = StatusInfo(code, message, description);
}

BasicResponse::BasicResponse(HTTP_CODE code)
{
    status = getStatusInfoByHttpCode(code);
}

BasicResponse::BasicResponse(HTTP_CODE code, String customDescription)
{
    status = getStatusInfoByHttpCode(code);
    status.setDescription(customDescription);
}

StatusInfo BasicResponse::getStatus()
{
    return status;
}

void BasicResponse::setStatus(StatusInfo &newStatus)
{
    status = newStatus;
}

String BasicResponse::toJson()
{
    // Crea un documento JSON
    JsonDocument doc;

    // Crea un oggetto annidato per "status" e usa il metodo toJson di StatusInfo
    JsonObject statusJson = doc["status"].to<JsonObject>();
    JsonDocument tempStatusDoc;
    DeserializationError statusError = deserializeJson(tempStatusDoc, status.toJson());
    if (!statusError)
    {
        statusJson.set(tempStatusDoc.as<JsonObject>());
    }
    else
    {
        Serial.println("Error serializing status");
    }

    // Converte il documento in una stringa JSON
    String output;
    serializeJson(doc, output);

    return output;
}

bool BasicResponse::fromJson(const String &json)
{
    // Crea un documento JSON per la deserializzazione
    JsonDocument doc;
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

    return true;
}

String BasicResponse::toString()
{
    return "{" + status.toString() +"}";
}
