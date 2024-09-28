#include "CommunicationMode.h"

const COMMUNICATION_MODE communicationModeStringToEnum(String inputString)
{
    inputString.toUpperCase();
    for (const auto &entry : COMMUNICATION_MODE_PAIR)
    {
        if (entry.second == inputString)
        {
            return entry.first;
        }
    }
    // Return a special value or throw an exception if the string is not found
    return COMMUNICATION_MODE::UNKNOWN_MODE;
}

const String communicationModeEnumToString(COMMUNICATION_MODE inputEnum)
{
    for (const auto &entry : COMMUNICATION_MODE_PAIR)
    {
        if (entry.first == inputEnum)
        {
            return entry.second;
        }
    }
    // Return a special value or throw an exception if the string is not found
    return "UKNOWN_EFFECT";
}
