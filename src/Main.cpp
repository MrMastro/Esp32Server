#include "Main.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

TaskHandle_t LedTask;
TaskHandle_t WebOtaServerTask;
TaskHandle_t SerialCableTask;
TaskHandle_t SerialBtTask;
int ledPin = 2;
SettingsModel s;

// Deprecated: MastroLed myRgbStript; // LEDStripDriver(Din: 19, Cin: 18);
NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip = nullptr;
LEDStripDriver *rgbStrip = nullptr;

CommandService commandService;
SettingService settingService;
LedService ledService;
InfoService infoService;
SerialService serialService;

// ################################################################################ //
//                              Setup and Loop Method                               //
// ################################################################################ //

// other baud rate: 115200
// baud rate: 9600

BluetoothSerial SerialBT;

void tryFunction(void)
{
  serialService.initSerialBegin(9600);
  servicesCollector.addService(&settingService, "SettingsService");
  settingService.loadSettings("/settings/settings.json");
  s = settingService.getSettings();
  serialService.initSerialBtBegin(s.deviceName);
  servicesCollector.attachSerial(&Serial, &WebSerial);
  servicesCollector.addService(&commandService, "CommandService", &s);
  servicesCollector.addService(&infoService, "InfoService", &s);
}

void setup(void)
{
  serialService.initSerialBegin(9600);
  servicesCollector.addService(&serialService, "SerialService");
  delay(10);
  servicesCollector.addService(&settingService, "SettingsService");
  settingService.loadSettings("/settings/settings.json");
  s = settingService.getSettings();
  serialService.setSettings(&s);

  Serial.println("");
  Serial.println("Load settings:");
  Serial.println(s.toJson());

  // init LedService
  if (s.ledSettings.enableStripRgb)
  {
    rgbStrip = new LEDStripDriver(s.ledSettings.pinLedDinRgb, s.ledSettings.pinLedCinRgb);
  }
  if (s.ledSettings.enableStripWs2811)
  {
    ws2811Strip = new NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod>(s.ledSettings.numLedWs2811, s.ledSettings.pinLedWs2811);
  }
  ledService = LedService(ws2811Strip, rgbStrip);
  //

  mastroServer = MastroServer(s.wirelessMode, s.ssidWIFI, s.passwordWIFI, s.ssidAP, s.passwordAP, s.deviceName, s.devicePassword, s.debug, ledPin);
  if (mastroServer.isAvaible())
  {
    WebSerial.begin(mastroServer.getWebServer(), "/webConsole");
  }

  servicesCollector.attachSerial(&Serial, &WebSerial);
  servicesCollector.attachServer(&mastroServer);

  serialService.logInfoln("Service init", "MAIN");
  servicesCollector.addService(&commandService, "CommandService", &s);
  servicesCollector.addService(&ledService, "LedService", &s);
  servicesCollector.addService(&infoService, "InfoService", &s);

  //  Attach pin
  serialService.logInfoln("Attach pin", "MAIN");
  servicesCollector.getService("LedService")->attachPin({ledPin});

  // Route handling
  serialService.logInfoln("Route handling", "MAIN");
  initRoutes(mastroServer);

  // Other
  serialService.logInfoln("Attach web serial", "MAIN");
  WebSerial.msgCallback(recvMsgBySerialWeb);
  // myRgbStript.setupLedRgb(); deprecated MastroLed

  delay(50);
  serialService.logInfoln("Init procedure completed", "MAIN");
  Serial.println("\n");
  Serial.println("IP");
  Serial.println(infoService.getIp());

  // Thread running
  xTaskCreate(ledTask, "LedTaskExecution", 4096, NULL, 1, &LedTask);
  xTaskCreate(webOtaServerTask, "WebServerTaskExecution", 4096, NULL, 1, &WebOtaServerTask);
  xTaskCreate(serialCableTask, "SerialCableTaskExecution", 4096, NULL, 1, &SerialCableTask);
  // xTaskCreate(serialTask, "SerialServerTaskExecution", 4096, NULL, 1, &SerialTask);

  // vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!
}

// loop is used for print debug
void loop(void)
{
    if(s.debug)
    {
      delay(1000);
      serialService.logInfoln("Info","MAIN (info task)");
    }
    yield();
}

// Serial bt task check input for bluetooth message
void serialBtTask(void *pvParameters)
{
  serialService.logInfoln("Serial Task execution", "MAIN");
  serialService.initSerialBtBegin(s.deviceName);
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      if (serialService.availableSerialBt())
      {
        String msgBt = serialService.getMsgbyBluetooth();
        Serial.println("bluetooth Serial has msg: " + msgBt);
        recvMsgBySerial(msgBt);
      }
    }
    else
    {
      yield();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Serial Cable task check input for usb Serial message
void serialCableTask(void *pvParameters)
{
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      if (serialService.availableSerial())
      {
        String msg = serialService.getMsgbySerial();
        Serial.println("Serial has msg: " + msg);
        recvMsgBySerial(msg);
      }
    }
    else
    {
      yield();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void webOtaServerTask(void *pvParameters)
{
  serialService.logInfoln("Web Task execution", "MAIN");
  while (true)
  {
    mastroServer.handleOta();
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void ledTask(void *pvParameters)
{
  serialService.logInfoln("Led Task execution", "MAIN");
  String msg = "";
  // Initial effect (commented for disable initial effect)
  WS2811_EFFECT firstEffect = WS2811EffectStringToEnum(s.initialEffect);
  String firstEffectString = WS2811EffectEnomToString(firstEffect);

  switch (firstEffect)
  {
  case WS2811_EFFECT::NO_EFFECT:
  case WS2811_EFFECT::UKNOWN_EFFECT:
  case WS2811_EFFECT::ACTUAL_EFFECT:
    msg = formatMsg(" {} | time: {} | R: {} | G: {} | B: {}  - None initial effect applied ", {firstEffectString, String(s.initialDeltaT), String(s.initialR), String(s.initialG), String(s.initialB)});
    serialService.logInfoln(msg, "MAIN");
    break;

  default:
    msg = formatMsg("First effect running: {} | time: {} | R: {} | G: {} | B: {} ", {firstEffectString, String(s.initialDeltaT), String(s.initialR), String(s.initialG), String(s.initialB)});
    serialService.logInfoln(msg, "MAIN");
    ((LedService *)servicesCollector.getService("LedService"))->startEffect(firstEffect, RgbColor(s.initialR, s.initialG, s.initialB), s.initialDeltaT, true, true);
    break;
  }

  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      // myRgbStript.loopLedRgb(); deprecated
      delay(10);
      ((LedService *)servicesCollector.getService("LedService"))->runEffectWs2811LifeCycle();
      //((LedService *)servicesCollector.getService("LedService"))->runEffectRgbLifeCycle(); //for now don't play rgb stript
    }
    else
    {
      yield();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void test()
{
  serialService.logInfoln("Test", "MAIN");
  String st = "name of device: " + s.deviceName;
  serialService.logInfoln(st, "MAIN");
  serialService.initSerialBtBegin(s.deviceName);
}

void recvMsgBySerialWeb(uint8_t *data, size_t len)
{
  String dataString = "";
  for (int i = 0; i < len; i++)
  {
    dataString += char(data[i]);
  }
  if (dataString.length() > 0)
  {
    ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(dataString);
  }
}

void recvMsgBySerial(String data)
{
  ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(data);
}