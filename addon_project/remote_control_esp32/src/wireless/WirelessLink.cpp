#include "WirelessLink.h"
#include "WirelessPairing.h"
#include <WiFi.h>
#include <esp_now.h>
#include <esp_idf_version.h>

// Finestra di ritrasmissione di SESSION_START: un dispositivo in modem sleep si sveglia a ogni beacon
// del router (~100 ms, fino a ~300 ms con DTIM 3)
static const uint32_t SESSION_START_WINDOW_MS = 600;

static bool ready = false;
static QueueHandle_t rxQueue = nullptr;
static QueueHandle_t txQueue = nullptr;
static volatile uint32_t sentCount = 0;      // esp_now_send accettato
static volatile uint32_t deliveredCount = 0; // conferma radio (ESP_NOW_SEND_SUCCESS)
static volatile uint32_t failedCount = 0;    // nessuna conferma radio
static uint32_t sendErrorCount = 0;          // esp_now_send rifiutato
static uint32_t expiredCount = 0;            // invio affidabile mai confermato entro la finestra

static uint8_t currentDirection = WIRELESS_DIRECTION_NONE; // ultima direzione del joystick (anche senza dispositivi)
static uint32_t lastActivityMs = 0;
static bool sessionActive = false;
static bool disconnected = false;

// Callback ESP-NOW: girano nel task WiFi, quindi solo contatori e code
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 5, 0)
static void onEspNowSend(const esp_now_send_info_t *txInfo, esp_now_send_status_t status)
{
  const uint8_t *mac = txInfo->des_addr;
#else
static void onEspNowSend(const uint8_t *mac, esp_now_send_status_t status)
{
#endif
  bool ok = status == ESP_NOW_SEND_SUCCESS;
  if (ok)
    deliveredCount = deliveredCount + 1;
  else
    failedCount = failedCount + 1;
  if (mac != nullptr && txQueue != nullptr)
  {
    WirelessTxResult result;
    memcpy(result.mac, mac, WIRELESS_MAC_LEN);
    result.delivered = ok;
    xQueueSend(txQueue, &result, 0);
  }
}

static void onEspNowRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len)
{
  if (len != (int)sizeof(WirelessPacket))
    return;
  WirelessRx rx;
  memcpy(rx.mac, info->src_addr, WIRELESS_MAC_LEN);
  memcpy(&rx.packet, data, sizeof(WirelessPacket));
  xQueueSend(rxQueue, &rx, 0);
}

bool wirelessLinkBegin(String &error)
{
  if (ready)
    return true;
  if (WiFi.getMode() == WIFI_MODE_NULL)
  {
    error = "WiFi spento: ESP-NOW richiede il WiFi attivo";
    return false;
  }
  if (rxQueue == nullptr)
    rxQueue = xQueueCreate(16, sizeof(WirelessRx));
  if (txQueue == nullptr)
    txQueue = xQueueCreate(32, sizeof(WirelessTxResult));
  esp_err_t err = esp_now_init();
  if (err != ESP_OK)
  {
    error = String("esp_now_init: ") + esp_err_to_name(err);
    return false;
  }
  esp_now_register_send_cb(onEspNowSend);
  esp_now_register_recv_cb(onEspNowRecv);
  ready = true;
  return true;
}

void wirelessLinkAddPeer(const uint8_t mac[WIRELESS_MAC_LEN])
{
  if (!ready || esp_now_is_peer_exist(mac))
    return;
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, mac, WIRELESS_MAC_LEN);
  peer.channel = 0; // 0 = canale attuale (quello del router)
  peer.ifidx = (WiFi.getMode() & WIFI_MODE_STA) ? WIFI_IF_STA : WIFI_IF_AP;
  peer.encrypt = false; // firma HMAC applicativa (vedi WIRELESS_JOYSTICK.md)
  esp_err_t err = esp_now_add_peer(&peer);
  if (err != ESP_OK)
    Serial.println("[Wireless] esp_now_add_peer " + wirelessMacToString(mac) + ": " + esp_err_to_name(err));
}

void wirelessLinkRemovePeer(const uint8_t mac[WIRELESS_MAC_LEN])
{
  if (ready && esp_now_is_peer_exist(mac))
    esp_now_del_peer(mac);
}

static bool sendPacket(WirelessSession &s, uint8_t type, uint8_t direction)
{
  WirelessPacket packet;
  if (!wirelessBuildPacket(s.sessionKey, type, ++s.txSeq, direction, packet))
    return false;
  esp_err_t err = esp_now_send(s.deviceMac, (const uint8_t *)&packet, sizeof(packet));
  if (err == ESP_OK)
  {
    sentCount = sentCount + 1;
    return true;
  }
  if (sendErrorCount++ == 0)
    Serial.println("[Wireless] esp_now_send verso " + s.deviceName + ": " + esp_err_to_name(err));
  return false;
}

void wirelessSendDirection(J_DIRECTION direction)
{
  uint8_t code = wirelessDirectionFromString(mapJdirectionsToString(direction));
  if (code == WIRELESS_DIRECTION_NONE)
    return; // NONE: niente da inviare
  currentDirection = code;
  if (!ready || disconnected)
    return; // disconnesso: la direzione si mostra a schermo ma non va a nessuno
  lastActivityMs = millis();

  if (!sessionActive)
  {
    // primo movimento dopo una pausa: SESSION_START porta gia' la direzione (applicata appena il dispositivo si sveglia)
    sessionActive = true;
    for (auto &entry : wirelessSessionsMutable())
      wirelessLinkSendReliable(entry.second, WIRELESS_PKT_SESSION_START, SESSION_START_WINDOW_MS);
    return;
  }

  for (auto &entry : wirelessSessionsMutable())
  {
    WirelessSession &s = entry.second;
    if (s.pendingType == WIRELESS_PKT_SESSION_START)
      continue; // le ritrasmissioni di SESSION_START portano gia' la direzione attuale
    sendPacket(s, WIRELESS_PKT_DIRECTION, code);
  }
}

void wirelessLinkSetDisconnected(bool value)
{
  disconnected = value;
}

bool wirelessLinkSessionActive()
{
  return sessionActive;
}

void wirelessLinkSetSessionActive(bool active)
{
  sessionActive = active;
}

void wirelessLinkSendReliable(WirelessSession &session, uint8_t type, uint32_t windowMs)
{
  session.pendingType = type;
  session.pendingDeadlineMs = millis() + windowMs;
  session.pendingInFlight = false;
  wirelessLinkResendPending(session);
}

bool wirelessLinkResendPending(WirelessSession &session)
{
  if (!ready || session.pendingType == 0)
    return false;
  session.pendingInFlight = sendPacket(session, session.pendingType, currentDirection);
  session.pendingSentMs = millis();
  return session.pendingInFlight;
}

uint32_t wirelessLinkLastActivityMs()
{
  return lastActivityMs;
}

bool wirelessLinkReceive(WirelessRx &out, uint32_t waitMs)
{
  return rxQueue != nullptr && xQueueReceive(rxQueue, &out, pdMS_TO_TICKS(waitMs)) == pdTRUE;
}

bool wirelessLinkReceiveTxResult(WirelessTxResult &out)
{
  return txQueue != nullptr && xQueueReceive(txQueue, &out, 0) == pdTRUE;
}

void wirelessLinkCountExpired()
{
  expiredCount++;
}

String wirelessLinkStats()
{
  return "ESP-NOW: inviati " + String(sentCount) + ", confermati dalla radio " + String(deliveredCount) +
         ", non confermati " + String(failedCount) + ", errori di invio " + String(sendErrorCount) +
         ", invii affidabili scaduti " + String(expiredCount);
}
