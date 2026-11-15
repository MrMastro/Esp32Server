#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <Arduino.h>
#include "settings/SettingsModel.h"
#include "settings/CommunicationMode.h"

// Risultato dell'avvio della rete
struct WifiStatus
{
  COMMUNICATION_MODE activeMode = COMMUNICATION_MODE::UNKNOWN_MODE; // modalità effettivamente attiva
  bool fallbackToAp = false; // true se WIFI_MODE è fallito e si è passati all'AP
  String ssid;               // rete a cui si è collegati, o nome dell'AP creato
  String ip;
};

// Avvia la rete come MastroServer del progetto principale:
//  - AP_MODE:   crea l'access point ssidAP / passwordAP
//  - WIFI_MODE: si collega a ssidWIFI; se entro WIFI_CONNECT_TIMEOUT_MS non riesce, crea l'AP
//  - modalità sconosciuta: AP
WifiStatus wifiBegin(const SettingsModel &settings);

// Stato attuale della rete, letto al momento (per l'indicatore a schermo)
enum class NETWORK_STATE
{
  AP,                // access point attivo
  WIFI_CONNECTED,    // collegato alla rete WiFi
  WIFI_DISCONNECTED, // in WIFI_MODE ma la connessione è caduta
  OFF
};

NETWORK_STATE wifiCurrentState(const WifiStatus &status);

#endif // WIFI_CONNECTION_H
