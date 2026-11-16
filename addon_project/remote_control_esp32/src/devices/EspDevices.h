#ifndef ESP_DEVICES_H
#define ESP_DEVICES_H

#include <Arduino.h>
#include <map>
#include <vector>

// Dispositivi conosciuti (ESP32 del progetto principale), letti da /config/espDevices.json:
// Al massimo ESP_DEVICES_MAX dispositivi (quelli in più vengono ignorati).
// [
//   { "deviceName": "Esp32Server", "ip": "172.17.252.192", "macAdress": "14:33:5C:02:7B:A0", "color": "#52A3BA" },
//   ...
// ]
// La lista può essere vuota ([]) o il file assente: in quel caso non ci sono dispositivi.
// color: colore del pallino a schermo, "#RRGGBB" (se manca o non è valido: bianco)
#define ESP_DEVICES_MAX 10

struct EspDevice
{
  String deviceName;
  String ip;
  String macAdress;
  String color;
  uint16_t color565 = 0xFFFF; // color convertito per il display (RGB565)
};

// Stato di un dispositivo e pallino a schermo
enum class ESP_DEVICE_STATUS
{
  OFFLINE,   // non contattabile        -> pallino vuoto
  ONLINE,    // risponde a getInfoHello -> pallino mezzo pieno
  CONNECTED  // collegato (da fare)     -> pallino pieno
};

String espDeviceStatusToString(ESP_DEVICE_STATUS status);

// Stato di un dispositivo, aggiornato da espDevicesScan()
struct EspDeviceState
{
  String deviceName;
  String ip;
  String macAdress;
  String color;
  uint16_t color565 = 0xFFFF;
  ESP_DEVICE_STATUS status = ESP_DEVICE_STATUS::OFFLINE; // ONLINE se ha risposto a getInfoHello con lo stesso macAdress
  String error = "";          // motivo per cui è offline (vuoto se online)
  uint32_t lastCheckMs = 0;   // millis() dell'ultimo controllo (0 = mai controllato)
};

// Carica la lista dalla SD e inizializza lo stato (tutti offline, mai controllati).
// Non stampa nulla: l'esito va in info.
bool espDevicesLoad(const char *path, bool sdAvailable, String *info = nullptr);

const std::vector<EspDevice> &espDevicesList();

// Stato dei dispositivi, con chiave il macAdress (maiuscolo)
const std::map<String, EspDeviceState> &espDevicesState();

// Contatta un dispositivo (GET http://<ip>/api/v1/getInfoHello) e restituisce il nuovo stato.
// Solo rete, non tocca lo stato condiviso: si può chiamare FUORI dal lock.
// attempts: tentativi se resta offline (il primo contatto dopo l'avvio spesso fallisce).
// Bloccante: fino a 7s per tentativo.
EspDeviceState espDevicesCheck(const EspDevice &device, uint8_t attempts = 1);

// Aggiorna lo stato condiviso di un dispositivo (chiamare DENTRO takeExclusiveExecution()).
void espDevicesSetState(const EspDeviceState &state);

// Controlla tutti i dispositivi e aggiorna lo stato (tutto in un colpo, bloccante).
void espDevicesScan(uint8_t attempts = 1);

// Diagnosi: GET http://<ip><path> verso un IP qualsiasi, restituisce l'esito come testo
String espDevicesProbe(const String &ip, const char *path = "/api/v1/getInfoHello");

// Contatore che cambia a ogni variazione di stato: chi disegna lo confronta per sapere se ridisegnare
uint32_t espDevicesStateVersion();

// Stato in JSON (per la seriale)
String espDevicesStateToJson();

#endif // ESP_DEVICES_H
