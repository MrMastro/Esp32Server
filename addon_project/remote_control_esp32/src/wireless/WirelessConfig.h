#ifndef WIRELESS_CONFIG_H
#define WIRELESS_CONFIG_H

#include <Arduino.h>

// Configurazione del joystick wireless, letta da /config/wireless.json sulla microSD
// (docs/WIRELESS_JOYSTICK.md, sezione Configurazioni). Se il file manca si usano i default.
struct WirelessConfig
{
  uint16_t sessionTimeoutS = 15;    // secondi senza movimenti prima di SESSION_END (e del passaggio all'heartbeat lento)
  uint32_t heartbeatActiveMs = 1000; // intervallo heartbeat durante l'uso
  uint16_t heartbeatIdleS = 15;     // intervallo heartbeat a riposo
  uint8_t missedHeartbeats = 3;     // heartbeat senza ACK prima di considerare il dispositivo non collegato
  uint16_t rediscoveryS = 60;       // intervallo di riscoperta dei dispositivi non collegati
  uint8_t pairAttempts = 2;         // tentativi HTTP per dispositivo (il primo contatto dopo l'avvio spesso fallisce)
  uint8_t longPressS = 5;           // secondi di pressione di SW per sospendere / riprendere (fase 5)
  bool remoteModemSleep = true;     // modem sleep del telecomando a riposo (false: radio sempre accesa, consuma di piu')

  // Carica i valori dal file; i campi mancanti restano ai default.
  // Ritorna false se il file non esiste o non è un JSON valido.
  bool loadFromFile(const char *path);
};

#endif // WIRELESS_CONFIG_H
