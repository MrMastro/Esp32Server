#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <Arduino.h>
#include "JoystickConfig.h"
#include "JoystickDirections.h"

// Logica del joystick (come nel progetto principale Esp32Server):
//  - gli assi danno una delle 9 direzioni base:
//      CENTER, UP, DOWN, LEFT, RIGHT, UP_LEFT, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT
//    (asse X -> UP / DOWN, asse Y -> RIGHT / LEFT)
//  - premendo il tasto centrale (SW) si entra in "modalità BUTTON": la stessa direzione
//    diventa la sua variante *_BUTTON (CENTER_BUTTON, UP_BUTTON, ... DOWN_RIGHT_BUTTON)
//  - ripremendo il tasto si torna alle direzioni senza BUTTON
class Joystick
{
public:
  void begin(const JoystickConfig &config);

  // Media di 'samples' letture a riposo per trovare il centro.
  void calibrate(int samples = 50);

  // Legge gli ingressi e ritorna la direzione corrente (base o *_BUTTON).
  J_DIRECTION read();

  // true se l'ultima read() ha prodotto una direzione diversa dalla precedente.
  bool changed() const { return _changed; }

  int rawX() const { return _x; }
  int rawY() const { return _y; }
  bool switchPressed() const { return _swLast == LOW; }
  bool buttonMode() const { return _buttonMode; }
  int centerX() const { return _cfg.centerX; }
  int centerY() const { return _cfg.centerY; }

  // Converte una direzione base nella sua variante *_BUTTON (CENTER -> CENTER_BUTTON, ...).
  static J_DIRECTION toButtonDirection(J_DIRECTION base);

private:
  // Dopo una pressione, i fronti entro questo tempo sono rimbalzi e vengono ignorati.
  static const uint32_t BUTTON_LOCKOUT_MS = 200;

  JoystickConfig _cfg;
  int _x = 0;
  int _y = 0;

  int _swLast = HIGH;
  uint32_t _lastToggleMs = 0;
  bool _buttonMode = false;

  J_DIRECTION _lastDirection = J_DIRECTION::NONE;
  bool _changed = false;

  void updateButton();
  J_DIRECTION readBaseDirection();
};

#endif // JOYSTICK_H
