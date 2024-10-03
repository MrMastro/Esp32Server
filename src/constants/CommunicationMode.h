#ifndef COMMUNICATION_COSTANTS
#define COMMUNICATION_COSTANTS

#include <vector>
#include <Arduino.h>

//COMUNICATION MODE OF SYSTEM
enum class COMMUNICATION_MODE
{
  AP_MODE,
  WIFI_MODE,
  BLUETOOTH_MODE,
  HYBRID_BLUETOOTH_AP,  //NOT AVAIBLE
  HYBRID_BLUETOOTH_WIFI,   //NOT AVAIBLE 
  UNKNOWN_MODE
};

const std::pair<COMMUNICATION_MODE, String> COMMUNICATION_MODE_PAIR[] = {
    {COMMUNICATION_MODE::AP_MODE, "AP_MODE"},
    {COMMUNICATION_MODE::WIFI_MODE, "WIFI_MODE"},
    {COMMUNICATION_MODE::BLUETOOTH_MODE, "BLUETOOTH_MODE"},
    {COMMUNICATION_MODE::HYBRID_BLUETOOTH_AP, "HYBRID_BLUETOOTH_AP"},
    {COMMUNICATION_MODE::HYBRID_BLUETOOTH_WIFI, "HYBRID_BLUETOOTH_WIFI"},
    {COMMUNICATION_MODE::UNKNOWN_MODE, "UKNOWN_MODE"}
    // Add more status codes and information as needed
};

//FUNCTION FOR MAPPING
const COMMUNICATION_MODE communicationModeStringToEnum(String inputString);
const String communicationModeEnumToString(COMMUNICATION_MODE inputEnum);

#endif //COMMUNICATION_COSTANTS