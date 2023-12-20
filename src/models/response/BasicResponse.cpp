#include "BasicResponse.h"
#include <utils/FunctionUtils.h>

BasicResponse::BasicResponse() : status(), dataJson("") {}

BasicResponse::BasicResponse(StatusInfo info, const String &dataJsonInput)
{
    Serial.println("BasicResponse constructor:");
    status = info;
    if(dataJsonInput != "")
        dataJson = dataJsonInput;
}

BasicResponse::BasicResponse(const String &message, const String &description, const String &dataJsonInput){
    status = StatusInfo(message,description);
    dataJson = dataJsonInput;
}

StatusInfo BasicResponse::getStatus()
{
    return status;
}

void BasicResponse::setStatus(StatusInfo &newStatus)
{
    status = newStatus;
}

StatusInfo::StatusInfo(): message(""), description() {}

StatusInfo::StatusInfo(const String &message, const String &description)
    : message(message), description(description) {}

String StatusInfo::getMessage()
{
    return message;
}

String StatusInfo::getDescription()
{
    return description;
}

void StatusInfo::setMessage(const String &newMessage)
{
    message = newMessage;
}

void StatusInfo::setDescription(const String &newDescription)
{
    description = newDescription;
}

String StatusInfo::toString()
{
    return "{ " + message + ", "+ description + " }";
}

String BasicResponse::getDataJson()
{
    return dataJson;
}

void BasicResponse::setDataJson(const String &json)
{
    dataJson = dataJson;
}

String BasicResponse::toString()
{
    return "{" + status.toString() + ", "+ dataJson +"}";
}
