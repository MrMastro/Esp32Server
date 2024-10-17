#ifndef BasicResponse_H
#define BasicResponse_H

#define DEPRECATED __attribute__((deprecated))

#include "./models/StatusInfo.h"
#include <./constants/Constants.h>

enum class HTTP_CODE;

class StatusInfo;

class BasicResponse
{
public:
    BasicResponse();
    BasicResponse(StatusInfo info);
    BasicResponse(const int &code, const String &message, const String &description);
    BasicResponse(HTTP_CODE code);
    BasicResponse(HTTP_CODE code, String customDescription);
    StatusInfo getStatus();
    void setStatus(StatusInfo &status);
    String toJson();
    bool fromJson(const String &json);
    String toString();
private:
    StatusInfo status;
};

#endif