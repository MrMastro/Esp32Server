#ifndef ESP_DEVICES_H
#define ESP_DEVICES_H

#include <Arduino.h>
#include <map>
#include <vector>

// Dispositivi conosciuti (ESP32 del progetto principale), letti da /config/espDevices.json:
// Al massimo ESP_DEVICES_MAX dispositivi (quelli in più vengono ignorati).
// [
//   { "deviceName": "Esp32Server", "ip": "172.17.252.192", "macAdress": "14:33:5C:02:7B:A0", "color": "#52A3BA",
//     "devicePassword": "..." },
//   ...
// ]
// La lista può essere vuota ([]) o il file assente: in quel caso non ci sono dispositivi.
// color: colore del pallino a schermo, "#RRGGBB" (se manca o non è valido: bianco)
// devicePassword: devicePassword del settings.json di quel dispositivo, per l'accoppiamento del
//                 joystick wireless (se manca il dispositivo viene contattato ma non accoppiato)
#define ESP_DEVICES_MAX 10

struct EspDevice
{
  String deviceName;
  String ip;
  String macAdress;
  String color;
  uint16_t color565 = 0xFFFF; // color convertito per il display (RGB565)
  String devicePassword;
};

// Stato di un dispositivo e pallino a schermo
enum class ESP_DEVICE_STATUS
{
  OFFLINE,   // non contattabile        -> pallino vuoto
  ONLINE,    // risponde all'accoppiamento HTTP -> pallino mezzo pieno
  CONNECTED  // collegato (HEARTBEAT_ACK) -> pallino pieno
  // disconnesso con la pressione lunga di SW: OFFLINE (pallino vuoto), motivo in pairError
};

String espDeviceStatusToString(ESP_DEVICE_STATUS status);

// Stato di un dispositivo, aggiornato dalla scansione (accoppiamento) in main.cpp
struct EspDeviceState
{
  String deviceName;
  String ip;
  String macAdress;
  String color;
  uint16_t color565 = 0xFFFF;
  ESP_DEVICE_STATUS status = ESP_DEVICE_STATUS::OFFLINE; // ONLINE se ha risposto all'accoppiamento
  String error = "";          // motivo per cui è offline (vuoto se online)
  uint32_t lastCheckMs = 0;   // millis() dell'ultimo controllo (0 = mai controllato)
  bool paired = false;        // accoppiato per il joystick wireless (chiave di sessione calcolata)
  String pairError = "";      // motivo per cui non è accoppiato (vuoto se accoppiato o mai tentato)
};

// Carica la lista dalla SD e inizializza lo stato (tutti offline, mai controllati).
// Non stampa nulla: l'esito va in info.
bool espDevicesLoad(const char *path, bool sdAvailable, String *info = nullptr);

const std::vector<EspDevice> &espDevicesList();

// Stato dei dispositivi, con chiave il macAdress (maiuscolo)
const std::map<String, EspDeviceState> &espDevicesState();

// Aggiorna lo stato condiviso di un dispositivo (chiamare DENTRO takeExclusiveExecution()).
void espDevicesSetState(const EspDeviceState &state);

// POST http://<ip><path> con body JSON (bloccante, fino a 7s). false + error se non c'e' risposta HTTP.
bool espDevicesHttpPost(const String &ip, const char *path, const String &jsonBody, int &statusCode, String &responseBody, String &error);

// Diagnosi (comando seriale hello): GET http://<ip><path> verso un IP qualsiasi, restituisce l'esito come testo.
// getInfoHello non si usa piu' nel funzionamento: il primo contatto e' l'accoppiamento.
String espDevicesProbe(const String &ip, const char *path = "/api/v1/getInfoHello");

// Contatore che cambia a ogni variazione di stato: chi disegna lo confronta per sapere se ridisegnare
uint32_t espDevicesStateVersion();

// Stato in JSON (per la seriale)
String espDevicesStateToJson();

#endif // ESP_DEVICES_H
