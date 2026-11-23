#ifndef API_COMMAND_SENDER_H
#define API_COMMAND_SENDER_H

#include <Arduino.h>
#include "joystick/JoystickDirections.h"

// Modalita' API-CALL (commandTransportMode = API_CALL in main_settings.json): niente ESP-NOW,
// a ogni cambio di direzione del joystick (tranne CENTER e CENTER_BUTTON) il telecomando invia a ogni
// dispositivo di espDevices.json una sola chiamata:
//   POST http://<ip>/api/v1/sendCommand   {"command": "<direzione>"}   (es. "DOWN_BUTTON")
// Il dispositivo la tratta come il suo joystick fisico (preset LED con quel trigger).
//
// Un task per dispositivo, ognuno con la sua coda: un dispositivo spento o lento non ritarda gli altri.
// Pallini: CONNECTED (pieno) se l'ultima chiamata ha avuto risposta HTTP, OFFLINE (vuoto) se no;
// all'avvio una GET /api/v1/getInfoHello li inizializza.

#define API_SEND_COMMAND_PATH "/api/v1/sendCommand"

// Avvia i task (uno per dispositivo con ip e macAdress). Da chiamare con il WiFi gia' collegato.
void apiCommandSenderBegin();

// Accoda la direzione per tutti i dispositivi (non bloccante). CENTER, CENTER_BUTTON e NONE: ignorate.
// Restituisce true se la direzione e' stata accodata.
bool apiCommandSend(J_DIRECTION direction);

// Stato per la seriale (comando devices)
String apiCommandSenderStatus();

#endif // API_COMMAND_SENDER_H
