#ifndef JOYSTICK_DISPLAY_TEST_H
#define JOYSTICK_DISPLAY_TEST_H

#include "joystick/Joystick.h"

// Test: mostra a schermo la stringa J_DIRECTION corrispondente al joystick,
// più i valori grezzi (X, Y, SW) e il centro calibrato.
// Da chiamare in loop(); commentare la chiamata per disattivarlo.
void testJoystickOnDisplay(Joystick &joystick);

// Testo mostrato nell'intestazione del test (es. "AP 192.168.4.1").
void testJoystickSetNetworkInfo(const String &info);

#endif // JOYSTICK_DISPLAY_TEST_H
