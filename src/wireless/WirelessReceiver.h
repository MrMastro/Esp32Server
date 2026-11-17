#ifndef WIRELESS_RECEIVER_H
#define WIRELESS_RECEIVER_H

#include <Arduino.h>

// Ricevitore ESP-NOW del joystick wireless (fase 2, addon_project/remote_control_esp32/docs/WIRELESS_JOYSTICK.md).
// La callback ESP-NOW (task WiFi) mette solo il pacchetto in coda; wirelessReceiverProcess(), chiamata in loop
// dal task wireless, verifica mittente, firma e seq e chiama l'handler solo quando la direzione cambia.

// Riceve la stringa della direzione ("UP", "CENTER_BUTTON", ...)
typedef void (*WirelessDirectionHandler)(const String &direction);

// Avvia ESP-NOW (il WiFi deve essere gia' attivo) e mette la radio in modem sleep (riposo). false se l'avvio fallisce.
// sessionTimeoutS: senza SESSION_START/DIRECTION per questo tempo la sessione si chiude da sola (modem sleep).
bool wirelessReceiverBegin(WirelessDirectionHandler onDirection, uint32_t sessionTimeoutS);

// Attende un pacchetto fino a waitMs e lo elabora.
void wirelessReceiverProcess(uint32_t waitMs);

#endif // WIRELESS_RECEIVER_H
