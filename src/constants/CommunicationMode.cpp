#include "CommunicationMode.h"

const std::pair<COMMUNICATION_MODE, String> COMMUNICATION_MODE_PAIR[] = {
    {COMMUNICATION_MODE::AP_MODE, "AP_MODE"},
    {COMMUNICATION_MODE::WIFI_MODE, "WIFI_MODE"},
    {COMMUNICATION_MODE::BLUETOOTH_MODE, "BLUETOOTH_MODE"},
    {COMMUNICATION_MODE::HYBRID_BLUETOOTH_AP, "HYBRID_BLUETOOTH_AP"},
    {COMMUNICATION_MODE::HYBRID_BLUETOOTH_WIFI, "HYBRID_BLUETOOTH_WIFI"},
    {COMMUNICATION_MODE::UNKNOWN_MODE, "UKNOWN_MODE"}
    // Add more status codes and information as needed
};

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
    return communicationModeEnumToString(COMMUNICATION_MODE::UNKNOWN_MODE);
}
