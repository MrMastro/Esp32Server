#include "WirelessReceiver.h"
#include "WirelessProtocol.h"
#include "RemotePairing.h"
#include <WiFi.h>
#include <esp_now.h>

struct RxItem
{
  uint8_t mac[WIRELESS_MAC_LEN];
  WirelessPacket packet;
};

static const uint32_t REJECT_LOG_INTERVAL_MS = 1000;       // al massimo un log di scarto al secondo
static const uint32_t UNKNOWN_SESSION_INTERVAL_MS = 1000;  // al massimo un UNKNOWN_SESSION al secondo

static QueueHandle_t rxQueue = nullptr;
static WirelessDirectionHandler directionHandler = nullptr;
static bool ready = false;

// Stato legato all'accoppiamento corrente: si azzera quando cambia generation
static uint32_t currentGeneration = 0;
static uint32_t lastSeq = 0;  // ultimo seq accettato dal telecomando
static uint32_t txSeq = 0;    // ultimo seq inviato al telecomando (HEARTBEAT_ACK)
static uint8_t lastDirection = WIRELESS_DIRECTION_NONE;

// Sessione (fase 4): radio sempre accesa durante l'uso, modem sleep a riposo
static bool inSession = false;
static uint32_t lastSessionPacketMs = 0; // ultimo SESSION_START / DIRECTION valido
static uint32_t sessionTimeoutMs = 30000;

static uint32_t rejectedCount = 0;
static uint32_t lastRejectLogMs = 0;
static uint32_t lastUnknownSessionMs = 0;

// Callback ESP-NOW (core 2.x / ESP-IDF 4.4): gira nel task WiFi, quindi solo copia in coda
static void onEspNowRecv(const uint8_t *mac, const uint8_t *data, int len)
{
  if (len != (int)sizeof(WirelessPacket))
    return;
  RxItem item;
  memcpy(item.mac, mac, WIRELESS_MAC_LEN);
  memcpy(&item.packet, data, sizeof(WirelessPacket));
  xQueueSend(rxQueue, &item, 0);
}

static void logReject(const String &reason, const uint8_t *mac)
{
  rejectedCount++;
  uint32_t now = millis();
  if (now - lastRejectLogMs >= REJECT_LOG_INTERVAL_MS)
  {
    lastRejectLogMs = now;
    Serial.println("[Wireless] pacchetto scartato da " + wirelessMacToString(mac) + ": " + reason +
                   " (scartati in totale: " + String(rejectedCount) + ")");
  }
}

static bool ensurePeer(const uint8_t *mac)
{
  if (esp_now_is_peer_exist(mac))
    return true;
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, mac, WIRELESS_MAC_LEN);
  peer.channel = 0; // 0 = canale attuale
  peer.ifidx = WIFI_IF_STA;
  peer.encrypt = false;
  esp_err_t err = esp_now_add_peer(&peer);
  if (err != ESP_OK)
  {
    Serial.printf("[Wireless] esp_now_add_peer %s: %s\n", wirelessMacToString(mac).c_str(), esp_err_to_name(err));
    return false;
  }
  return true;
}

// Pacchetti che solo il telecomando invia: a questi si risponde UNKNOWN_SESSION se non lo si riconosce
static bool isRemotePacketType(uint8_t type)
{
  return type == WIRELESS_PKT_SESSION_START || type == WIRELESS_PKT_DIRECTION || type == WIRELESS_PKT_HEARTBEAT ||
         type == WIRELESS_PKT_SESSION_END || type == WIRELESS_PKT_REMOTE_PAUSE;
}

// Rifiuto con risposta UNKNOWN_SESSION (non firmata: non si ha la chiave giusta), cosi' il telecomando
// si riaccoppia subito (es. dopo un riavvio del dispositivo). Limitata a una al secondo.
static void rejectUnknownSession(const String &reason, const RxItem &item)
{
  logReject(reason, item.mac);
  if (!isRemotePacketType(item.packet.type))
    return;
  uint32_t now = millis();
  if (lastUnknownSessionMs != 0 && now - lastUnknownSessionMs < UNKNOWN_SESSION_INTERVAL_MS)
    return;
  lastUnknownSessionMs = now;
  if (!ensurePeer(item.mac))
    return;
  WirelessPacket reply = {};
  reply.version = WIRELESS_PROTOCOL_VERSION;
  reply.type = WIRELESS_PKT_UNKNOWN_SESSION;
  reply.seq = 0;
  reply.direction = WIRELESS_DIRECTION_NONE;
  esp_now_send(item.mac, (const uint8_t *)&reply, sizeof(reply));
}

static void startSession()
{
  lastSessionPacketMs = millis();
  if (inSession)
    return;
  inSession = true;
  WiFi.setSleep(false); // radio sempre accesa: le direzioni arrivano in pochi ms
  Serial.println("[Wireless] inizio sessione: radio sempre accesa");
}

static void endSession(const String &reason)
{
  if (!inSession)
    return;
  inSession = false;
  WiFi.setSleep(true); // modem sleep: la radio si sveglia a ogni beacon del router
  Serial.println("[Wireless] fine sessione (" + reason + "): modem sleep");
}

// Applica la direzione solo se cambia (stato, non eventi: un heartbeat con la stessa direzione non fa nulla)
static void applyDirection(uint8_t code, const uint8_t *mac)
{
  if (code == WIRELESS_DIRECTION_NONE || code == lastDirection)
    return;
  String name = wirelessDirectionToString(code);
  if (name.length() == 0)
  {
    logReject("codice direzione " + String(code) + " sconosciuto", mac);
    return;
  }
  lastDirection = code;
  if (directionHandler != nullptr)
    directionHandler(name);
}

static void sendHeartbeatAck(const RemotePairing &pairing)
{
  if (!ensurePeer(pairing.remoteMac))
    return;
  WirelessPacket ack;
  if (!wirelessBuildPacket(pairing.sessionKey, WIRELESS_PKT_HEARTBEAT_ACK, ++txSeq, lastDirection, ack))
    return;
  esp_now_send(pairing.remoteMac, (const uint8_t *)&ack, sizeof(ack));
}

bool wirelessReceiverBegin(WirelessDirectionHandler onDirection, uint32_t sessionTimeoutS)
{
  if (ready)
    return true;
  directionHandler = onDirection;
  sessionTimeoutMs = sessionTimeoutS * 1000;

  // A riposo modem sleep: i pacchetti ESP-NOW che arrivano mentre la radio dorme vanno persi, per questo il
  // telecomando ritrasmette SESSION_START e gli heartbeat finche' la radio non conferma la ricezione.
  WiFi.setSleep(true);

  rxQueue = xQueueCreate(16, sizeof(RxItem));
  esp_err_t err = esp_now_init();
  if (err != ESP_OK)
  {
    Serial.printf("[Wireless] esp_now_init fallito: %s\n", esp_err_to_name(err));
    return false;
  }
  esp_now_register_recv_cb(onEspNowRecv);
  Serial.println("[Wireless] ricevitore ESP-NOW attivo: MAC " + WiFi.macAddress() + ", canale " + String(WiFi.channel()) +
                 ", modem sleep a riposo, timeout sessione " + String(sessionTimeoutS) + " s");
  ready = true;
  return true;
}

void wirelessReceiverProcess(uint32_t waitMs)
{
  if (!ready)
  {
    vTaskDelay(pdMS_TO_TICKS(waitMs));
    return;
  }

  // timeout di sicurezza: se il telecomando sparisce (spento, fuori portata) non si resta con la radio accesa
  if (inSession && millis() - lastSessionPacketMs >= sessionTimeoutMs)
    endSession("nessun movimento per " + String(sessionTimeoutMs / 1000) + " s");

  RxItem item;
  if (xQueueReceive(rxQueue, &item, pdMS_TO_TICKS(waitMs)) != pdTRUE)
    return;

  RemotePairing pairing;
  if (!remotePairingGet(pairing))
  {
    rejectUnknownSession("nessun telecomando accoppiato", item);
    return;
  }
  if (pairing.generation != currentGeneration)
  {
    // nuovo accoppiamento: seq e direzione ripartono da zero
    currentGeneration = pairing.generation;
    lastSeq = 0;
    txSeq = 0;
    lastDirection = WIRELESS_DIRECTION_NONE;
  }

  const WirelessPacket &p = item.packet;
  if (memcmp(item.mac, pairing.remoteMac, WIRELESS_MAC_LEN) != 0)
  {
    rejectUnknownSession("mittente non accoppiato", item);
    return;
  }
  if (p.version != WIRELESS_PROTOCOL_VERSION)
  {
    rejectUnknownSession("versione " + String(p.version), item);
    return;
  }
  if (!wirelessVerifyPacket(pairing.sessionKey, p))
  {
    // es. il telecomando usa ancora la chiave di un accoppiamento precedente
    rejectUnknownSession("firma non valida", item);
    return;
  }
  if (p.seq <= lastSeq)
  {
    // pacchetto vecchio o ripetuto: si scarta senza rispondere
    logReject("seq " + String(p.seq) + " <= " + String(lastSeq) + " (vecchio o ripetuto)", item.mac);
    return;
  }
  lastSeq = p.seq;

  switch (p.type)
  {
  case WIRELESS_PKT_SESSION_START:
    // primo movimento dopo una pausa (e, durante l'uso, l'heartbeat del telecomando): radio accesa,
    // direzione applicata subito, timer di sicurezza rinnovato
    startSession();
    applyDirection(p.direction, item.mac);
    sendHeartbeatAck(pairing);
    break;
  case WIRELESS_PKT_DIRECTION:
    startSession(); // se SESSION_START fosse andato perso
    applyDirection(p.direction, item.mac);
    break;
  case WIRELESS_PKT_HEARTBEAT:
    // heartbeat a riposo: porta la direzione attuale (corregge un DIRECTION perso), non apre la sessione
    applyDirection(p.direction, item.mac);
    sendHeartbeatAck(pairing);
    break;
  case WIRELESS_PKT_SESSION_END:
    endSession("SESSION_END dal telecomando");
    break;
  default:
    // altri tipi: fasi successive
    break;
  }
}
