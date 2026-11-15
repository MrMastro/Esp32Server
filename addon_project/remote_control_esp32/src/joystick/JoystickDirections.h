#ifndef JOYSTICK_DIRECTIONS
#define JOYSTICK_DIRECTIONS

// Copia di Esp32Server/src/constants/JoystickDirections.h:
// le stringhe sono i comandi condivisi con il progetto principale.

#include <Arduino.h>

enum class J_DIRECTION
{
  CENTER,
  UP,
  DOWN,
  LEFT,
  RIGHT,
  UP_LEFT,
  UP_RIGHT,
  DOWN_LEFT,
  DOWN_RIGHT,
  CENTER_BUTTON,
  UP_BUTTON,
  DOWN_BUTTON,
  LEFT_BUTTON,
  RIGHT_BUTTON,
  UP_LEFT_BUTTON,
  UP_RIGHT_BUTTON,
  DOWN_LEFT_BUTTON,
  DOWN_RIGHT_BUTTON,
  NONE
};

J_DIRECTION mapStringToJdirections(String inputString);
String mapJdirectionsToString(J_DIRECTION input);

#endif // JOYSTICK_DIRECTIONS
