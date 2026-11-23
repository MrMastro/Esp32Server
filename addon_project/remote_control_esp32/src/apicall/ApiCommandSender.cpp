#include "ApiCommandSender.h"
#include "devices/EspDevices.h"
#include "system/ExclusiveExecution.h"
#include "system/Log.h"
#include <ArduinoJson.h>
#include <WiFi.h>

// Priorita' bassa come il task di accoppiamento (C6 a un solo core: la seriale ha la precedenza)
#define PRIORITY_API_SENDER 1
#define STACK_SIZE_API_SENDER 6144

static const uint8_t QUEUE_LENGTH = 8;              // comandi in attesa per dispositivo (piena: si scarta il piu' vecchio)
static const uint32_t TIMEOUT_REACHABLE_MS = 5000;  // dispositivo che ha risposto all'ultima chiamata
static const uint32_t TIMEOUT_UNREACHABLE_MS = 2000; // dispositivo che non ha risposto: attesa breve, per non accumulare ritardo
static const uint32_t PROBE_TIMEOUT_MS = 5000;      // GET getInfoHello iniziale
static const char *PROBE_PATH = "/api/v1/getInfoHello";

struct CommandItem
{
  char command[24];
};

struct Worker
{
  int index = -1; // in espDevicesList()
  QueueHandle_t queue = nullptr;
  volatile bool reachable = false;
  volatile uint32_t sent = 0;
  volatile uint32_t answered = 0;
  volatile uint32_t failed = 0;
  volatile uint32_t dropped = 0;
  String lastCommand;
  String lastResult;
  uint32_t lastMs = 0;
};

static Worker workers[ESP_DEVICES_MAX];
static int workerCount = 0;
static bool started = false;

// Esito di una chiamata: campi del worker (letti dal comando devices, che gira DENTRO il lock) e pallino del
// dispositivo (CONNECTED se ha risposto, OFFLINE con il motivo se no). Prende il lock.
static void recordResult(Worker &w, const EspDevice &device, bool reachable, const String &command, const String &result,
                         const String &error)
{
  takeExclusiveExecution();
  w.reachable = reachable;
  if (command.length() > 0)
    w.lastCommand = command;
  w.lastResult = result;
  w.lastMs = millis();
  auto it = espDevicesState().find(device.macAdress);
  if (it != espDevicesState().end())
  {
    EspDeviceState state = it->second;
    state.status = reachable ? ESP_DEVICE_STATUS::CONNECTED : ESP_DEVICE_STATUS::OFFLINE;
    state.error = reachable ? "" : error;
    state.paired = false;
    state.pairError = "";
    state.lastCheckMs = millis();
    espDevicesSetState(state);
  }
  freeExclusiveExecution();
}

static void prepareArp(const EspDevice &device)
{
  String arpError;
  if (!espDevicesPrepareArp(device.ip, device.macAdress, arpError))
    logWarning("[ApiCall] " + device.deviceName + ": ARP: " + arpError);
}

static void probeDevice(Worker &w, const EspDevice &device)
{
  prepareArp(device);
  int statusCode = 0;
  String body, error;
  bool ok = espDevicesHttpGet(device.ip, PROBE_PATH, PROBE_TIMEOUT_MS, statusCode, body, error);
  recordResult(w, device, ok, "", ok ? "getInfoHello HTTP " + String(statusCode) : "getInfoHello: " + error, error);
  logInfo("[ApiCall] " + device.deviceName + (ok ? " raggiungibile" : " non raggiungibile: " + error));
}

static void sendCommand(Worker &w, const EspDevice &device, const String &command)
{
  JsonDocument req;
  req["command"] = command;
  String body;
  serializeJson(req, body);

  prepareArp(device);
  int statusCode = 0;
  String response, error;
  uint32_t start = millis();
  w.sent = w.sent + 1;
  bool ok = espDevicesHttpPost(device.ip, API_SEND_COMMAND_PATH, body,
                               w.reachable ? TIMEOUT_REACHABLE_MS : TIMEOUT_UNREACHABLE_MS, statusCode, response, error);
  uint32_t elapsed = millis() - start;
  if (ok)
    w.answered = w.answered + 1;
  else
    w.failed = w.failed + 1;
  bool changed = ok != w.reachable;
  recordResult(w, device, ok, command, ok ? "HTTP " + String(statusCode) + " in " + String(elapsed) + " ms" : error, error);
  if (ok)
    logInfo("[ApiCall] " + device.deviceName + " <- " + command + ": HTTP " + String(statusCode) + " in " + String(elapsed) + " ms");
  else if (changed || w.failed == 1)
    logInfo("[ApiCall] " + device.deviceName + " <- " + command + ": " + error);
}

static void senderTask(void *pvParameters)
{
  Worker &w = *(Worker *)pvParameters;
  const EspDevice &device = espDevicesList()[w.index]; // la lista non cambia dopo l'avvio
  logInfo("[Task] apiSenderTask avviato per " + device.deviceName);
  probeDevice(w, device);
  while (true)
  {
    CommandItem item;
    if (xQueueReceive(w.queue, &item, portMAX_DELAY) != pdTRUE)
      continue;
    sendCommand(w, device, String(item.command));
  }
}

void apiCommandSenderBegin()
{
  if (started)
    return;
  started = true;
  // radio sempre accesa: con il modem sleep ogni chiamata attenderebbe il beacon successivo del router
  WiFi.setSleep(false);
  const std::vector<EspDevice> &devices = espDevicesList();
  for (size_t i = 0; i < devices.size() && i < ESP_DEVICES_MAX; i++)
  {
    Worker &w = workers[workerCount];
    w.index = (int)i;
    w.queue = xQueueCreate(QUEUE_LENGTH, sizeof(CommandItem));
    if (w.queue == nullptr)
    {
      logWarning("[ApiCall] coda non creata per " + devices[i].deviceName);
      continue;
    }
    if (xTaskCreate(senderTask, "ApiSenderTask", STACK_SIZE_API_SENDER, &w, PRIORITY_API_SENDER, NULL) != pdPASS)
    {
      logWarning("[ApiCall] task non creato per " + devices[i].deviceName);
      continue;
    }
    workerCount++;
  }
  logInfo("[ApiCall] modalita' API-CALL: POST " API_SEND_COMMAND_PATH " a " + String(workerCount) + " dispositivi");
}

bool apiCommandSend(J_DIRECTION direction)
{
  if (direction == J_DIRECTION::CENTER || direction == J_DIRECTION::CENTER_BUTTON || direction == J_DIRECTION::NONE)
    return false;
  String command = mapJdirectionsToString(direction);
  if (command.length() == 0)
    return false;
  CommandItem item = {};
  strlcpy(item.command, command.c_str(), sizeof(item.command));
  for (int i = 0; i < workerCount; i++)
  {
    Worker &w = workers[i];
    if (xQueueSend(w.queue, &item, 0) != pdTRUE)
    {
      // coda piena (dispositivo lento o spento): si scarta il comando piu' vecchio
      CommandItem old;
      xQueueReceive(w.queue, &old, 0);
      w.dropped = w.dropped + 1;
      xQueueSend(w.queue, &item, 0);
    }
  }
  return true;
}

String apiCommandSenderStatus()
{
  String out = "[ApiCall] modalita' API-CALL (commandTransportMode = API_CALL): POST " API_SEND_COMMAND_PATH "\n";
  const std::vector<EspDevice> &devices = espDevicesList();
  uint32_t now = millis();
  for (int i = 0; i < workerCount; i++)
  {
    const Worker &w = workers[i];
    out += "[ApiCall] " + devices[w.index].deviceName + ": " + (w.reachable ? "raggiungibile" : "non raggiungibile") +
           ", inviati " + String(w.sent) + ", con risposta " + String(w.answered) + ", senza risposta " + String(w.failed) +
           (w.dropped > 0 ? ", scartati (coda piena) " + String(w.dropped) : String("")) +
           ", in coda " + String(uxQueueMessagesWaiting(w.queue)) +
           (w.lastMs != 0 ? ", ultimo: " + (w.lastCommand.length() > 0 ? w.lastCommand + " -> " : String("")) + w.lastResult +
                                " (" + String((now - w.lastMs) / 1000) + " s fa)"
                          : String("")) +
           "\n";
  }
  if (workerCount == 0)
    out += "[ApiCall] nessun dispositivo in espDevices.json\n";
  return out;
}
