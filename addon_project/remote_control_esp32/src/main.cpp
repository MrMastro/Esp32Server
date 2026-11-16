#include <Arduino.h>

#include "BoardPins.h"
#include "display/Display.h"
#include "display/StatusBar.h"
#include "storage/SdStorage.h"
#include "settings/SettingsModel.h"
#include "settings/DeviceMode.h"
#include "network/WifiConnection.h"
#include "network/OtaUpload.h"
#include "settings/SettingsValidator.h"
#include "system/ExclusiveExecution.h"
#include <WebServer.h>
#include "hal/usb_serial_jtag_ll.h" // watchdog ricezione USB, vedi readSerialMessage()

#ifndef BOOT_SERIAL_WAIT_MS
#define BOOT_SERIAL_WAIT_MS 3000 // valore reale in platformio.ini (build_flags)
#endif
#include "joystick/Joystick.h"
#include "joystick/JoystickConfig.h"
#include "test/JoystickDisplayTest.h"

static SettingsModel settings;
static DEVICE_MODE deviceMode = DEVICE_MODE::REMOTE_CONTROL;
static WifiStatus wifiStatus;

static JoystickConfig joystickConfig;
static Joystick joystick;

static bool sdAvailable = false;

// ---------------------------------------------------------------------------
// API impostazioni: POST /api/settings
// Corpo JSON: deviceName + devicePassword (autenticazione) + UNA chiave di main_settings.json.
//   { "deviceName": "ESP32_Remote", "devicePassword": "...", "ota_upload": true }
//   { "deviceName": "ESP32_Remote", "devicePassword": "...", "communicationMode": "WIFI_MODE" }
// Se valida: salva la chiave sulla SD, risponde 200 e riavvia la scheda dopo 5 secondi.
// ---------------------------------------------------------------------------
static WebServer server(80);

static const uint32_t REBOOT_DELAY_MS = 5000;
static bool rebootPending = false;
static uint32_t rebootAtMs = 0;
static String rebootKey = "";
static int rebootShownSeconds = -1;

static void sendJson(int code, const String &status, const String &message)
{
  JsonDocument res;
  res["status"] = status;
  res["message"] = message;
  String body;
  serializeJson(res, body);
  server.send(code, "application/json", body);
}

static void handleSettingsPost()
{
  if (rebootPending)
  {
    sendJson(409, "error", "riavvio già in corso");
    return;
  }
  if (!server.hasArg("plain"))
  {
    sendJson(400, "error", "corpo della richiesta mancante");
    return;
  }

  JsonDocument request;
  DeserializationError error = deserializeJson(request, server.arg("plain"));
  if (error)
  {
    sendJson(400, "error", String("JSON non valido: ") + error.c_str());
    return;
  }

  SettingsValidation v = validateSettingsRequest(request, settings);
  if (!v.ok)
  {
    Serial.println("[API] richiesta rifiutata: " + v.error);
    sendJson(v.httpCode, "error", v.error);
    return;
  }

  if (!sdAvailable)
  {
    sendJson(503, "error", "microSD non disponibile: impossibile salvare");
    return;
  }
  if (!settings.saveKey(SD_MAIN_SETTINGS, v.key, request[v.key]))
  {
    sendJson(500, "error", "scrittura su microSD fallita");
    return;
  }

  Serial.println("[API] modificata la chiave '" + v.key + "', riavvio tra 5 secondi");
  sendJson(200, "ok", "chiave '" + v.key + "' salvata, riavvio tra 5 secondi");

  rebootPending = true;
  rebootKey = v.key;
  rebootAtMs = millis() + REBOOT_DELAY_MS;
  rebootShownSeconds = -1;
}

// Schermata "chiave modificata + conto alla rovescia", poi riavvio
static void handlePendingReboot()
{
  int32_t remainingMs = (int32_t)(rebootAtMs - millis());
  if (remainingMs <= 0)
  {
    displayMessage("Riavvio...");
    delay(100);
    ESP.restart();
  }

  int seconds = (remainingMs + 999) / 1000;
  if (seconds == rebootShownSeconds)
    return;
  rebootShownSeconds = seconds;

  gfx->fillScreen(RGB565_BLACK);
  displayPrintCentered("MODIFICA", 40, 3, RGB565_CYAN);
  uint8_t keySize = rebootKey.length() * 6 * 2 <= LCD_WIDTH - 10 ? 2 : 1;
  displayPrintCentered(rebootKey, 90, keySize, RGB565_YELLOW);
  displayPrintCentered("Riavvio tra", 140, 2, RGB565_WHITE);
  displayPrintCentered(String(seconds), 170, 4, RGB565_ORANGE);
  displayFlush();
}

static void serverBegin()
{
  server.on("/api/settings", HTTP_POST, handleSettingsPost);
  server.onNotFound([]()
                    { sendJson(404, "error", "endpoint non trovato"); });
  server.begin();
  Serial.println("[API] server attivo: POST http://" + wifiStatus.ip + "/api/settings");
}

static void setupRemoteControl(bool sdAvailable)
{
  if (sdAvailable)
  {
    joystickConfig.loadFromFile(SD_JOYSTICK_CONFIG);
  }

  if (joystickConfig.enableJoystick)
  {
    joystick.begin(joystickConfig);
    if (joystickConfig.autoCalibrate)
    {
      displayMessage("Calibrazione", "non toccare");
      joystick.calibrate();
    }
  }
}

// Stampa la configurazione in uso (password in chiaro, per il debug)
static void printSettings()
{
  Serial.println("\nLoad settings:");
  Serial.println(settings.toJsonString(false));
}

// ---------------------------------------------------------------------------
// Comandi seriali: stessi nomi e formato del progetto principale (CommandService / Commands.cpp),
// "comando,param1,param2" con i parametri separati da virgola (qui va bene anche lo spazio).
// ---------------------------------------------------------------------------

// Riavvio ritardato (come insertDelayedCommand(CMD::REBOOT, ...) del progetto principale)
static bool delayedRebootPending = false;
static uint32_t delayedRebootAtMs = 0;

static bool isUnsignedLong(const String &str)
{
  if (str.length() == 0)
    return false;
  for (size_t i = 0; i < str.length(); i++)
  {
    if (!isDigit(str[i]))
      return false;
  }
  return true;
}

static void insertDelayedReboot(unsigned long millsDelayed)
{
  delayedRebootPending = true;
  delayedRebootAtMs = millis() + millsDelayed;
}

static void checkDelayedRebootAndExecute()
{
  if (delayedRebootPending && (int32_t)(millis() - delayedRebootAtMs) >= 0)
  {
    Serial.println("Comando delayed eseguito: reboot");
    delay(50);
    ESP.restart();
  }
}

// Esito dei comandi, con gli stessi messaggi del progetto principale (constants.h)
static void printCommandSuccess(const String &command, const String &description = "")
{
  Serial.println("Comando eseguito: " + command + " - " + description);
}

static void printCommandError(const String &command, const String &description)
{
  Serial.println("ERRORE: " + command + " - " + description);
}

// reboot[,ms]: senza parametro (o 0) riavvia subito, altrimenti dopo ms millisecondi
static void reboot(const String &command, const String &param)
{
  if (param.length() > 0 && !isUnsignedLong(param))
  {
    printCommandError(command, "first param must be a number");
    return;
  }

  unsigned long millsDelayed = param.length() > 0 ? strtoul(param.c_str(), nullptr, 10) : 0;
  if (millsDelayed == 0)
  {
    printCommandSuccess(command, "restarting...");
    delay(50);
    ESP.restart();
    return;
  }
  insertDelayedReboot(millsDelayed);
  printCommandSuccess(command, "reboot tra " + String(millsDelayed) + " ms");
}

// change_communication_mode,<AP_MODE|WIFI_MODE>: salva su main_settings.json e riavvia dopo 1s
static void changeCommunicationMode(const String &command, const String &param)
{
  if (param.length() == 0)
  {
    printCommandError(command, "insert mode like first param");
    return;
  }
  COMMUNICATION_MODE cm = communicationModeStringToEnum(param);
  if (cm == COMMUNICATION_MODE::UNKNOWN_MODE)
  {
    printCommandError(command, "Communication mode is not valid");
    return;
  }
  if (!sdAvailable)
  {
    printCommandError(command, "microSD non disponibile: impossibile salvare");
    return;
  }

  JsonDocument value;
  value.set(communicationModeEnumToString(cm));
  if (!settings.saveKey(SD_MAIN_SETTINGS, "communicationMode", value.as<JsonVariantConst>()))
  {
    printCommandError(command, "Error while set new communication");
    return;
  }
  printCommandSuccess(command, "communication mode is setted, restarting....");
  insertDelayedReboot(1000);
}

static void printInfo(const String &command)
{
  String info = "mode=" + settings.mode +
                ", deviceName=" + settings.deviceName +
                ", communicationMode=" + (wifiStatus.fallbackToAp ? String("AP_MODE (fallback)") : communicationModeEnumToString(wifiStatus.activeMode)) +
                ", ssid=" + wifiStatus.ssid +
                ", ip=" + wifiStatus.ip +
                ", ota=" + (otaIsEnabled() ? "ON" : "OFF") +
                ", sd=" + (sdAvailable ? "OK" : "NO");
  printCommandSuccess(command, info);
}

// Funzione di prova lanciata dal comando seriale "t": metti qui il codice da testare
static void test()
{
  Serial.println("[Test] test() eseguita");
}

// Comandi riconosciuti (in C++ lo switch non accetta String: si mappa su un enum).
// I primi sono gli stessi di CMD nel progetto principale; gli ultimi sono solo di questo progetto.
enum class SERIAL_COMMAND
{
  REBOOT,                    // "reboot" (alias "reset")
  CHANGE_COMMUNICATION_MODE, // "change_communication_mode"
  INFO,                      // "info"
  INFO_IP,                   // "info_ip"
  SETTINGS,                  // "settings"
  TEST,                      // "t"
  NONE,
  UNKNOWN
};

static SERIAL_COMMAND parseSerialCommand(String command)
{
  command.toLowerCase();
  if (command.length() == 0)
    return SERIAL_COMMAND::NONE;
  if (command == "reboot" || command == "reset")
    return SERIAL_COMMAND::REBOOT;
  if (command == "change_communication_mode")
    return SERIAL_COMMAND::CHANGE_COMMUNICATION_MODE;
  if (command == "info")
    return SERIAL_COMMAND::INFO;
  if (command == "info_ip")
    return SERIAL_COMMAND::INFO_IP;
  if (command == "settings")
    return SERIAL_COMMAND::SETTINGS;
  if (command == "t")
    return SERIAL_COMMAND::TEST;
  return SERIAL_COMMAND::UNKNOWN;
}

// Comandi dal monitor seriale (caratteri in sequenza, il messaggio si chiude dopo 1s di pausa):
//   reboot[,ms]                      -> riavvia l'ESP32 (subito, o dopo ms millisecondi); alias: reset
//   change_communication_mode,<mode> -> AP_MODE / WIFI_MODE, salva sulla SD e riavvia
//   info                             -> stato del dispositivo
//   info_ip                          -> indirizzo IP
//   settings                         -> ristampa la configurazione caricata
//   t                                -> esegue test()
// Raccolta del messaggio seriale come SerialService::getMsgbySerial() del progetto principale
// (Serial.readString): i caratteri si scrivono in sequenza e il messaggio è completo quando
// non ne arrivano altri per SERIAL_MSG_TIMEOUT_MS. Non serve premere Invio.
// A differenza di readString non blocca il task mentre aspetta.
static const uint32_t SERIAL_MSG_TIMEOUT_MS = 1000; // come il timeout di default di Serial
static const size_t SERIAL_MSG_MAX_LENGTH = 64;

// Watchdog della ricezione USB (HWCDC, USB Serial/JTAG del C6).
// Il driver Arduino legge la FIFO hardware solo nell'interrupt "pacchetto ricevuto"; se un
// pacchetto arriva prima/durante Serial.begin() (che azzera i flag di interrupt) o se un
// interrupt va perso, i dati restano nella FIFO (64 byte), nessuno la svuota e l'host non può
// più inviare nulla: la COM risulta "occupata" (Write timeout) finché non si stacca il cavo.
// Qui, se la FIFO contiene dati non letti dal driver, li si legge direttamente.
static portMUX_TYPE usbRxMux = portMUX_INITIALIZER_UNLOCKED;

static size_t drainStuckUsbRxFifo(uint8_t *out, size_t maxLen)
{
  if (Serial.available() || !usb_serial_jtag_ll_rxfifo_data_available())
    return 0;
  portENTER_CRITICAL(&usbRxMux); // niente interrupt nel frattempo: l'ISR non legge in parallelo
  size_t n = usb_serial_jtag_ll_read_rxfifo(out, maxLen);
  portEXIT_CRITICAL(&usbRxMux);
  return n;
}

static bool readSerialMessage(String &msg)
{
  static String buffer = "";
  static uint32_t lastCharMs = 0;

  while (Serial.available())
  {
    char c = Serial.read();
    if (buffer.length() < SERIAL_MSG_MAX_LENGTH)
      buffer += c;
    lastCharMs = millis();
  }

  uint8_t stuck[64];
  size_t stuckLen = drainStuckUsbRxFifo(stuck, sizeof(stuck));
  for (size_t i = 0; i < stuckLen; i++)
  {
    if (buffer.length() < SERIAL_MSG_MAX_LENGTH)
      buffer += (char)stuck[i];
    lastCharMs = millis();
  }

  if (buffer.length() == 0 || millis() - lastCharMs < SERIAL_MSG_TIMEOUT_MS)
    return false;

  msg = buffer;
  buffer = "";
  msg.trim(); // toglie eventuali CR/LF se il monitor li invia
  return msg.length() > 0;
}

static void executeSerialMessage(const String &msg)
{
  Serial.println("Serial has msg: " + msg);

  String command = msg;
  String param = "";
  int sep = msg.indexOf(',');
  if (sep < 0)
    sep = msg.indexOf(' ');
  if (sep > 0)
  {
    command = msg.substring(0, sep);
    param = msg.substring(sep + 1);
    command.trim();
    param.trim();
  }

  switch (parseSerialCommand(command))
  {
  case SERIAL_COMMAND::REBOOT:
    reboot(command, param);
    break;
  case SERIAL_COMMAND::CHANGE_COMMUNICATION_MODE:
    changeCommunicationMode(command, param);
    break;
  case SERIAL_COMMAND::INFO:
    printInfo(command);
    break;
  case SERIAL_COMMAND::INFO_IP:
    printCommandSuccess(command, wifiStatus.ip);
    break;
  case SERIAL_COMMAND::SETTINGS:
    printSettings();
    break;
  case SERIAL_COMMAND::TEST:
    test();
    break;
  case SERIAL_COMMAND::UNKNOWN:
    printCommandError(command, "Comando non riconosciuto (disponibili: reboot[,ms], change_communication_mode,<mode>, info, info_ip, settings, t)");
    break;
  case SERIAL_COMMAND::NONE:
  default:
    break;
  }
}

static void startTasks(); // definita sotto, insieme ai task

void setup()
{
  Serial.begin(115200);
  // NB: non usare Serial.setTxTimeoutMs(0): sul C6 manda in stallo la ricezione USB (HWCDC)
  // dopo le stampe lunghe e la COM smette di accettare comandi. Si lascia il default (100ms):
  // senza host USB (es. power bank) le print vengono comunque scartate senza attese.

  // 1) Display: avvia anche il bus SPI condiviso con la SD
  displayBegin();
  displayMessage("Avvio...");

  // Dopo ogni reset la porta USB del C6 sparisce e ricompare: il monitor seriale impiega
  // qualche secondo a ricollegarsi e ciò che viene stampato prima va perso (Windows lo legge
  // e lo scarta). Si attende BOOT_SERIAL_WAIT_MS prima del log di avvio (0 = nessuna attesa).
  delay(BOOT_SERIAL_WAIT_MS);

  // 2) microSD + impostazioni generali (default se SD o file mancano).
  //    Come nel progetto principale, la prima cosa stampata è la configurazione caricata.
  String sdInfo, settingsInfo;
  sdAvailable = sdBegin(&sdInfo);
  bool settingsFromFile = false;
  settings = SettingsModel::load(SD_MAIN_SETTINGS, sdAvailable, &settingsFromFile, &settingsInfo);
  printSettings();
  Serial.println(settingsInfo);
  Serial.println(sdInfo);

  if (!settingsFromFile)
  {
    displayMessage(sdAvailable ? "No settings" : "No SD", "uso default");
    delay(1500);
  }

  // 3) Rete (AP_MODE / WIFI_MODE)
  displayMessage("WiFi...", settings.communicationMode);
  wifiStatus = wifiBegin(settings);
  displayMessage(wifiStatus.fallbackToAp ? "AP (fallback)" : communicationModeEnumToString(wifiStatus.activeMode),
                 wifiStatus.ip);
  delay(1500);

  // 4) OTA (solo se ota_upload = true) e API impostazioni
  otaBegin(settings);
  serverBegin();

  // 5) Modalità del dispositivo
  deviceMode = deviceModeStringToEnum(settings.mode);
  if (deviceMode == DEVICE_MODE::UNKNOWN)
  {
    Serial.println("[Main] mode sconosciuta (" + settings.mode + "): uso remote_control");
    displayMessage("Mode ignota", "uso remote_control");
    delay(1500);
    deviceMode = DEVICE_MODE::REMOTE_CONTROL;
  }

  switch (deviceMode)
  {
  case DEVICE_MODE::REMOTE_CONTROL:
    setupRemoteControl(sdAvailable);
    break;
  default:
    break;
  }

  statusBarBegin(&wifiStatus);
  testJoystickSetNetworkInfo("IP " + wifiStatus.ip + (otaIsEnabled() ? "  OTA ON" : ""));

  // 6) Task (serial, API/OTA, comandi ritardati, joystick)
  startTasks();
}

// ---------------------------------------------------------------------------
// Task FreeRTOS (come Main.cpp del progetto principale).
// Ogni task fa un giro di lavoro dentro take/freeExclusiveExecution e poi dorme 10ms
// con vTaskDelay: il C6 ha un solo core, quindi ogni task DEVE cedere la CPU.
// ---------------------------------------------------------------------------

// Priorità: NON quelle del progetto principale. Là l'ESP32 ha due core e i task dell'app
// girano su un core diverso da WiFi/lwIP; il C6 ha un solo core, e valori come
// configMAX_PRIORITIES-3 (=22) mettono i task sopra lwIP (18) e alla pari di esp_timer (22),
// togliendo CPU allo stack di rete e alla seriale USB.
// Qui tutti i task stanno in basso (come loop() di Arduino, priorità 1) e la seriale è la più alta
// tra loro, così i comandi hanno sempre la precedenza su joystick e schermo.
#define PRIORITY_SERIAL 5
#define PRIORITY_DELAYED_TASK 4
#define PRIORITY_SERVER 3
#define PRIORITY_JOYSTICK 2 // al posto di PRIORITY_LED_EFFECTS (qui non ci sono strisce LED)

#define STACK_SIZE_SERVER 8192
#define STACK_SIZE_JOYSTICK 6144
#define STACK_SIZE_DELAYED_TASK 4096
#define STACK_SIZE_SERIAL 8192

static const TickType_t TASK_DELAY_TICKS = 10 / portTICK_PERIOD_MS;

// API impostazioni (WebServer) e OTA.
// Durante un upload OTA, otaHandle() resta dentro per tutto il trasferimento tenendo il lock:
// gli altri task aspettano e la schermata di avanzamento non viene sovrascritta.
static void apiServerTask(void *pvParameters)
{
  Serial.println("[Task] apiServerTask avviato");
  while (true)
  {
    takeExclusiveExecution();
    otaHandle();
    server.handleClient();
    freeExclusiveExecution();
    vTaskDelay(TASK_DELAY_TICKS);
  }
}

// Comandi dal monitor seriale (serialCableTask nel progetto principale)
static void serialCableTask(void *pvParameters)
{
  Serial.println("[Task] serialCableTask avviato");
  while (true)
  {
    String msg;
    if (readSerialMessage(msg))
    {
      takeExclusiveExecution();
      executeSerialMessage(msg);
      freeExclusiveExecution();
    }
    vTaskDelay(TASK_DELAY_TICKS);
  }
}

// Comandi ritardati (reboot,ms / change_communication_mode) e riavvio richiesto dall'API
static void commandDelayedTask(void *pvParameters)
{
  Serial.println("[Task] commandDelayedTask avviato");
  while (true)
  {
    takeExclusiveExecution();
    checkDelayedRebootAndExecute();
    if (rebootPending)
    {
      handlePendingReboot();
    }
    freeExclusiveExecution();
    vTaskDelay(TASK_DELAY_TICKS);
  }
}

// Lettura del joystick e schermo (modalità remote_control)
static void joystickTask(void *pvParameters)
{
  Serial.println("[Task] joystickTask avviato");
  while (true)
  {
    takeExclusiveExecution();
    // durante il conto alla rovescia del riavvio lo schermo è di commandDelayedTask
    if (!rebootPending)
    {
      // --- TEST: commenta la riga sotto per disattivare il test a schermo del joystick ---
      // secondo parametro: true = stampa le direzioni sulla seriale, false = solo a schermo
      testJoystickOnDisplay(joystick, false);
    }
    freeExclusiveExecution();
    vTaskDelay(TASK_DELAY_TICKS);
  }
}

static void startTasks()
{
  exclusiveExecutionBegin();

  xTaskCreate(serialCableTask, "SerialCableTaskExecution", STACK_SIZE_SERIAL, NULL, PRIORITY_SERIAL, NULL);
  xTaskCreate(apiServerTask, "ApiServerTaskExecution", STACK_SIZE_SERVER, NULL, PRIORITY_SERVER, NULL);
  xTaskCreate(commandDelayedTask, "CommandDelayedTaskExecution", STACK_SIZE_DELAYED_TASK, NULL, PRIORITY_DELAYED_TASK, NULL);

  switch (deviceMode)
  {
  case DEVICE_MODE::REMOTE_CONTROL:
    if (joystickConfig.enableJoystick)
    {
      xTaskCreate(joystickTask, "JoystickTaskExecution", STACK_SIZE_JOYSTICK, NULL, PRIORITY_JOYSTICK, NULL);
    }
    break;
  default:
    break;
  }

  Serial.println("[Main] Init procedure completed");
}

// Tutto il lavoro è nei task: come nel progetto principale loop() non fa nulla.
// Il task di loop() viene eliminato per non tenere occupato l'unico core del C6.
void loop()
{
  vTaskDelete(NULL);
}
