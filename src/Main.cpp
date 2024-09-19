#include "Main.h"

TaskHandle_t LedTask;
boolean doTest = false;
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

void setup(void)
{
  delay(1000);
  serialService.initSerialBegin(9600);
  serialService.logInfoln("Setup","MAIN");
  delay(10);
  servicesCollector.addService(&serialService, "SerialService");
  servicesCollector.addService(&settingService, "SettingsService");

  settingService.loadSettings("/settings/settings.json");
  s = settingService.getSettings();

  // SerialSerivice init bluetooth with name s.getDevice
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
  Serial.println(((InfoService *)servicesCollector.getService("InfoService"))->getIp());

  // Thread running
  xTaskCreate(ledTask, "LedTaskExecution", 4096, NULL, 1, &LedTask);
  // vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!
}

void loop(void)
{
  mastroServer.handleOta();

  if (!servicesCollector.isBusyForServiceApi())
  {

    // if (serialService.availableSerial())
    // {
    //   String msg = serialService.getMsgbySerial();
    //   Serial.print("Ricevuto:");
    //   Serial.println(msg);
    // }

    // if (Serial.available())
    // {
    //   recvMsgBySerial(Serial.readString());
    //   test();
    // }
  }
  else
  {
    yield();
  }
}

void ledTask(void *pvParameters)
{
  serialService.logInfoln("LedTask Running", "MAIN");
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
  }
}

void test()
{
  serialService.logInfoln("Test", "MAIN");
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