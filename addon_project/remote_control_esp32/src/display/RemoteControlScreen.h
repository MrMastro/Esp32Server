#ifndef REMOTE_CONTROL_SCREEN_H
#define REMOTE_CONTROL_SCREEN_H

#include <Arduino.h>
#include "joystick/Joystick.h"

// Schermata della modalità remote_control:
//   in alto             IP (e OTA ON) a sinistra, WIFI/AP a destra
//   sotto               riga dei pallini dei dispositivi conosciuti (fino a 10)
//   al centro           la direzione J_DIRECTION (verde, arancione in modalità BUTTON)
//   in basso            SW / BUTTON
struct RemoteControlScreenOptions
{
  bool logDirections = false; // stampa "Joystick direction: ..." sulla seriale a ogni cambio
  bool showRawValues = false; // mostra centro calibrato e X/Y grezzi (occupano spazio)
  const char *bottomLabel = nullptr; // scritta in fondo allo schermo (es. "JOYSTICK TEST"), nullptr = nessuna
};

// Da chiamare a ogni giro DOPO joystick.read() (direction = valore letto): ridisegna solo se qualcosa
// è cambiato (max ~30 fps). La lettura è fuori, così la direzione si invia ai dispositivi senza passare dallo schermo.
void remoteControlScreenUpdate(Joystick &joystick, J_DIRECTION direction, const RemoteControlScreenOptions &options);

// Testo della riga sotto l'intestazione (es. "IP 172.17.252.107  OTA ON").
void remoteControlScreenSetNetworkInfo(const String &info);

// Schermata definitiva della modalità remote_control.
// showRawValues: true mostra centro calibrato e X/Y grezzi del joystick.
void remoteControlOnDisplay(Joystick &joystick, J_DIRECTION direction, bool showRawValues = false);

#endif // REMOTE_CONTROL_SCREEN_H
