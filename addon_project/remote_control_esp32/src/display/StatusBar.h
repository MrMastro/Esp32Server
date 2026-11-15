#ifndef STATUS_BAR_H
#define STATUS_BAR_H

#include <Arduino.h>
#include "network/WifiConnection.h"

// Indicatori nell'angolo in alto a destra dello schermo (stato della rete).
// Va passato lo stato restituito da wifiBegin().
void statusBarBegin(const WifiStatus *status);

// true se lo stato mostrato non è più aggiornato (es. WiFi caduto): serve ridisegnare.
bool statusBarChanged();

// Disegna gli indicatori nel buffer 'gfx' (poi serve displayFlush()).
void statusBarDraw();

#endif // STATUS_BAR_H
