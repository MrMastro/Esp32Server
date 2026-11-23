#include "CommandTransportMode.h"

#include <utility>

static const std::pair<COMMAND_TRANSPORT_MODE, const char *> COMMAND_TRANSPORT_MODE_PAIR[] = {
  {COMMAND_TRANSPORT_MODE::ESP_NOW, "ESP_NOW"},
  {COMMAND_TRANSPORT_MODE::API_CALL, "API_CALL"},
};

COMMAND_TRANSPORT_MODE commandTransportModeStringToEnum(String inputString)
{
  inputString.trim();
  inputString.toUpperCase();
  for (const auto &entry : COMMAND_TRANSPORT_MODE_PAIR)
  {
    if (inputString.equals(entry.second))
    {
      return entry.first;
    }
  }
  return COMMAND_TRANSPORT_MODE::UNKNOWN_MODE;
}

String commandTransportModeEnumToString(COMMAND_TRANSPORT_MODE inputEnum)
{
  for (const auto &entry : COMMAND_TRANSPORT_MODE_PAIR)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  return "UNKNOWN_MODE";
}
