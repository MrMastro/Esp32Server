#include "WirelessManager.h"
#include "system/Log.h"
#include "WirelessLink.h"
#include "WirelessPairing.h"
#include "devices/EspDevices.h"
#include "system/ExclusiveExecution.h"
#include "display/RemoteControlScreen.h"
#include "display/Display.h" // colori RGB565_*
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
// Nuovi tentativi verso i dispositivi non collegati, uno per dispositivo (indici in espDevicesList()):
// dopo un accoppiamento senza risposta fastRetryS, poi il doppio a ogni fallimento, fino a rediscoveryS
static uint32_t retryAtMs[ESP_DEVICES_MAX];       // 0 = nessuna attesa (si tenta alla prossima verifica)
static uint8_t retryFails[ESP_DEVICES_MAX];       // accoppiamenti consecutivi senza risposta
static const uint32_t RETRY_CHECK_MS = 1000;      // ogni quanto si controlla chi va ritentato
static uint32_t radioHoldUntilMs = 0;             // radio accesa almeno fino a qui (attesa di ACK)
static bool radioSleeping = false;                 // stato del modem sleep del telecomando applicato

// Disconnessione / connessione con la pressione lunga di SW (fase 5)
enum class LINK_STATE
{
  CONNECTED,     // funzionamento normale (dispositivi accoppiati o in accoppiamento)
  DISCONNECTING, // DISCONNECT inviato, in attesa degli ACK
  DISCONNECTED   // disconnesso: niente invii, heartbeat, riscoperta ne' accoppiamenti
};
static const uint32_t DISCONNECT_SEND_WINDOW_MS = 400; // ritrasmissione di DISCONNECT (dispositivi forse in modem sleep)
static const uint32_t DISCONNECT_ACK_WAIT_MS = 500;    // attesa massima degli ACK, poi si va avanti lo stesso
static const uint32_t RESULT_MESSAGE_MS = 1000;        // conferma breve: "DISCONNESSO" / "CONNESSO" / "NESSUN DISPOSITIVO"
static const uint32_t CONNECTING_TIMEOUT_MS = 15000;   // slider "CONNESSIONE" al massimo per questo tempo
static volatile LINK_STATE linkState = LINK_STATE::CONNECTED;
static volatile bool connectToggleRequested = false;
static uint32_t disconnectDeadlineMs = 0;
// Riscontro a schermo della connessione richiesta con SW: slider "CONNESSIONE" finche' il primo
// dispositivo risulta collegato (HEARTBEAT_ACK) o nessuno ha accettato l'accoppiamento
static volatile bool connectingFeedback = false;
static volatile int connectingPaired = 0;
static uint32_t connectingStartMs = 0;

// Fine del riscontro della connessione: via lo slider, conferma breve (DENTRO il lock)
static void finishConnectingFeedback(bool connected)
{
  connectingFeedback = false;
  remoteControlScreenSetBusy(nullptr);
  if (connected)
    remoteControlScreenShowMessage("CONNESSO", "", RGB565_GREEN, RESULT_MESSAGE_MS);
  else
    remoteControlScreenShowMessage("NESSUN", "DISPOSITIVO", RGB565_ORANGE, RESULT_MESSAGE_MS);
}

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

// Prossimo tentativo verso un dispositivo (DENTRO il lock). unreachable = nessuna risposta HTTP:
// attesa crescente da fastRetryS a rediscoveryS; altrimenti (rifiutato, sessione persa) rediscoveryS.
static void scheduleRetry(int index, bool unreachable)
{
  if (index < 0 || index >= ESP_DEVICES_MAX)
    return;
  uint32_t maxMs = (uint32_t)cfg.rediscoveryS * 1000;
  uint32_t delayMs = maxMs;
  if (unreachable)
  {
    uint8_t shift = min<uint8_t>(retryFails[index], 8);
    delayMs = min<uint32_t>(((uint32_t)cfg.fastRetryS * 1000) << shift, maxMs);
    if (retryFails[index] < 255)
      retryFails[index]++;
  }
  retryAtMs[index] = millis() + delayMs;
  if (retryAtMs[index] == 0)
    retryAtMs[index] = 1; // 0 vuol dire "nessuna attesa"
}

static void resetRetry(int index)
{
  if (index < 0 || index >= ESP_DEVICES_MAX)
    return;
  retryAtMs[index] = 0;
  retryFails[index] = 0;
}

// Accoda l'accoppiamento di un dispositivo (se non e' gia' in coda). Chiamabile ovunque.
static bool requestPair(int index)
{
  if (index < 0 || index >= (int)espDevicesList().size() || pairQueue == nullptr || pairPending[index] ||
      linkState != LINK_STATE::CONNECTED)
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
// Eccezione: nessuna risposta HTTP ma la sessione attuale funziona ancora (heartbeat confermati, es. comando
// "pair" con una rete lenta) -> la sessione resta: il dispositivo non ha ricevuto nulla e usa ancora quella chiave.
// Se invece la richiesta fosse arrivata e solo la risposta persa, il dispositivo avrebbe la chiave nuova:
// gli heartbeat ricevono UNKNOWN_SESSION e si riaccoppia da solo.
static void pairDevice(int index, const EspDevice &device)
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
    result = wirelessPair(device, (uint32_t)cfg.pairTimeoutS * 1000, session, error);
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
  if (linkState != LINK_STATE::CONNECTED)
  {
    // disconnesso mentre l'accoppiamento era in corso: si scarta (alla connessione se ne fa uno nuovo)
    freeExclusiveExecution();
    return;
  }

  if (result == PAIR_RESULT::UNREACHABLE)
  {
    auto it = wirelessSessions().find(device.macAdress);
    if (it != wirelessSessions().end() && !it->second.lost)
    {
      // sessione ancora valida: niente da cambiare (ne' sessione ne' pallino)
      resetRetry(index);
      freeExclusiveExecution();
      logInfo("[Wireless] " + device.deviceName + ": riaccoppiamento senza risposta (" + error +
              "), resta la sessione attuale");
      return;
    }
  }

  if (result == PAIR_RESULT::PAIRED)
  {
    connectingPaired = connectingPaired + 1;
    wirelessSessionSet(session); // sostituisce la sessione precedente; primo heartbeat subito
    resetRetry(index);
  }
  else
  {
    wirelessSessionRemove(device.macAdress);
    scheduleRetry(index, result == PAIR_RESULT::UNREACHABLE);
  }
  espDevicesSetState(state);
  uint32_t retryInS = result == PAIR_RESULT::PAIRED ? 0 : (retryAtMs[index] - millis() + 500) / 1000;
  freeExclusiveExecution();

  if (result == PAIR_RESULT::PAIRED)
    logInfo("[Wireless] " + device.deviceName + " accoppiato: canale " + String(session.channel) +
                   ", impronta chiave " + wirelessKeyFingerprint(session.sessionKey));
  else
    logInfo("[Wireless] " + device.deviceName + " non accoppiato: " + (state.pairError.length() > 0 ? state.pairError : error) +
            ", nuovo tentativo tra " + String(retryInS) + " s");
}

static void pairingTask(void *pvParameters)
{
  logInfo("[Task] pairingTask avviato");
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
      pairDevice(index, devices[index]);
      pairingInProgress = false;
    }
    pairPending[index] = false;
    // connessione richiesta con SW: finita la coda senza nessun dispositivo accoppiato -> riscontro a schermo
    if (connectingFeedback && uxQueueMessagesWaiting(pairQueue) == 0 && connectingPaired == 0)
    {
      takeExclusiveExecution();
      finishConnectingFeedback(false);
      freeExclusiveExecution();
      logInfo("[Wireless] connessione: nessun dispositivo ha accettato l'accoppiamento");
    }
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
    logInfo("[Wireless] " + s.deviceName + ": " + reason + ", riaccoppiamento");
    requestPair(deviceIndexByMac(mac));
  }
  else
  {
    scheduleRetry(deviceIndexByMac(mac), false);
    logInfo("[Wireless] " + s.deviceName + ": " + reason + ", riprovo tra " + String(cfg.rediscoveryS) + " s");
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
      logInfo("[Wireless] " + s.deviceName + " collegato (HEARTBEAT_ACK)");
      if (connectingFeedback)
        finishConnectingFeedback(true);
    }
    break;

  case WIRELESS_PKT_DISCONNECT_ACK:
    if (rx.packet.version != WIRELESS_PROTOCOL_VERSION || !wirelessVerifyPacket(s.sessionKey, rx.packet) ||
        rx.packet.seq <= s.rxSeq)
      break;
    s.rxSeq = rx.packet.seq;
    s.disconnectAcked = true;
    logInfo("[Wireless] t=" + String(millis()) + " DISCONNECT_ACK da " + s.deviceName);
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
  logInfo("[Wireless] fine sessione (" + String(cfg.sessionTimeoutS) + " s senza movimenti)");
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

// Riaccoppia i dispositivi non accoppiati (es. accesi dopo il telecomando, riavviati, rete lenta) quando
// scade la loro attesa (scheduleRetry): pochi secondi dopo un fallimento, poi sempre piu' di rado.
static void rediscovery()
{
  uint32_t now = millis();
  if (now - lastRediscoveryMs < RETRY_CHECK_MS)
    return;
  lastRediscoveryMs = now;

  const std::vector<EspDevice> &devices = espDevicesList();
  takeExclusiveExecution();
  for (size_t i = 0; i < devices.size() && i < ESP_DEVICES_MAX; i++)
  {
    if (devices[i].devicePassword.length() == 0)
      continue; // non configurato: inutile riprovare
    auto it = espDevicesState().find(devices[i].macAdress);
    bool paired = it != espDevicesState().end() && it->second.paired;
    if (paired || pairPending[i])
      continue;
    if (retryAtMs[i] != 0 && (int32_t)(now - retryAtMs[i]) < 0)
      continue; // non ancora
    requestPair((int)i);
  }
  freeExclusiveExecution();
}

// Imposta lo stesso stato a tutti i dispositivi (DENTRO il lock)
static void setAllDevicesStatus(ESP_DEVICE_STATUS status, const String &error, const String &pairError)
{
  for (const EspDevice &device : espDevicesList())
  {
    auto it = espDevicesState().find(device.macAdress);
    if (it == espDevicesState().end())
      continue;
    EspDeviceState state = it->second;
    state.status = status;
    state.paired = false;
    state.error = error;
    state.pairError = pairError;
    state.lastCheckMs = millis();
    espDevicesSetState(state);
  }
}

// Disconnessione: per i dispositivi e' come spegnere il telecomando, ma vengono avvisati subito
static void startDisconnect()
{
  takeExclusiveExecution();
  linkState = LINK_STATE::DISCONNECTING;
  connectingFeedback = false;
  disconnectDeadlineMs = millis() + DISCONNECT_ACK_WAIT_MS;
  wirelessLinkSetDisconnected(true);
  wirelessLinkSetSessionActive(false);
  int sent = 0;
  for (auto &entry : wirelessSessionsMutable())
  {
    WirelessSession &s = entry.second;
    s.disconnectAcked = false;
    if (s.lost)
      continue;
    wirelessLinkSendReliable(s, WIRELESS_PKT_DISCONNECT, DISCONNECT_SEND_WINDOW_MS);
    sent++;
  }
  radioHoldUntilMs = max(radioHoldUntilMs, millis() + DISCONNECT_ACK_WAIT_MS + ACK_HOLD_MS);
  freeExclusiveExecution();
  logInfo("[Wireless] t=" + String(millis()) + " disconnessione: DISCONNECT a " + String(sent) + " dispositivi");
}

// Fine dell'attesa degli ACK (tutti confermati o tempo scaduto): sessioni cancellate, pallini grigi
static void finishDisconnectIfDone()
{
  if (linkState != LINK_STATE::DISCONNECTING)
    return;
  takeExclusiveExecution();
  int total = 0, acked = 0;
  for (const auto &entry : wirelessSessions())
  {
    if (entry.second.lost)
      continue;
    total++;
    if (entry.second.disconnectAcked)
      acked++;
  }
  if (acked < total && (int32_t)(millis() - disconnectDeadlineMs) < 0)
  {
    freeExclusiveExecution();
    return; // si aspettano ancora degli ACK
  }
  std::vector<String> macs;
  for (const auto &entry : wirelessSessions())
    macs.push_back(entry.first);
  for (const String &mac : macs)
    wirelessSessionRemove(mac); // chiavi cancellate: alla connessione se ne fanno di nuove
  // pallino vuoto: per il telecomando i dispositivi disconnessi sono offline
  setAllDevicesStatus(ESP_DEVICE_STATUS::OFFLINE, "disconnesso (pressione lunga di SW)", "disconnesso (pressione lunga di SW)");
  linkState = LINK_STATE::DISCONNECTED;
  remoteControlScreenSetDisconnected(true);
  remoteControlScreenSetBusy(nullptr);
  remoteControlScreenShowMessage("DISCONNESSO", "", RGB565_ORANGE, RESULT_MESSAGE_MS);
  freeExclusiveExecution();
  logInfo("[Wireless] t=" + String(millis()) + " dispositivi disconnessi: " + String(acked) + "/" + String(total) +
                 " hanno confermato" +
                 (acked < total ? " (gli altri chiuderanno la sessione allo scadere del loro timeout)" : ""));
}

// Connessione: come all'accensione (accoppiamento da capo, nuove chiavi)
static void startConnect()
{
  takeExclusiveExecution();
  linkState = LINK_STATE::CONNECTED;
  wirelessLinkSetDisconnected(false);
  setAllDevicesStatus(ESP_DEVICE_STATUS::OFFLINE, "in connessione", "");
  remoteControlScreenSetDisconnected(false);
  connectingPaired = 0;
  connectingFeedback = true;
  connectingStartMs = millis(); // lo slider "CONNESSIONE" e' gia' a schermo (main.cpp)
  for (int i = 0; i < ESP_DEVICES_MAX; i++)
    resetRetry(i);
  freeExclusiveExecution();
  lastRediscoveryMs = millis();
  int queued = wirelessRequestPairAll();
  logInfo("[Wireless] t=" + String(millis()) + " connessione: accoppiamento di " + String(queued) + " dispositivi");
  if (queued == 0)
  {
    takeExclusiveExecution();
    finishConnectingFeedback(false);
    freeExclusiveExecution();
  }
}

static void handleConnectToggleRequest()
{
  if (!connectToggleRequested)
    return;
  connectToggleRequested = false;
  logInfo("[Wireless] t=" + String(millis()) + " richiesta di disconnessione/connessione presa dal task wireless");
  if (linkState == LINK_STATE::CONNECTED)
    startDisconnect();
  else if (linkState == LINK_STATE::DISCONNECTED)
    startConnect();
  // DISCONNECTING: si ignora (disconnessione gia' in corso: il suo slider resta a schermo)
}

static void wirelessTask(void *pvParameters)
{
  logInfo("[Task] wirelessTask avviato");
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
    handleConnectToggleRequest();
    finishDisconnectIfDone();
    if (connectingFeedback && millis() - connectingStartMs >= CONNECTING_TIMEOUT_MS)
    {
      // accoppiati ma nessun HEARTBEAT_ACK entro il tempo massimo
      takeExclusiveExecution();
      finishConnectingFeedback(false);
      freeExclusiveExecution();
      logInfo("[Wireless] connessione: nessun dispositivo collegato entro " + String(CONNECTING_TIMEOUT_MS / 1000) + " s");
    }
    if (linkState == LINK_STATE::CONNECTED)
    {
      endSessionIfIdle();
      sendHeartbeats();
      rediscovery();
    }
    updateRadioSleep();
  }
}

// ---------------------------------------------------------------------------

void wirelessManagerBegin(const WirelessConfig &config)
{
  cfg = config;
  for (int i = 0; i < ESP_DEVICES_MAX; i++)
  {
    pairPending[i] = false;
    retryAtMs[i] = 0;
    retryFails[i] = 0;
  }
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

void wirelessRequestConnectToggle()
{
  connectToggleRequested = true;
}

bool wirelessManagerDisconnected()
{
  return linkState != LINK_STATE::CONNECTED;
}

uint32_t wirelessManagerLongPressMs()
{
  return (uint32_t)cfg.longPressS * 1000;
}

String wirelessManagerStatus()
{
  String out = String(linkState == LINK_STATE::CONNECTED ? "" : "[Wireless] dispositivi DISCONNESSI (pressione lunga di SW per riconnettere)\n") +
               "[Wireless] sessione joystick " + String(wirelessLinkSessionActive() ? "attiva" : "a riposo") +
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
  // dispositivi non accoppiati: quando si riprova
  const std::vector<EspDevice> &devices = espDevicesList();
  for (size_t i = 0; i < devices.size() && i < ESP_DEVICES_MAX; i++)
  {
    if (wirelessSessions().count(devices[i].macAdress) > 0 || devices[i].devicePassword.length() == 0 ||
        linkState != LINK_STATE::CONNECTED)
      continue;
    String when;
    if (pairPending[i])
      when = "accoppiamento in corso o in coda";
    else if (retryAtMs[i] == 0 || (int32_t)(now - retryAtMs[i]) >= 0)
      when = "nuovo tentativo a breve";
    else
      when = "nuovo tentativo tra " + String((retryAtMs[i] - now + 500) / 1000) + " s";
    out += "[Wireless] " + devices[i].deviceName + " non accoppiato: " + when +
           (retryFails[i] > 0 ? " (tentativi senza risposta di fila: " + String(retryFails[i]) + ")" : String("")) + "\n";
  }
  return out;
}
