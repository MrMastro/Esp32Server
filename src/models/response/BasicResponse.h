#include <ArduinoJson.h>
#include <iostream>
#ifndef BasicResponse_H
#define BasicResponse_H

// Custom class to store additional information
class StatusInfo
{
public:
    StatusInfo();
    StatusInfo(const String &message, const String &description);
    String getMessage();
    String getDescription();
    void setMessage(const String &newMessage);
    void setDescription(const String &newDescription);
    String toString();
private:
    String message;
    String description;
};

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