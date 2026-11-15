#include "CommunicationMode.h"

#include <utility>

static const std::pair<COMMUNICATION_MODE, const char *> COMMUNICATION_MODE_PAIR[] = {
  {COMMUNICATION_MODE::AP_MODE, "AP_MODE"},
  {COMMUNICATION_MODE::WIFI_MODE, "WIFI_MODE"},
};

COMMUNICATION_MODE communicationModeStringToEnum(String inputString)
{
  inputString.toUpperCase();
  for (const auto &entry : COMMUNICATION_MODE_PAIR)
  {
    if (inputString.equals(entry.second))
    {
      return entry.first;
    }
  }
  return COMMUNICATION_MODE::UNKNOWN_MODE;
}

String communicationModeEnumToString(COMMUNICATION_MODE inputEnum)
{
  for (const auto &entry : COMMUNICATION_MODE_PAIR)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  return "UNKNOWN_MODE";
}
