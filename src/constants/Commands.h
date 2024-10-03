#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <Arduino.h>

//COMMAND OF COMMANDSERVICE
enum class CMD
{
  LED_ON,
  LED_OFF,
  LED_TOGGLE,
  START_PROGRESS_BAR,
  OFF_STRIPT,
  CHANGE_COMMUNICATION_MODE,
  INFO,
  INFO_IP,
  UNKNOWN
};

const std::pair<CMD, String> CMD_PAIR[] = {
    {CMD::LED_ON, "led_on"},
    {CMD::LED_OFF, "led_off"},
    {CMD::LED_TOGGLE, "led_toggle"},
    {CMD::START_PROGRESS_BAR, "start_progress_bar"},
    {CMD::OFF_STRIPT, "off_stript"},
    {CMD::CHANGE_COMMUNICATION_MODE, "change_communication_mode"},
    {CMD::INFO, "info"},
    {CMD::INFO_IP, "info_ip"},
    {CMD::UNKNOWN, "unknown_command"}
    // Add more status codes and information as needed
};

//FUNCTION FOR MAPPING
CMD mapStringToEnum(String inputString);
String mapEnumToString(CMD input);

#endif  // COMMAND_H