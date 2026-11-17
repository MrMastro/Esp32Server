#ifndef JOYSTICK_DISPLAY_TEST_H
#define JOYSTICK_DISPLAY_TEST_H

#include "joystick/Joystick.h"

// Test: la schermata remote_control (RemoteControlScreen) con la scritta "JOYSTICK TEST" in fondo.
// Da chiamare in loop al posto di remoteControlOnDisplay(); commentare la chiamata per disattivarlo.
// logDirections: se true stampa sulla seriale la direzione a ogni cambio.
// showRawValues: se true mostra centro calibrato e X/Y grezzi del joystick.
void testJoystickOnDisplay(Joystick &joystick, J_DIRECTION direction, bool logDirections = true, bool showRawValues = true);

#endif // JOYSTICK_DISPLAY_TEST_H
