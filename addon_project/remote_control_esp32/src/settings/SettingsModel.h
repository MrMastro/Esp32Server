#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Impostazioni generali, lette da /config/main_settings.json sulla microSD.
// Stessi nomi dei campi di SettingsModel del progetto principale (solo quelli utili al telecomando),
// più "mode" per scegliere la funzione del dispositivo.
class SettingsModel
{
public:
  String mode;              // "remote_control" (vedi DeviceMode.h)
  String deviceName;        // hostname in rete (e nome per l'OTA)
  String devicePassword;    // password per l'OTA
  String communicationMode; // "AP_MODE" | "WIFI_MODE" (vedi CommunicationMode.h)
  bool debug;
  String ssidAP;
  String passwordAP; // almeno 8 caratteri, altrimenti l'AP non parte
  String ssidWIFI;
  String passwordWIFI;

  // Come SettingsModel::getDefault() del progetto principale
  static SettingsModel getDefault();

  // Applica i campi presenti nel JSON; quelli mancanti restano invariati.
  void fromJson(const JsonDocument &doc);

  // Carica il file dalla SD. Se la SD manca, il file non esiste o il JSON
  // non è valido, ritorna le impostazioni di default. 'loadedFromFile' dice quale dei due.
  static SettingsModel load(const char *path, bool sdAvailable, bool *loadedFromFile = nullptr);

  // Stampa le impostazioni sulla seriale (password nascoste).
  void print() const;
};

#endif // SETTINGS_MODEL_H
