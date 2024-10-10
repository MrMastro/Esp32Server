#include "BasicResponse.h"
#include <utils/FunctionUtils.h>
#include "constants/Constants.h"

BasicResponse::BasicResponse() : status(), dataJson("") {}

BasicResponse::BasicResponse(StatusInfo info, const String &dataJsonInput)
{
    status = info;
    if(dataJsonInput != "")
        dataJson = dataJsonInput;
}

BasicResponse::BasicResponse(const int &code, const String &message, const String &description, const String &dataJsonInput){
    status = StatusInfo(code, message, description);
    dataJson = dataJsonInput;
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

BasicResponse::BasicResponse(StatusInfo info)
{
    status = info;
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
