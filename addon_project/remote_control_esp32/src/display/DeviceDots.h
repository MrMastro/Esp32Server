#ifndef DEVICE_DOTS_H
#define DEVICE_DOTS_H

#include <Arduino.h>

// Riga di pallini, uno per dispositivo di espDevices.json (nell'ordine del file),
// nel colore "color" del dispositivo:
//   OFFLINE   -> pallino vuoto (solo bordo)
//   ONLINE    -> pallino mezzo pieno (metà inferiore piena)
//   CONNECTED -> pallino pieno
// I pallini (raggio 8 px, uno ogni 22 px) sono centrati tra 'left' e 'right' sulla riga 'centerY';
// a tutta larghezza ne entrano fino a 10 (ESP_DEVICES_MAX).

// true se lo stato dei dispositivi è cambiato dall'ultimo deviceDotsDraw()
bool deviceDotsChanged();

// Disegna i pallini sul canvas (senza flush)
void deviceDotsDraw(int16_t centerY, int16_t left, int16_t right);

#endif // DEVICE_DOTS_H
