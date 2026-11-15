#include <Arduino.h>

#include "BoardPins.h"
#include "display/Display.h"
#include "storage/SdStorage.h"
#include "settings/SettingsModel.h"
#include "settings/DeviceMode.h"
#include "network/WifiConnection.h"
#include "joystick/Joystick.h"
#include "joystick/JoystickConfig.h"
#include "test/JoystickDisplayTest.h"

static SettingsModel settings;
static DEVICE_MODE deviceMode = DEVICE_MODE::REMOTE_CONTROL;
static WifiStatus wifiStatus;

static JoystickConfig joystickConfig;
static Joystick joystick;

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

void setup()
{
  Serial.begin(115200);
  // Senza monitor seriale aperto, la seriale USB bloccherebbe ogni print fino a ~100ms
  Serial.setTxTimeoutMs(0);

  // 1) Display: avvia anche il bus SPI condiviso con la SD
  displayBegin();
  displayMessage("Avvio...");

  // 2) microSD + impostazioni generali (default se SD o file mancano)
  bool sdAvailable = sdBegin();
  bool settingsFromFile = false;
  settings = SettingsModel::load(SD_MAIN_SETTINGS, sdAvailable, &settingsFromFile);
  if (settings.debug)
  {
    settings.print();
  }
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

  // 4) Modalità del dispositivo
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

  testJoystickSetNetworkInfo((wifiStatus.activeMode == COMMUNICATION_MODE::WIFI_MODE ? "WIFI " : "AP ") + wifiStatus.ip);
}

void loop()
{
  switch (deviceMode)
  {
  case DEVICE_MODE::REMOTE_CONTROL:
    // --- TEST: commenta la riga sotto per disattivare il test a schermo del joystick ---
    if (joystickConfig.enableJoystick) testJoystickOnDisplay(joystick);
    break;
  default:
    break;
  }

  delay(2);
}
