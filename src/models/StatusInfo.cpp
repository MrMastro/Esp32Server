#include "StatusInfo.h"

StatusInfo::StatusInfo(): message(""), description() {}

StatusInfo::StatusInfo(const String &message, const String &description)
    : message(message), description(description) {}

String StatusInfo::getMessage()
{
    return message;
}

String StatusInfo::getDescription()
{
    return description;
}

void StatusInfo::setMessage(const String &newMessage)
{
    message = newMessage;
}

void StatusInfo::setDescription(const String &newDescription)
{
    description = newDescription;
}

String StatusInfo::toString()
{
    return "{ " + message + ", "+ description + " }";
}