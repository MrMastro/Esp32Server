#include "Commands.h"

const std::pair<CMD, String> CMD_PAIR[] = {
  {CMD::LED_ON, "led_on"},
  {CMD::LED_OFF, "led_off"},
  {CMD::LED_TOGGLE, "led_toggle"},
  {CMD::START_PROGRESS_BAR, "start_progress_bar"},
  {CMD::START_EFFECT, "start_effect"},
  {CMD::STOP_EFFECT, "stop_effect"},
  {CMD::OFF_STRIP, "off_strip"},
  {CMD::CHANGE_COMMUNICATION_MODE, "change_communication_mode"},
  {CMD::REBOOT, "reboot"},
  {CMD::INFO, "info"},
  {CMD::INFO_IP, "info_ip"},
  {CMD::NONE, "none"},
  {CMD::UNKNOWN, "unknown_command"}
  // Add more status codes and information as needed
};

CMD mapStringToEnum(String inputString)
{
  inputString.toLowerCase();
  for (const auto &entry : CMD_PAIR)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return CMD::UNKNOWN;
}

String mapEnumToString(CMD input)
{
  for (const auto &entry : CMD_PAIR)
  {
    if (entry.first == input)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return mapEnumToString(CMD::UNKNOWN);
}
