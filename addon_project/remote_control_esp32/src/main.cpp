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
#include <WebServer.h>

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

// Comandi dal monitor seriale (una riga terminata da invio):
//   settings  -> ristampa la configurazione caricata
static void handleSerialCommands()
{
  static String line = "";
  while (Serial.available())
  {
    char c = Serial.read();
    if (c == '\r')
      continue;
    if (c != '\n')
    {
      if (line.length() < 64)
        line += c;
      continue;
    }

    line.trim();
    if (line.equalsIgnoreCase("settings"))
    {
      printSettings();
    }
    else if (line.length() > 0)
    {
      Serial.println("[Serial] comando sconosciuto: " + line + " (disponibili: settings)");
    }
    line = "";
  }
}

void setup()
{
  Serial.begin(115200);
  // Senza monitor seriale aperto, la seriale USB bloccherebbe ogni print fino a ~100ms
  Serial.setTxTimeoutMs(0);

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
}

void loop()
{
  otaHandle();
  server.handleClient();
  handleSerialCommands();

  if (rebootPending)
  {
    handlePendingReboot();
    delay(2);
    return;
  }

  switch (deviceMode)
  {
  case DEVICE_MODE::REMOTE_CONTROL:
    // --- TEST: commenta la riga sotto per disattivare il test a schermo del joystick ---
    // secondo parametro: true = stampa le direzioni sulla seriale, false = solo a schermo
    if (joystickConfig.enableJoystick) testJoystickOnDisplay(joystick, false);
    break;
  default:
    break;
  }

  delay(2);
}
