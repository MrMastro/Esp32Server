#include "Cmd.h"

Cmd mapStringToEnum(String inputString)
{
  inputString.toUpperCase();
  if (inputString.equalsIgnoreCase("led on"))
  {
    return Cmd::LED_ON;
  }
  else if (inputString.equalsIgnoreCase("led off"))
  {
    return Cmd::LED_OFF;
  }
  else if (inputString.equalsIgnoreCase("led toggle"))
  {
    return Cmd::LED_TOGGLE;
  }
  else if (inputString.equalsIgnoreCase("info"))
  {
    return Cmd::INFO;
  }
  else
  {
    // Valore predefinito in caso di stringa non riconosciuta
    return Cmd::OTHER;
  }
}