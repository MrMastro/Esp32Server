#ifndef WIRELESS_PAIRING_H
#define WIRELESS_PAIRING_H

#include <Arduino.h>
#include <map>
#include "WirelessProtocol.h"
#include "devices/EspDevices.h"

// Fase 1 del joystick wireless (docs/WIRELESS_JOYSTICK.md): accoppiamento HTTP con un dispositivo.
// Il telecomando invia un nonce casuale con deviceName e devicePassword del dispositivo (da espDevices.json:
// deviceName deve essere uguale a quello del settings.json del dispositivo); il dispositivo risponde con
// il suo nonce. Entrambi calcolano la stessa chiave di sessione (wirelessDeriveSessionKey), che firmera'
// i pacchetti ESP-NOW (fase 2). La password non viaggia piu' dopo l'accoppiamento.
// E' anche il primo contatto con il dispositivo: getInfoHello non serve.

struct WirelessSession
{
  String deviceName;
  uint8_t deviceMac[WIRELESS_MAC_LEN] = {0};
  uint8_t sessionKey[WIRELESS_SESSION_KEY_LEN] = {0};
  uint8_t channel = 0;   // canale WiFi riportato dal dispositivo
  uint32_t pairedMs = 0; // millis() dell'accoppiamento
  uint32_t txSeq = 0;    // ultimo seq inviato (riparte da 0 a ogni accoppiamento: il primo pacchetto ha seq 1)

  // Heartbeat (fase 3), gestiti da WirelessManager
  uint32_t rxSeq = 0;           // ultimo seq accettato dal dispositivo (anti-replay delle risposte)
  uint32_t lastHeartbeatMs = 0; // 0 = mai inviato: il primo parte subito dopo l'accoppiamento
  uint32_t lastAckMs = 0;
  bool awaitingAck = false;     // ultimo heartbeat ancora senza HEARTBEAT_ACK
  uint8_t missed = 0;           // heartbeat consecutivi senza ACK
  bool connected = false;       // ultimo heartbeat confermato (pallino pieno)
  bool lost = false;            // troppi heartbeat persi o sessione sconosciuta: in attesa di riaccoppiamento

  // Invio affidabile (fase 4): il pacchetto viene ritrasmesso finche' la radio del dispositivo non conferma
  // la ricezione (il dispositivo in modem sleep riceve solo quando si sveglia) o finche' scade la finestra.
  uint8_t pendingType = 0;       // WIRELESS_PKT_* in attesa di conferma radio, 0 = nessuno
  uint32_t pendingDeadlineMs = 0;
  bool pendingInFlight = false;  // inviato, esito radio non ancora arrivato
  uint32_t pendingSentMs = 0;

  bool disconnectAcked = false;  // DISCONNECT confermato (fase 5)
};

enum class PAIR_RESULT
{
  PAIRED,        // 200: chiave di sessione calcolata
  REJECTED,      // il dispositivo ha risposto ma non ha accoppiato (401, 403, 409, MAC diverso, ...)
  UNREACHABLE,   // nessuna risposta HTTP (spento, IP sbagliato, rete)
  NOT_CONFIGURED // devicePassword mancante in espDevices.json: dispositivo non contattato
};

// Accoppia il dispositivo (POST /api/v1/remote/pair). Solo rete: si chiama FUORI dal lock.
// Bloccante: fino a 7s. error descrive l'esito se diverso da PAIRED.
PAIR_RESULT wirelessPair(const EspDevice &device, WirelessSession &session, String &error);

// Sessioni attive, con chiave il MAC del dispositivo (maiuscolo). Da usare DENTRO takeExclusiveExecution().
// wirelessSessionSet registra anche il dispositivo come peer ESP-NOW.
void wirelessSessionSet(const WirelessSession &session);
const std::map<String, WirelessSession> &wirelessSessions();
std::map<String, WirelessSession> &wirelessSessionsMutable(); // per l'invio (aggiorna txSeq e heartbeat)
// Elimina la sessione (e il peer ESP-NOW) del dispositivo con questo MAC, se c'e'
void wirelessSessionRemove(const String &deviceMac);

#endif // WIRELESS_PAIRING_H
