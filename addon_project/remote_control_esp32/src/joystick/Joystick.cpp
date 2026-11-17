#include "Joystick.h"
#include "system/Log.h"

void Joystick::begin(const JoystickConfig &config)
{
  _cfg = config;
  pinMode(_cfg.pinSwitch, INPUT_PULLUP);
  analogReadResolution(12); // 0-4095, come sull'ESP32 del progetto principale

  _swLast = digitalRead(_cfg.pinSwitch);
}

void Joystick::calibrate(int samples)
{
  long sumX = 0, sumY = 0;
  for (int i = 0; i < samples; i++)
  {
    sumX += analogRead(_cfg.pinAnalogX);
    sumY += analogRead(_cfg.pinAnalogY);
    delay(10);
  }
  _cfg.centerX = sumX / samples;
  _cfg.centerY = sumY / samples;
  logInfof("[Joystick] centro calibrato -> X: %d | Y: %d\n", _cfg.centerX, _cfg.centerY);
}

// Ogni pressione del tasto centrale (fronte HIGH -> LOW) alterna la modalità BUTTON.
// Il fronte viene accettato subito; i successivi entro BUTTON_LOCKOUT_MS sono rimbalzi.
void Joystick::updateButton()
{
  int sw = digitalRead(_cfg.pinSwitch);
  uint32_t now = millis();

  if (_swLast == HIGH && sw == LOW && (now - _lastToggleMs) >= BUTTON_LOCKOUT_MS)
  {
    _buttonMode = !_buttonMode;
    _lastToggleMs = now;
    _pressStartMs = now;
    _pressReleased = false;
    _longPressFired = false;
  }
  // rilascio vero (non un rimbalzo subito dopo la pressione): la pressione lunga si interrompe
  if (sw == HIGH && (now - _lastToggleMs) >= BUTTON_LOCKOUT_MS)
    _pressReleased = true;
  _swLast = sw;
}

uint32_t Joystick::heldMs() const
{
  if (_pressReleased || _swLast != LOW)
    return 0;
  return millis() - _pressStartMs;
}

bool Joystick::takeLongPress(uint32_t thresholdMs)
{
  if (_longPressFired || heldMs() < thresholdMs)
    return false;
  _longPressFired = true;
  _buttonMode = !_buttonMode; // annulla il toggle BUTTON di inizio pressione
  return true;
}

J_DIRECTION Joystick::readBaseDirection()
{
  _x = analogRead(_cfg.pinAnalogX);
  _y = analogRead(_cfg.pinAnalogY);

  const int dx = _x - _cfg.centerX;
  const int dy = _y - _cfg.centerY;
  const int dz = _cfg.deadZone;

  const bool up = dx > dz;
  const bool down = dx < -dz;
  const bool right = dy > dz;
  const bool left = dy < -dz;

  if (up && left) return J_DIRECTION::UP_LEFT;
  if (up && right) return J_DIRECTION::UP_RIGHT;
  if (down && left) return J_DIRECTION::DOWN_LEFT;
  if (down && right) return J_DIRECTION::DOWN_RIGHT;
  if (up) return J_DIRECTION::UP;
  if (down) return J_DIRECTION::DOWN;
  if (left) return J_DIRECTION::LEFT;
  if (right) return J_DIRECTION::RIGHT;
  return J_DIRECTION::CENTER;
}

J_DIRECTION Joystick::toButtonDirection(J_DIRECTION base)
{
  switch (base)
  {
  case J_DIRECTION::CENTER: return J_DIRECTION::CENTER_BUTTON;
  case J_DIRECTION::UP: return J_DIRECTION::UP_BUTTON;
  case J_DIRECTION::DOWN: return J_DIRECTION::DOWN_BUTTON;
  case J_DIRECTION::LEFT: return J_DIRECTION::LEFT_BUTTON;
  case J_DIRECTION::RIGHT: return J_DIRECTION::RIGHT_BUTTON;
  case J_DIRECTION::UP_LEFT: return J_DIRECTION::UP_LEFT_BUTTON;
  case J_DIRECTION::UP_RIGHT: return J_DIRECTION::UP_RIGHT_BUTTON;
  case J_DIRECTION::DOWN_LEFT: return J_DIRECTION::DOWN_LEFT_BUTTON;
  case J_DIRECTION::DOWN_RIGHT: return J_DIRECTION::DOWN_RIGHT_BUTTON;
  default: return J_DIRECTION::NONE;
  }
}

J_DIRECTION Joystick::read()
{
  updateButton();
  J_DIRECTION direction = readBaseDirection();
  if (_buttonMode)
  {
    direction = toButtonDirection(direction);
  }

  _changed = (direction != _lastDirection);
  _lastDirection = direction;
  return direction;
}
