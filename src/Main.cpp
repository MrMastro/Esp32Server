#include "Main.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

int ledPin = 2;
SettingsModel s;

AsyncWebServer webServer(80);
BluetoothSerial SerialBT;

// Deprecated: MastroLed myRgbStript; // LEDStripDriver(Din: 19, Cin: 18);
NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip = nullptr;
LEDStripDriver *rgbStrip = nullptr;

//CommandService commandService;
//SettingService settingService;
//LedService ledService;
//InfoService infoService;
//SerialService serialService;

// AsyncWebServer server(80);
//  ################################################################################ //
//                               Setup and Loop Method                               //
//  ################################################################################ //

// other baud rate: 115200
// baud rate: 9600


// void tryFunction(void)
// {
//   serialService.initSerialBegin(9600);
//   servicesCollector.addService(&settingService, "SettingsService");
//   settingService.loadSettings("/settings/settings.json");
//   s = settingService.getSettings();
//   serialService.initSerialBtBegin(s.deviceName, &SerialBT);
//   servicesCollector.attachSerial(&Serial, &WebSerial);
//   servicesCollector.addService(&commandService, "CommandService", &s);
//   servicesCollector.addService(&infoService, "InfoService", &s);
// }

void setup(void)
{
  Serial.begin(9600);
  //serialService.attachSerial(&Serial);
  //servicesCollector.addService(&serialService, "SerialService");

  delay(10);
  s = SettingsModel().getDefault();
  // servicesCollector.addService(&settingService, "SettingsService");
  // settingService.loadSettings("/settings/settings.json");
  // s = settingService.getSettings();
  // serialService.setSettings(&s);

  // checkCoexistance();

  Serial.println("");
  Serial.println("Load settings:");
  Serial.println(s.toJson());

  //----------------
  // mastroServer = MastroServer(&webServer, s.wirelessMode, s.ssidWIFI, s.passwordWIFI, s.ssidAP, s.passwordAP, s.deviceName, s.devicePassword, s.debug, ledPin);
  // mastroServer.getWebServer()->begin();
  initWifi("AP",s.ssidAP,s.passwordAP);
  initWebServer();
  //----------------

  delay(100);

  // init blutooth--------------
  //serialService.initSerialBtBegin(s.deviceName, &SerialBT);
  // Avvia Bluetooth seriale
  if (!SerialBT.begin(s.deviceName))
  { // Imposta il nome del dispositivo Bluetooth
    Serial.println("An error occurred initializing Bluetooth");
  }
  else
  {
    Serial.println("Bluetooth Serial started");
  }
  //---------------------------

  // init LedService
  if (s.ledSettings.enableStripRgb)
  {
    rgbStrip = new LEDStripDriver(s.ledSettings.pinLedDinRgb, s.ledSettings.pinLedCinRgb);
  }
  if (s.ledSettings.enableStripWs2811)
  {
    ws2811Strip = new NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod>(s.ledSettings.numLedWs2811, s.ledSettings.pinLedWs2811);
  }

  ///---
  // ledService = LedService(ws2811Strip, rgbStrip);

  // servicesCollector.attachSerial(&Serial, &WebSerial);
  // servicesCollector.attachServer(&mastroServer);

  // serialService.logInfoln("Service init", "MAIN");
  // servicesCollector.addService(&commandService, "CommandService", &s);
  // servicesCollector.addService(&ledService, "LedService", &s);
  // servicesCollector.addService(&infoService, "InfoService", &s);

  // //  Attach pin
  // serialService.logInfoln("Attach pin", "MAIN");
  // servicesCollector.getService("LedService")->attachPin({ledPin});

  // // Route handling
  // serialService.logInfoln("Route handling", "MAIN");
  // initRoutes(mastroServer);

  // // Other
  // serialService.logInfoln("Attach web serial", "MAIN");
  // // WebSerial.msgCallback(recvMsgBySerialWeb); todo
  // //  myRgbStript.setupLedRgb(); deprecated MastroLed

  // delay(50);
  // serialService.logInfoln("Init procedure completed", "MAIN");
  // Serial.println("\n");
  // Serial.println("IP");
  // Serial.println(infoService.getIp());

  // // Thread running
  // xTaskCreate(webOtaServerTask, "WebOtaServerTaskExecution", 10000, NULL, 1, NULL);
  // // xTaskCreate(serialBtTask, "SerialBluetoothTaskExecution", 10000, NULL, 0, NULL);
  xTaskCreate(serialCableTask, "SerialCableTaskExecution", 10000, NULL, 2, NULL);
  // xTaskCreate(ledTask, "LedTaskExecution", 10000, NULL, 3, NULL);

  // // xTaskCreate(serialBtTask, "SerialBluetoothTaskExecution", 4096, NULL, 1, NULL); <-backup
  // // vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!

  // ///
}

// loop is used for print debug
void loop(void)
{
  // delay(100);
  // if (s.debug)
  // {
  //   if (!serialService.getLastSentMsg().equals("[ LOG - MAIN (info task) ] Info"))
  //   {
  //     serialService.logInfoln("Info", "MAIN (info task)");
  //   }
  // }
  // yield();
  // vTaskDelay(10 / portTICK_PERIOD_MS);
}

// Serial bt task check input for bluetooth message
void serialBtTask(void *pvParameters)
{
  // //todoserialService.logInfoln("Serial Bluetooth Task execution", "MAIN");
  // // serialService.initSerialBtBegin(s.deviceName, &SerialBT);
  // serialService.logInfoln("Start listining bluethoot serial", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      //todo change in serial service
      if (SerialBT.available())
      {
        //todo change btSerial
        //String msgBt = serialService.getMsgbyBluetooth();
        String msgBt = SerialBT.readString();
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
  // while (true)
  // {
  //   if (!servicesCollector.isBusyForServiceApi())
  //   {
  //     if (serialService.availableSerial())
  //     {
  //       String msg = serialService.getMsgbySerial();
  //       if (msg.equals("t"))
  //       {
  //         test();
  //       }
  //       else
  //       {
  //         Serial.println("Serial has msg: " + msg);
  //         recvMsgBySerial(msg);
  //       }
  //     }
  //   }
  //   else
  //   {
  //     yield();
  //   }
  //   vTaskDelay(10 / portTICK_PERIOD_MS);
  // }
}

void webOtaServerTask(void *pvParameters)
{
  // serialService.logInfoln("Web Task execution", "MAIN");
  // while (true)
  // {
  //   // mastroServer.handleOta();
  //   vTaskDelay(10 / portTICK_PERIOD_MS);
  // }
}

void ledTask(void *pvParameters)
{
  // serialService.logInfoln("Led Task execution", "MAIN");
  // String msg = "";
  // // Initial effect (commented for disable initial effect)
  // WS2811_EFFECT firstEffect = WS2811EffectStringToEnum(s.initialEffect);
  // String firstEffectString = WS2811EffectEnomToString(firstEffect);

  // switch (firstEffect)
  // {
  // case WS2811_EFFECT::NO_EFFECT:
  // case WS2811_EFFECT::UKNOWN_EFFECT:
  // case WS2811_EFFECT::ACTUAL_EFFECT:
  //   msg = formatMsg(" {} | time: {} | R: {} | G: {} | B: {}  - None initial effect applied ", {firstEffectString, String(s.initialDeltaT), String(s.initialR), String(s.initialG), String(s.initialB)});
  //   serialService.logInfoln(msg, "MAIN");
  //   break;

  // default:
  //   msg = formatMsg("First effect running: {} | time: {} | R: {} | G: {} | B: {} ", {firstEffectString, String(s.initialDeltaT), String(s.initialR), String(s.initialG), String(s.initialB)});
  //   serialService.logInfoln(msg, "MAIN");
  //   ((LedService *)servicesCollector.getService("LedService"))->startEffect(firstEffect, RgbColor(s.initialR, s.initialG, s.initialB), s.initialDeltaT, true, true);
  //   break;
  // }

  // while (true)
  // {
  //   if (!servicesCollector.isBusyForServiceApi())
  //   {
  //     // myRgbStript.loopLedRgb(); deprecated
  //     delay(10);
  //     ((LedService *)servicesCollector.getService("LedService"))->runEffectWs2811LifeCycle();
  //     //((LedService *)servicesCollector.getService("LedService"))->runEffectRgbLifeCycle(); //for now don't play rgb stript
  //   }
  //   else
  //   {
  //     yield();
  //   }

  //   vTaskDelay(10 / portTICK_PERIOD_MS);
  // }
}

void test()
{
  // serialService.logInfoln("Test", "MAIN");
  // String st = "name of device: " + s.deviceName;
  // serialService.logInfoln(st, "MAIN");
  // Serial.println("Free heap first Bluetooth: " + String(ESP.getFreeHeap()));
  // serialService.initSerialBtBegin(s.deviceName, &SerialBT);
  // serialService.logInfoln("create task bluetooth", "MAIN");
  // // Serial.println("Free heap after Bluetooth: " + String(ESP.getFreeHeap()));
  // xTaskCreate(serialBtTask, "SerialServerTaskExecution", 8192, NULL, 1, NULL);
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

void checkCoexistance()
{
// #ifdef CONFIG_BT_BLUETOOTH_COEXIST_ENABLE
// #if CONFIG_BT_BLUETOOTH_COEXIST_ENABLE == 1
//   serialService.logInfoln("Bluetooth/WiFi coexistence is enabled.", "MAIN");
// #else
//   serialService.logInfoln("Bluetooth/WiFi coexistence is defined but not enabled.", "MAIN");
// #endif
// #else
//   serialService.logInfoln("Bluetooth/WiFi coexistence is not defined.", "MAIN");
// #endif
}

////////////////////
void initAP(String ssid, String password)
{
  String ip;
  Serial.println("init AP mode");
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(ssid, password);
  // dnsServer.start(53, "*", WiFi.softAPIP());
  //  Print the AP IP address to the serial monitor
  Serial.println("AP IP address: ");
  ip = WiFi.softAPIP().toString();
  Serial.println(ip);
}

void initWifi(String mode, String ssid, String password)
{
  if (mode == "wifi")
  {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    String ip = WiFi.localIP().toString();
    Serial.println(ip);
  }
  else
  {
    initAP("esp32WIfi", password);
  }
}

void initWebServer()
{
  // Configura il web server
  webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              //
              request->send(200, "text/html", "ESP32 Web Server + Bluetooth Active!");
              //
            });
  webServer.begin();
  Serial.println("HTTP server started");
}