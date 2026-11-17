#ifndef WIRELESS_LINK_H
#define WIRELESS_LINK_H

#include <Arduino.h>
#include "joystick/JoystickDirections.h"
#include "WirelessProtocol.h"

struct WirelessSession;

// ESP-NOW del joystick wireless (docs/WIRELESS_JOYSTICK.md): invio dei pacchetti firmati con la chiave
// di sessione di ogni dispositivo accoppiato (WirelessPairing) e ricezione delle risposte.
// Le funzioni che toccano le sessioni vanno chiamate DENTRO takeExclusiveExecution().

// Pacchetto ricevuto da un dispositivo (HEARTBEAT_ACK, UNKNOWN_SESSION, ...): solo copiato, non verificato
struct WirelessRx
{
  uint8_t mac[WIRELESS_MAC_LEN];
  WirelessPacket packet;
};

// Esito radio di un invio (conferma di ricezione a livello radio, non accettazione del pacchetto)
struct WirelessTxResult
{
  uint8_t mac[WIRELESS_MAC_LEN];
  bool delivered;
};

// Avvia ESP-NOW (dopo il WiFi). Idempotente. false (con error) se non riesce.
bool wirelessLinkBegin(String &error);

// Registra / rimuove il dispositivo come peer ESP-NOW.
void wirelessLinkAddPeer(const uint8_t mac[WIRELESS_MAC_LEN]);
void wirelessLinkRemovePeer(const uint8_t mac[WIRELESS_MAC_LEN]);

// Nuova direzione del joystick (al cambio). Se non c'e' una sessione la apre: SESSION_START affidabile a
// tutti (i dispositivi possono essere in modem sleep); altrimenti DIRECTION a tutti. Non bloccante.
void wirelessSendDirection(J_DIRECTION direction);

// Sessione (fase 4): aperta dal primo movimento, chiusa da WirelessManager dopo sessionTimeoutS
bool wirelessLinkSessionActive();
void wirelessLinkSetSessionActive(bool active);

// Accoda un invio affidabile (ritrasmesso finche' la radio conferma o fino a windowMs) e lo invia subito.
// Il pacchetto porta sempre la direzione attuale (anche nelle ritrasmissioni).
void wirelessLinkSendReliable(WirelessSession &session, uint8_t type, uint32_t windowMs);

// Ritrasmette il pacchetto in attesa (chiamata da WirelessManager). false se l'invio e' rifiutato.
bool wirelessLinkResendPending(WirelessSession &session);

// millis() dell'ultimo cambio di direzione (0 = mai): per l'heartbeat adattivo e la fine sessione
uint32_t wirelessLinkLastActivityMs();

// Code dalle callback ESP-NOW (attesa fino a waitMs). false se vuote.
bool wirelessLinkReceive(WirelessRx &out, uint32_t waitMs);
bool wirelessLinkReceiveTxResult(WirelessTxResult &out);

// Statistiche di invio (dall'avvio)
String wirelessLinkStats();
void wirelessLinkCountExpired(); // invio affidabile mai confermato dalla radio entro la finestra

#endif // WIRELESS_LINK_H
