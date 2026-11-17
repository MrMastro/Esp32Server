#include "JoystickDisplayTest.h"

#include "display/RemoteControlScreen.h"

// Stessa schermata di remoteControlOnDisplay(), con la scritta "JOYSTICK TEST" in fondo.
void testJoystickOnDisplay(Joystick &joystick, J_DIRECTION direction, bool logDirections, bool showRawValues)
{
  RemoteControlScreenOptions options;
  options.logDirections = logDirections;
  options.showRawValues = showRawValues;
  options.bottomLabel = "JOYSTICK TEST";
  remoteControlScreenUpdate(joystick, direction, options);
}
