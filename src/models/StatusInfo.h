#ifndef STATUS_INFO_H
#define STATUS_INFO_H

#include <Arduino.h>

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

#endif //STATUS_INFO_H