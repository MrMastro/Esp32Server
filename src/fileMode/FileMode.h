#ifndef FILE_MODE_H
#define FILE_MODE_H

#include <Arduino.h>

// Modalità file: la memoria flash (LittleFS: /settings/settings.json, preset, /www ...) diventa
// un'unità di rete WebDAV per Esplora risorse di Windows, così i file si modificano senza
// "Upload Filesystem Image". Stessa idea della modalità SD del telecomando (remote_control_esp32).
//
// - si chiede con i comandi "microsd" o "fileremote" (seriale): vale per UN solo avvio
//   (flag in NVS letto e cancellato all'avvio), quindi riavvio o corrente staccata = modalità normale;
// - in modalità file girano solo WiFi, WebDAV (porta 80, mDNS http://<deviceName>.local/) e seriale:
//   niente AsyncWebServer, LED, OTA, joystick, wireless;
// - il LED di stato resta acceso fisso.

// Chiede la modalità file per il prossimo avvio.
void fileModeRequest();

// true se era stata chiesta la modalità file; cancella comunque la richiesta (va chiamata all'avvio).
bool fileModeConsumeRequest();

// Avvia la modalità file: WiFi (come communicationMode, ripiego in AP), LittleFS, mDNS, WebDAV + task.
void fileModeRun(const String &communicationMode, const String &ssidWIFI, const String &passwordWIFI,
                 const String &ssidAP, const String &passwordAP, const String &deviceName);

bool fileModeActive();
String fileModeIp();
// Indirizzo da usare in "Connetti unità di rete" (http://<nome>.local/ oppure http://<ip>/)
String fileModeAddress();

#endif // FILE_MODE_H
