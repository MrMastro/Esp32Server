#include "WirelessManager.h"
#include "WirelessLink.h"
#include "WirelessPairing.h"
#include "devices/EspDevices.h"
#include "system/ExclusiveExecution.h"
#include <WiFi.h>

// Priorita' basse come gli altri task del C6 (vedi main.cpp): un solo core, la seriale ha la precedenza
#define PRIORITY_WIRELESS 2
#define PRIORITY_PAIRING 1
#define STACK_SIZE_WIRELESS 4096
#define STACK_SIZE_PAIRING 6144

static const uint32_t WIRELESS_LOOP_MS = 20;

// Invio affidabile (fase 4): finestre di ritrasmissione. Un dispositivo in modem sleep si sveglia a ogni
// beacon del router (~100 ms, fino a ~300 ms con DTIM 3): si ritrasmette finche' la sua radio conferma.
static const uint32_t HEARTBEAT_WINDOW_MS = 600;
static const uint32_t SESSION_END_WINDOW_MS = 300;
static const uint32_t RESEND_AFTER_MS = 40; // nessun esito radio entro questo tempo: si ritrasmette
static const uint32_t ACK_HOLD_MS = 150;    // radio del telecomando accesa dopo un invio, per ricevere l'ACK

static WirelessConfig cfg;
static QueueHandle_t pairQueue = nullptr;          // indici in espDevicesList()
static volatile bool pairPending[ESP_DEVICES_MAX]; // gia' in coda o in corso
static volatile bool pairingInProgress = false;    // accoppiamento HTTP in corso: radio del telecomando accesa
static uint32_t lastRediscoveryMs = 0;
static uint32_t radioHoldUntilMs = 0;              // radio accesa almeno fino a qui (attesa di ACK)
static bool radioSleeping = false;                 // stato del modem sleep del telecomando applicato

// ---------------------------------------------------------------------------
// Utilita' (lo stato dei dispositivi si tocca DENTRO takeExclusiveExecution())
// ---------------------------------------------------------------------------

static int deviceIndexByMac(const String &mac)
{
  const std::vector<EspDevice> &devices = espDevicesList();
  for (size_t i = 0; i < devices.size(); i++)
  {
    if (devices[i].macAdress.equalsIgnoreCase(mac))
      return (int)i;
  }
  return -1;
}

// Aggiorna stato (pallino) e motivo di un dispositivo
static void setDeviceStatus(const String &mac, ESP_DEVICE_STATUS status, bool paired, const String &pairError)
{
  auto it = espDevicesState().find(mac);
  if (it == espDevicesState().end())
    return;
  EspDeviceState state = it->second;
  state.status = status;
  state.paired = paired;
  state.pairError = pairError;
  if (status != ESP_DEVICE_STATUS::OFFLINE)
    state.error = "";
  state.lastCheckMs = millis();
  espDevicesSetState(state);
}

// Accoda l'accoppiamento di un dispositivo (se non e' gia' in coda). Chiamabile ovunque.
static bool requestPair(int index)
{
  if (index < 0 || index >= (int)espDevicesList().size() || pairQueue == nullptr || pairPending[index])
    return false;
  pairPending[index] = true;
  uint8_t i = (uint8_t)index;
  if (xQueueSend(pairQueue, &i, 0) != pdTRUE)
  {
    pairPending[index] = false;
    return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// Task di accoppiamento (HTTP, bloccante: fuori dal lock)
// ---------------------------------------------------------------------------

// Accoppia un dispositivo e restituisce il suo nuovo stato:
// accoppiato -> ONLINE + paired (CONNECTED al primo HEARTBEAT_ACK); risposta di errore -> ONLINE + pairError;
// nessuna risposta -> OFFLINE; devicePassword mancante -> OFFLINE, non contattato.
static void pairDevice(const EspDevice &device)
{
  EspDeviceState state;
  state.deviceName = device.deviceName;
  state.ip = device.ip;
  state.macAdress = device.macAdress;
  state.color = device.color;
  state.color565 = device.color565;

  WirelessSession session;
  String error;
  PAIR_RESULT result = PAIR_RESULT::UNREACHABLE;
  for (uint8_t i = 0; i < cfg.pairAttempts; i++)
  {
    result = wirelessPair(device, session, error);
    if (result != PAIR_RESULT::UNREACHABLE)
      break; // si ritenta solo se non ha risposto
  }

  switch (result)
  {
  case PAIR_RESULT::PAIRED:
    state.status = ESP_DEVICE_STATUS::ONLINE;
    state.paired = true;
    break;
  case PAIR_RESULT::REJECTED:
    state.status = ESP_DEVICE_STATUS::ONLINE;
    state.pairError = error;
    break;
  case PAIR_RESULT::NOT_CONFIGURED:
    state.status = ESP_DEVICE_STATUS::OFFLINE;
    state.error = "non contattato";
    state.pairError = error;
    break;
  case PAIR_RESULT::UNREACHABLE:
  default:
    state.status = ESP_DEVICE_STATUS::OFFLINE;
    state.error = error;
    break;
  }
  state.lastCheckMs = millis();

  takeExclusiveExecution();
  if (result == PAIR_RESULT::PAIRED)
    wirelessSessionSet(session); // sostituisce la sessione precedente; primo heartbeat subito
  else
    wirelessSessionRemove(device.macAdress);
  espDevicesSetState(state);
  freeExclusiveExecution();

  if (result == PAIR_RESULT::PAIRED)
    Serial.println("[Wireless] " + device.deviceName + " accoppiato: canale " + String(session.channel) +
                   ", impronta chiave " + wirelessKeyFingerprint(session.sessionKey));
  else
    Serial.println("[Wireless] " + device.deviceName + " non accoppiato: " + (state.pairError.length() > 0 ? state.pairError : error));
}

static void pairingTask(void *pvParameters)
{
  Serial.println("[Task] pairingTask avviato");
  while (true)
  {
    uint8_t index;
    if (xQueueReceive(pairQueue, &index, portMAX_DELAY) != pdTRUE)
      continue;
    const std::vector<EspDevice> &devices = espDevicesList(); // la lista non cambia dopo l'avvio
    if (index < devices.size())
    {
      // l'HTTP con il modem sleep attivo su entrambi i lati falliva: radio accesa per tutto l'accoppiamento
      pairingInProgress = true;
      if (radioSleeping)
      {
        WiFi.setSleep(false);
        radioSleeping = false;
      }
      pairDevice(devices[index]);
      pairingInProgress = false;
    }
    pairPending[index] = false;
  }
}

// ---------------------------------------------------------------------------
// Task wireless: risposte dei dispositivi, heartbeat, riscoperta
// ---------------------------------------------------------------------------

// Sessione da riaccoppiare (heartbeat persi o sessione sconosciuta al dispositivo). DENTRO il lock.
// repairNow = false: niente riaccoppiamento immediato, ci pensa la riscoperta periodica
// (evita un ciclo continuo di accoppiamenti HTTP con un dispositivo che non risponde via ESP-NOW).
static void markLost(WirelessSession &s, const String &mac, const String &reason, bool repairNow)
{
  if (s.lost)
    return;
  s.lost = true;
  s.connected = false;
  setDeviceStatus(mac, ESP_DEVICE_STATUS::ONLINE, false, reason);
  if (repairNow)
  {
    Serial.println("[Wireless] " + s.deviceName + ": " + reason + ", riaccoppiamento");
    requestPair(deviceIndexByMac(mac));
  }
  else
  {
    Serial.println("[Wireless] " + s.deviceName + ": " + reason + ", riprovo alla prossima riscoperta (" +
                   String(cfg.rediscoveryS) + " s)");
  }
}

static void handleRx(const WirelessRx &rx)
{
  String mac = wirelessMacToString(rx.mac);
  takeExclusiveExecution();
  auto &sessions = wirelessSessionsMutable();
  auto it = sessions.find(mac);
  if (it == sessions.end())
  {
    freeExclusiveExecution();
    return; // dispositivo senza sessione: niente da fare
  }
  WirelessSession &s = it->second;

  switch (rx.packet.type)
  {
  case WIRELESS_PKT_UNKNOWN_SESSION:
    // non firmato (il dispositivo non ha la chiave): al massimo provoca un riaccoppiamento
    markLost(s, mac, "sessione sconosciuta al dispositivo (riavviato?)", true);
    break;

  case WIRELESS_PKT_HEARTBEAT_ACK:
    if (rx.packet.version != WIRELESS_PROTOCOL_VERSION || !wirelessVerifyPacket(s.sessionKey, rx.packet) ||
        rx.packet.seq <= s.rxSeq)
      break; // firma non valida o risposta vecchia/ripetuta: ignorata
    s.rxSeq = rx.packet.seq;
    s.awaitingAck = false;
    s.missed = 0;
    s.lastAckMs = millis();
    if (!s.connected && !s.lost)
    {
      s.connected = true;
      setDeviceStatus(mac, ESP_DEVICE_STATUS::CONNECTED, true, "");
      Serial.println("[Wireless] " + s.deviceName + " collegato (HEARTBEAT_ACK)");
    }
    break;

  default:
    break;
  }
  freeExclusiveExecution();
}

// Heartbeat: a riposo HEARTBEAT, in sessione SESSION_START (stesso effetto + risveglia un dispositivo che si
// fosse riaddormentato e ne rinnova il timer di sicurezza). Entrambi ricevono HEARTBEAT_ACK.
// Invio affidabile: ritrasmesso finche' la radio del dispositivo conferma (puo' essere in modem sleep).
static void sendHeartbeats()
{
  uint32_t now = millis();
  bool active = wirelessLinkSessionActive();
  uint32_t interval = active ? cfg.heartbeatActiveMs : (uint32_t)cfg.heartbeatIdleS * 1000;
  uint8_t type = active ? WIRELESS_PKT_SESSION_START : WIRELESS_PKT_HEARTBEAT;

  takeExclusiveExecution();
  for (auto &entry : wirelessSessionsMutable())
  {
    WirelessSession &s = entry.second;
    if (s.lost || s.pendingType != 0)
      continue; // un invio affidabile in corso (es. SESSION_START del primo movimento) vale come heartbeat
    // con il passaggio a "attivo" l'intervallo si accorcia subito (lastHeartbeatMs resta quello vecchio)
    if (s.lastHeartbeatMs != 0 && now - s.lastHeartbeatMs < interval)
      continue;
    if (s.awaitingAck)
    {
      s.missed++;
      if (s.missed >= cfg.missedHeartbeats)
      {
        // mai un ACK da questo accoppiamento: il dispositivo risponde via HTTP ma non via ESP-NOW
        // (firmware senza heartbeat, canale diverso, ...): riaccoppiarlo subito non servirebbe
        bool everAcked = s.lastAckMs != 0;
        markLost(s, entry.first,
                 everAcked ? String(s.missed) + " heartbeat senza risposta"
                           : "nessuna risposta via ESP-NOW dopo l'accoppiamento",
                 everAcked);
        continue;
      }
    }
    s.lastHeartbeatMs = now;
    s.awaitingAck = true;
    wirelessLinkSendReliable(s, type, HEARTBEAT_WINDOW_MS);
    radioHoldUntilMs = max(radioHoldUntilMs, now + HEARTBEAT_WINDOW_MS + ACK_HOLD_MS);
  }
  freeExclusiveExecution();
}

// Esiti radio degli invii: un invio affidabile confermato non va piu' ritrasmesso
static void handleTxResults()
{
  WirelessTxResult result;
  if (!wirelessLinkReceiveTxResult(result))
    return;
  takeExclusiveExecution();
  auto &sessions = wirelessSessionsMutable();
  do
  {
    auto it = sessions.find(wirelessMacToString(result.mac));
    if (it == sessions.end())
      continue;
    WirelessSession &s = it->second;
    if (s.pendingType == 0 || !s.pendingInFlight)
      continue;
    s.pendingInFlight = false;
    if (result.delivered)
      s.pendingType = 0; // consegnato: la risposta (ACK) e' gestita da handleRx
  } while (wirelessLinkReceiveTxResult(result));
  freeExclusiveExecution();
}

// Ritrasmissioni degli invii affidabili non ancora confermati, fino alla scadenza della finestra
static void processPending()
{
  uint32_t now = millis();
  takeExclusiveExecution();
  for (auto &entry : wirelessSessionsMutable())
  {
    WirelessSession &s = entry.second;
    if (s.pendingType == 0)
      continue;
    if ((int32_t)(now - s.pendingDeadlineMs) >= 0)
    {
      // mai confermato dalla radio: dispositivo spento o fuori portata (per gli heartbeat conta come perso)
      s.pendingType = 0;
      s.pendingInFlight = false;
      wirelessLinkCountExpired();
      continue;
    }
    if (!s.pendingInFlight || now - s.pendingSentMs >= RESEND_AFTER_MS)
      wirelessLinkResendPending(s);
  }
  freeExclusiveExecution();
}

// Fine sessione: sessionTimeoutS senza movimenti -> SESSION_END a tutti (i dispositivi tornano in modem sleep)
static void endSessionIfIdle()
{
  if (!wirelessLinkSessionActive())
    return;
  uint32_t now = millis();
  if (now - wirelessLinkLastActivityMs() < (uint32_t)cfg.sessionTimeoutS * 1000)
    return;
  takeExclusiveExecution();
  wirelessLinkSetSessionActive(false);
  for (auto &entry : wirelessSessionsMutable())
  {
    if (!entry.second.lost)
      wirelessLinkSendReliable(entry.second, WIRELESS_PKT_SESSION_END, SESSION_END_WINDOW_MS);
  }
  freeExclusiveExecution();
  Serial.println("[Wireless] fine sessione (" + String(cfg.sessionTimeoutS) + " s senza movimenti)");
}

// Modem sleep del telecomando: spento in sessione, durante l'accoppiamento, con invii affidabili in corso
// e mentre si attende un ACK; acceso a riposo (se remoteModemSleep = true).
static void updateRadioSleep()
{
  bool awake = !cfg.remoteModemSleep || wirelessLinkSessionActive() || pairingInProgress ||
               (int32_t)(millis() - radioHoldUntilMs) < 0;
  if (!awake)
  {
    takeExclusiveExecution();
    for (const auto &entry : wirelessSessions())
    {
      if (entry.second.pendingType != 0)
      {
        awake = true;
        break;
      }
    }
    freeExclusiveExecution();
  }
  if (awake == !radioSleeping)
    return;
  radioSleeping = !awake;
  WiFi.setSleep(radioSleeping);
}

// Ogni rediscoveryS: riaccoppia i dispositivi non accoppiati (es. accesi dopo il telecomando)
static void rediscovery()
{
  uint32_t now = millis();
  if (now - lastRediscoveryMs < (uint32_t)cfg.rediscoveryS * 1000)
    return;
  lastRediscoveryMs = now;

  const std::vector<EspDevice> &devices = espDevicesList();
  takeExclusiveExecution();
  for (size_t i = 0; i < devices.size(); i++)
  {
    if (devices[i].devicePassword.length() == 0)
      continue; // non configurato: inutile riprovare
    auto it = espDevicesState().find(devices[i].macAdress);
    bool paired = it != espDevicesState().end() && it->second.paired;
    if (!paired)
      requestPair((int)i);
  }
  freeExclusiveExecution();
}

static void wirelessTask(void *pvParameters)
{
  Serial.println("[Task] wirelessTask avviato");
  lastRediscoveryMs = millis();
  while (true)
  {
    WirelessRx rx;
    // l'attesa sulla coda fa anche da pausa del giro
    if (wirelessLinkReceive(rx, WIRELESS_LOOP_MS))
    {
      handleRx(rx);
      while (wirelessLinkReceive(rx, 0))
        handleRx(rx);
    }
    handleTxResults();
    processPending();
    endSessionIfIdle();
    sendHeartbeats();
    rediscovery();
    updateRadioSleep();
  }
}

// ---------------------------------------------------------------------------

void wirelessManagerBegin(const WirelessConfig &config)
{
  cfg = config;
  for (int i = 0; i < ESP_DEVICES_MAX; i++)
    pairPending[i] = false;
  pairQueue = xQueueCreate(ESP_DEVICES_MAX, sizeof(uint8_t));
  xTaskCreate(pairingTask, "PairingTaskExecution", STACK_SIZE_PAIRING, NULL, PRIORITY_PAIRING, NULL);
  xTaskCreate(wirelessTask, "WirelessTaskExecution", STACK_SIZE_WIRELESS, NULL, PRIORITY_WIRELESS, NULL);
}

int wirelessRequestPairAll()
{
  int queued = 0;
  for (size_t i = 0; i < espDevicesList().size(); i++)
  {
    if (requestPair((int)i))
      queued++;
  }
  return queued;
}

String wirelessManagerStatus()
{
  String out = "[Wireless] sessione joystick " + String(wirelessLinkSessionActive() ? "attiva" : "a riposo") +
               ", radio del telecomando " + String(radioSleeping ? "in modem sleep" : "accesa") + "\n";
  uint32_t now = millis();
  for (const auto &entry : wirelessSessions())
  {
    const WirelessSession &s = entry.second;
    out += "[Wireless] sessione " + s.deviceName + " (" + entry.first + "): " +
           (s.lost ? "persa, in riaccoppiamento" : (s.connected ? "collegata" : "in attesa del primo ACK")) +
           ", canale " + String(s.channel) + ", impronta chiave " + wirelessKeyFingerprint(s.sessionKey) +
           ", accoppiata da " + String((now - s.pairedMs) / 1000) + " s" +
           (s.lastAckMs != 0 ? ", ultimo ACK " + String((now - s.lastAckMs) / 1000) + " s fa" : String("")) +
           (s.missed > 0 ? ", heartbeat persi " + String(s.missed) : String("")) + "\n";
  }
  if (wirelessSessions().empty())
    out += "[Wireless] nessun dispositivo accoppiato\n";
  return out;
}
