#ifndef REMOTE_PAIRING_H
#define REMOTE_PAIRING_H

#include <Arduino.h>
#include "WirelessProtocol.h"

// Telecomando accoppiato (fase 1 del joystick wireless): MAC e chiave di sessione.
// Scritto dall'endpoint POST /api/v1/remote/pair (task del web server), letto dal task wireless:
// accesso protetto da una sezione critica. Un nuovo accoppiamento sostituisce il precedente.
// Solo in RAM: dopo un riavvio del dispositivo il telecomando deve riaccoppiarsi.
struct RemotePairing
{
  bool valid = false;
  uint8_t remoteMac[WIRELESS_MAC_LEN] = {0};
  uint8_t sessionKey[WIRELESS_SESSION_KEY_LEN] = {0};
  uint32_t pairedMs = 0;   // millis() dell'accoppiamento
  uint32_t generation = 0; // cambia a ogni accoppiamento (impostato da remotePairingSet): chi legge
                           // azzera seq e stato quando cambia
};

// Salva un nuovo accoppiamento (generation viene assegnato qui)
void remotePairingSet(const RemotePairing &pairing);

// Copia del telecomando accoppiato; false se nessuno
bool remotePairingGet(RemotePairing &out);

#endif // REMOTE_PAIRING_H
