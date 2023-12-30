#ifndef BasicResponse_H
#define BasicResponse_H

#include "./models/StatusInfo.h"
#include <./constants/constants.h>

class BasicResponse
{
public:
    BasicResponse();
    BasicResponse(StatusInfo info);
    BasicResponse(StatusInfo info, const String &dataJson);
    BasicResponse(const String &message, const String &description, const String &dataJson);
    BasicResponse(HTTP_CODE code);
    BasicResponse(HTTP_CODE code, String customDescription);
    StatusInfo getStatus();
    void setStatus(StatusInfo &status);
    String getDataJson();
    void setDataJson(const String &json);
    String toString();
private:
    StatusInfo status;
    String dataJson;
};

#endif