#ifndef JOYSTICK_CONFIG_H
#define JOYSTICK_CONFIG_H

#include <Arduino.h>
#include "BoardPins.h"

// Configurazione del joystick, letta da /config/joystick.json sulla microSD.
// I nomi dei campi pin sono gli stessi di JoystickSettingsModel del progetto principale.
struct JoystickConfig
{
  bool enableJoystick = true;
  int pinSwitch = DEFAULT_PIN_JOY_SW;
  int pinAnalogX = DEFAULT_PIN_JOY_VRX;
  int pinAnalogY = DEFAULT_PIN_JOY_VRY;

  // Centro dell'ADC (0-4095). Usato solo se autoCalibrate = false.
  int centerX = 1930;
  int centerY = 1960;
  // Scostamento dal centro sotto il quale l'asse è considerato centrato.
  int deadZone = 400;
  // Se true, all'avvio misura il centro (non toccare la levetta durante il boot).
  bool autoCalibrate = true;

  // Carica i valori dal file; i campi mancanti restano ai default.
  // Ritorna false se il file non esiste o non è un JSON valido.
  bool loadFromFile(const char *path);
};

#endif // JOYSTICK_CONFIG_H
