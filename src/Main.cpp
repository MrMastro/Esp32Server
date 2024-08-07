#include "Main.h"

TaskHandle_t LedTask;
boolean doTest = false;
int ledPin = 2;
SettingsModel s;

// ################################################################################ //
//                              Setup and Loop Method                               //
// ################################################################################ //

void setup(void)
{
  Serial.begin(9600);
  delay(10);
  servicesCollector.addService(&settingService, "SettingsService");
  settingService.loadSettings("/settings/settings.json");
  s = settingService.getSettings();
  mastroServer = MastroServer(s.wirelessMode, s.ssidWIFI, s.passwordWIFI, s.ssidAP, s.passwordAP, s.deviceName, s.devicePassword, s.debug, ledPin);
  if (mastroServer.isAvaible())
  {
    WebSerial.begin(mastroServer.getWebServer(), "/webConsole");
  }
  servicesCollector.attachSerial(&Serial, &WebSerial);
  servicesCollector.attachServer(&mastroServer);

  logInfoln("Service init");
  servicesCollector.addService(&commandService, "CommandService");
  servicesCollector.addService(&ledService, "LedService");
  servicesCollector.addService(&infoService, "InfoService");

  //  Attach pin
  servicesCollector.getService("LedService")->attachPin({ledPin});

  // Route handling
  initRoutes(mastroServer);

  // Other
  WebSerial.msgCallback(recvMsgBySerialWeb);
  // myRgbStript.setupLedRgb(); deprecated MastroLed

  delay(50);
  logInfoln("Init procedure completed");

  Serial.println("IP");
  Serial.println(((InfoService *)servicesCollector.getService("LedService"))->getIp());

  // Thread running
  xTaskCreate(ledTask, "LedTaskExecution", 4096, NULL, 1, &LedTask);
  // vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!
}

void loop(void)
{
  mastroServer.handleOta();

  if (!servicesCollector.isBusyForServiceApi())
  {
    if (Serial.available())
    {
      recvMsgBySerial(Serial.readString());
      test();
    }
  }
  else
  {
    yield();
  }
}

void ledTask(void *pvParameters)
{
  logInfoln("LedTask Running");

  // Initial effect (commented for disable initial effect)
  WS2811_EFFECT firstEffect = WS2811EffectStringToEnum(s.initialEffect);
  String firstEffectString = WS2811EffectEnomToString(firstEffect);

  switch (firstEffect)
  {
  case WS2811_EFFECT::NO_EFFECT:
  case WS2811_EFFECT::UKNOWN_EFFECT:
  case WS2811_EFFECT::ACTUAL_EFFECT:
    logInfoln(firstEffectString + " - None initial effect applied");
    break;

  default:
    logInfoln("First effect running: " + firstEffectString);
    ((LedService *)servicesCollector.getService("LedService"))->startEffect(firstEffect, RgbColor(0, 0, 255), 100, true, true);
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
  }
}

void test()
{
  logInfoln("Test");
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

void logInfoln(String msg)
{
  if (s.debug)
  {
    String log = "[ LOG - MAIN ] {msg}";
    log.replace("{msg}", msg);
    differentSerialprintln(log, "\033[32m", &Serial, &WebSerial);
  }
}