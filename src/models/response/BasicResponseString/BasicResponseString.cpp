#include "BasicResponseString.h"
#include <utils/FunctionUtils.h>
#include "constants/Constants.h"

BasicResponseString::BasicResponseString() : status(), dataJson("") {}

BasicResponseString::BasicResponseString(StatusInfo info, const String &dataJsonInput)
{
    status = info;
    if(dataJsonInput != "")
        dataJson = dataJsonInput;
}

BasicResponseString::BasicResponseString(const int &code, const String &message, const String &description, const String &dataJsonInput){
    status = StatusInfo(code, message, description);
    dataJson = dataJsonInput;
}

BasicResponseString::BasicResponseString(HTTP_CODE code)
{
    status = getStatusInfoByHttpCode(code);
}

BasicResponseString::BasicResponseString(HTTP_CODE code, String customDescription)
{
    status = getStatusInfoByHttpCode(code);
    status.setDescription(customDescription);
}

BasicResponseString::BasicResponseString(StatusInfo info)
{
    status = info;
}

StatusInfo BasicResponseString::getStatus()
{
    return status;
}

void BasicResponseString::setStatus(StatusInfo &newStatus)
{
    status = newStatus;
}

String BasicResponseString::getDataJson()
{
    return dataJson;
}

void BasicResponseString::setDataJson(const String &json)
{
    dataJson = json;
}

String BasicResponseString::toString()
{
    return "{" + status.toString() + ", "+ dataJson +"}";
}
