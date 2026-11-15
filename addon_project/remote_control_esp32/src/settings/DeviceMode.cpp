#include "DeviceMode.h"

#include <utility>

static const std::pair<DEVICE_MODE, const char *> DEVICE_MODE_PAIR[] = {
  {DEVICE_MODE::REMOTE_CONTROL, "remote_control"},
};

DEVICE_MODE deviceModeStringToEnum(String inputString)
{
  inputString.toLowerCase();
  for (const auto &entry : DEVICE_MODE_PAIR)
  {
    if (inputString.equals(entry.second))
    {
      return entry.first;
    }
  }
  return DEVICE_MODE::UNKNOWN;
}

String deviceModeEnumToString(DEVICE_MODE inputEnum)
{
  for (const auto &entry : DEVICE_MODE_PAIR)
  {
    if (entry.first == inputEnum)
    {
      return entry.second;
    }
  }
  return "unknown";
}
