#ifndef SETTINGS_VALIDATOR_H
#define SETTINGS_VALIDATOR_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include "SettingsModel.h"

// Esito della validazione di una richiesta di modifica impostazioni
struct SettingsValidation
{
  bool ok = false;
  int httpCode = 400; // codice HTTP da restituire se !ok
  String error;       // messaggio di errore se !ok
  String key;         // chiave da modificare se ok
};

// Valida il corpo della POST /api/settings:
//  - deviceName e devicePassword devono coincidere con quelli attuali (autenticazione)
//  - deve esserci UNA sola altra chiave, tra quelle modificabili di main_settings.json
//  - il valore deve avere tipo e formato corretti per quella chiave
SettingsValidation validateSettingsRequest(const JsonDocument &request, const SettingsModel &current);

#endif // SETTINGS_VALIDATOR_H
