#include "JoystickDisplayTest.h"

#include "display/RemoteControlScreen.h"

// Stessa schermata di remoteControlOnDisplay(), con la scritta "JOYSTICK TEST" in fondo.
void testJoystickOnDisplay(Joystick &joystick, bool logDirections, bool showRawValues)
{
  RemoteControlScreenOptions options;
  options.logDirections = logDirections;
  options.showRawValues = showRawValues;
  options.bottomLabel = "JOYSTICK TEST";
  remoteControlScreenUpdate(joystick, options);
}
