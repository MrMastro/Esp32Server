#ifndef BasicResponse_H
#define BasicResponse_H

#include <./models/StatusInfo.h>

class BasicResponse
{
public:
    BasicResponse();
    BasicResponse(StatusInfo info, const String &dataJson="");
    BasicResponse(const String &message, const String &description, const String &dataJson="");
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