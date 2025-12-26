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
  START_EFFECT,                 //start_effect,<name_effect>,<rgbColors>,<ms>,<rbg_e>,<ws2811_s_e>,<matrix_e>
  STOP_EFFECT,                  //stop_effect,<name_effect>,<rgbColors>,<ms>,<rbg_e>,<ws2811_s_e>,<matrix_e>
  OFF_STRIP,
  CHANGE_COMMUNICATION_MODE,    //change_communication_mode,<AP_MODE||WIFI_MODE||BLUETOOTH_MODE||> => change_communication_mode,BLUETOOTH_MODE, | change_communication_mode,WIFI_MODE
  REBOOT,
  INFO,
  INFO_IP,
  NONE,
  UNKNOWN
};

//FUNCTION FOR MAPPING
CMD mapStringToEnum(String inputString);
String mapEnumToString(CMD input);

#endif  // COMMAND_H