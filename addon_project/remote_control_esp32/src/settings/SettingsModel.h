#ifndef SETTINGS_MODEL_H
#define SETTINGS_MODEL_H

#include <Arduino.h>
#include <ArduinoJson.h>

// Impostazioni generali, lette da /config/main_settings.json sulla microSD.
// Stessi nomi dei campi di SettingsModel del progetto principale (solo quelli utili al telecomando),
// più "mode" per scegliere la funzione del dispositivo e "ota_upload" per l'OTA.
class SettingsModel
{
public:
  String mode;              // "remote_control" (vedi DeviceMode.h)
  String deviceName;        // hostname in rete (e nome per l'OTA)
  String devicePassword;    // password per l'OTA e per l'API delle impostazioni
  String communicationMode; // "AP_MODE" | "WIFI_MODE" (vedi CommunicationMode.h)
  bool debug;
  String ssidAP;
  String passwordAP; // almeno 8 caratteri, altrimenti l'AP non parte
  String ssidWIFI;
  String passwordWIFI;
  bool otaUpload; // JSON: "ota_upload". true = OTA via WiFi (upload da PlatformIO) attivo

  // Come SettingsModel::getDefault() del progetto principale
  static SettingsModel getDefault();

  // Applica i campi presenti nel JSON; quelli mancanti restano invariati.
  void fromJson(const JsonDocument &doc);

  // Scrive tutti i campi nel JSON.
  void toJson(JsonDocument &doc) const;

  // Carica il file dalla SD. Se la SD manca, il file non esiste o il JSON
  // non è valido, ritorna le impostazioni di default. 'loadedFromFile' dice quale dei due.
  // Non stampa nulla: la descrizione dell'origine finisce in 'loadInfo'.
  static SettingsModel load(const char *path, bool sdAvailable, bool *loadedFromFile = nullptr, String *loadInfo = nullptr);

  // JSON formattato delle impostazioni (per il log di avvio). Password mascherate se hidePasswords.
  String toJsonString(bool hidePasswords = true) const;

  // Modifica una sola chiave nel file sulla SD, lasciando invariato il resto.
  // Se il file non esiste (o non è valido) lo crea partendo dalle impostazioni correnti.
  bool saveKey(const char *path, const String &key, JsonVariantConst value) const;
};

#endif // SETTINGS_MODEL_H
