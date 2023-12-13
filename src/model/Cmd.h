#include <Arduino.h>
#pragma once

enum class Cmd
{
  LED_ON,
  LED_OFF,
  LED_TOGGLE,
  INFO,
  OTHER
};

Cmd mapStringToEnum(String inputString);