#ifndef STATUS_INFO_H
#define STATUS_INFO_H

#include <Arduino.h>
#include <ArduinoJson.h>

class StatusInfo
{
public:
    StatusInfo();
    StatusInfo(const int &code, const String &message, const String &description);
    String getMessage();
    String getDescription();
    int getCode();
    void setMessage(const String &newMessage);
    void setDescription(const String &newDescription);
    String toString();
    String toJson() const;
    bool fromJson(const String &json);
private:
    int code;
    String message;
    String description;
};

#endif //STATUS_INFO_H