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
  INFO,
  UNKNOWN
};

const std::pair<CMD, String> CMD_PAIR[] = {
    {CMD::LED_ON, "led on"},
    {CMD::LED_OFF, "led off"},
    {CMD::LED_TOGGLE, "led toggle"},
    {CMD::START_PROGRESS_BAR, "start progress bar"},
    {CMD::OFF_STRIPT, "off stript"},
    {CMD::INFO, "info"},
    {CMD::UNKNOWN, "Unknown command"}
    // Add more status codes and information as needed
};

//FUNCTION FOR MAPPING
CMD mapStringToEnum(String inputString);

#endif  // COMMAND_H