#include "BasicResponse.h"
#include <utils/FunctionUtils.h>

BasicResponse::BasicResponse() : status(), dataJson("") {}

BasicResponse::BasicResponse(StatusInfo info, const String &dataJsonInput)
{
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
