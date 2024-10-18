#ifndef BasicResponseString_H
#define BasicResponseString_H

#define DEPRECATED __attribute__((deprecated))

#include "./models/StatusInfo.h"
#include <./constants/Constants.h>

enum class HTTP_CODE;

class StatusInfo;

class BasicResponseString
{
public:
    BasicResponseString();
    BasicResponseString(StatusInfo info);
    BasicResponseString(StatusInfo info, const String &dataJson);
    BasicResponseString(const int &code, const String &message, const String &description, const String &dataJsonInput);
    BasicResponseString(HTTP_CODE code);
    BasicResponseString(HTTP_CODE code, String customDescription);
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