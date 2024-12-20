#include "Main.h"

int ledPin = 2;
SettingsModel s;

AsyncWebServer webServer(80);
BluetoothSerial SerialBT;

// Deprecated: MastroLed myRgbStript; // LEDStripDriver(Din: 19, Cin: 18);
NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip = nullptr;
LEDStripDriver *rgbStrip = nullptr;
DriverLed *myDriver;

CommandService commandService;
SettingService settingService;
LedService ledService;
InfoService infoService;
SerialService serialService;

// AsyncWebServer server(80);
//  ################################################################################ //
//                               Setup and Loop Method                               //
//  ################################################################################ //

// other baud rate: 115200
// baud rate: 9600

void setup_communication(SettingsModel sm)
{
  serialService.logInfoln("setup_communication method", "MAIN");
  boolean unknonwMode = false;
  boolean defaultMode = false;

  // Task for serial COM
  serialService.logInfoln("Create SerialCableTaskExecution for comunicate with serial com", "MAIN");
  xTaskCreate(serialCableTask, "SerialCableTaskExecution", 10000, NULL, 0, NULL);

  // Task for communication
  COMMUNICATION_MODE mode = communicationModeStringToEnum(sm.communicationMode);
  switch (mode)
  {

  case COMMUNICATION_MODE::AP_MODE:
    serialService.logInfoln("Communication mode is AP, init AP web Server", "MAIN");
    mastroServer = MastroServer(&webServer, "AP", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
    initRoutes(mastroServer);
    infoWebServer();
    break;

  case COMMUNICATION_MODE::WIFI_MODE:
    serialService.logInfoln("Communication mode is WIFI, init WIFI web Server", "MAIN");
    mastroServer = MastroServer(&webServer, "WIFI", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
    initRoutes(mastroServer);
    infoWebServer();
    break;

  case COMMUNICATION_MODE::BLUETOOTH_MODE:
    serialService.logInfoln("Communication mode is BLUETOOTH, init Bluetooth and Bluetooth Serial", "MAIN");
    serialService.initSerialBtBegin(sm.deviceName, &SerialBT);
    xTaskCreate(serialBtTask, "SerialBluetoothTaskExecution", 10000, NULL, 1, NULL);
    break;

  // Case WIP or Defaults
  case COMMUNICATION_MODE::UNKNOWN_MODE:
    serialService.logWarning("Communication mode is UNKNOWN", "MAIN", "setup_communication");
    defaultMode = true;
    unknonwMode = true;
    break;
  case COMMUNICATION_MODE::HYBRID_BLUETOOTH_AP:
    serialService.logWarning("Communication mode is HYBRID_BLUETOOTH_AP, this communication is yet as WIP", "MAIN", "setup_communication");
    defaultMode = true;
    unknonwMode = true;
    break;
  case COMMUNICATION_MODE::HYBRID_BLUETOOTH_WIFI:
    serialService.logWarning("Communication mode is HYBRID_BLUETOOTH_WIFI, this communication is yet as WIP", "MAIN", "setup_communication");
    defaultMode = true;
    unknonwMode = true;
    break;
  default:
    defaultMode = true;
    unknonwMode = true;
    break;
  }
  if (defaultMode)
  {
    if (unknonwMode)
    {
      serialService.logWarning("the mode passing is unknown. Loading ap mode as default", "MAIN", "setup_communication");
    }
    else
    {
      serialService.logWarning("Hybrid Communcation isn't yet avaible in this version (WIP). Loading ap mode as default", "MAIN", "setup_communication");
    }
    mastroServer = MastroServer(&webServer, "AP", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
    infoWebServer();
  }
}

void infoWebServer()
{

  if (!mastroServer.isAvaible())
  {
    serialService.logError("Server not initializated", "MAIN", "setup_webServer");
    return;
  }

  Serial.println("\n");
  Serial.println("SERVER MODE: " + mastroServer.getWifiCommunicationMode());
  Serial.println("IP: " + infoService.getIp());
}

void initServices(HardwareSerial *serialPointer)
{
  serialService.attachSerial(&Serial);
  serialService.logInfoln("Service init", "MAIN");
  servicesCollector.addService(&serialService, "SerialService");
  servicesCollector.addService(&settingService, "SettingService");
  settingService.loadSettings(SETTINGS_FILE_LOCATION_PATH);
  s = settingService.getSettings();
  serialService.setSettings(&s);

  // init LedService
  rgbStrip = new LEDStripDriver(s.ledSettings.pinLedDinRgb, s.ledSettings.pinLedCinRgb);
  ws2811Strip = new NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod>(s.ledSettings.numLedWs2811, s.ledSettings.pinLedWs2811);
  myDriver = new DriverLed(ws2811Strip, rgbStrip);
  ledService = LedService(myDriver, s.ledSettings.enableStripRgb, s.ledSettings.enableStripWs2811);

  servicesCollector.attachServer(&mastroServer);
  servicesCollector.addService(&commandService, "CommandService", &s);
  servicesCollector.addService(&ledService, "LedService", &s);
  servicesCollector.addService(&infoService, "InfoService", &s);

  //  Attach pin
  serialService.logInfoln("Attach pin", "MAIN");
  servicesCollector.getService("LedService")->attachPin({ledPin});
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("\n");
  Serial.println("Started");
  delay(10);
  Serial.print("\n");
  initServices(&Serial);
  delay(10);

  Serial.println("");
  Serial.println("Load settings:");
  Serial.println(s.toJson());

  // init communication (Server wifi/ap or Bluetooth)
  setup_communication(s);

  delay(100);

  // Thread running
  if (mastroServer.isAvaible())
  {
    // xTaskCreate(webOtaServerTask, "WebOtaServerTaskExecution", 10000, NULL, 1, NULL); commented, cause instability
  }
  xTaskCreate(ledTask, "LedTaskExecution", 10000, NULL, 3, NULL);
  xTaskCreate(commandDelayedTask, "commandDelayedTaskExecution", 10000, NULL, 4, NULL);
  // vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!

  serialService.logInfoln("Init procedure completed", "MAIN");
}

// loop is used for print debug
void loop(void)
{
}

// Serial bt task check input for bluetooth message
void serialBtTask(void *pvParameters)
{
  serialService.logInfoln("Serial Bluetooth Task execution", "MAIN");
  serialService.logInfoln("Start listining bluetooth serial", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      if (serialService.availableSerialBt())
      {
        String msgBt = serialService.getMsgbyBluetooth();
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
  serialService.logInfoln("Serial Task execution", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      // todo change with if (serialService.availableSerial())
      if (serialService.availableSerial())
      {
        String msg = serialService.getMsgbySerial();
        if (msg.equals("t"))
        {
          test();
        }
        else
        {
          Serial.println("Serial has msg: " + msg);
          recvMsgBySerial(msg);
        }
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
  serialService.logInfoln("Web Task execution (OTA)", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      mastroServer.handleOta();
    }
    else
    {
      yield();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void commandDelayedTask(void *pvParameters)
{
  serialService.logInfoln("Delayed Command Task execution", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      commandService.checkDelyedCmdAndExecute();
    }
    else
    {
      yield();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void ledTask(void *pvParameters)
{
  serialService.logInfoln("Led Task execution", "MAIN");
  String msg = "";

  std::vector<RgbColor> rgbColors = getRgbColorsByRequest(s.initialColors);

  // Initial effect (commented for disable initial effect)
  if (!isPresentEffect(s.initialEffect))
  {
    msg = formatMsg(" {} | time: {} | colors: {}  - None initial effect applied ", {s.initialEffect, vectorRgbColorToString(rgbColors)});
    serialService.logInfoln(msg, "MAIN");
  }
  else
  {
    msg = formatMsg("First effect running: {} | time: {} | colors: {} ", {s.initialEffect, String(s.initialDeltaT), vectorRgbColorToString(rgbColors)});
    serialService.logInfoln(msg, "MAIN");
    ((LedService *)servicesCollector.getService("LedService"))->startEffect(s.initialEffect, rgbColors, s.initialDeltaT, s.ledSettings.enableStripRgb, s.ledSettings.enableStripWs2811);
  }

  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {

      boolean executedCorrectly = false;

      if (s.ledSettings.enableStripRgb)
      {
        executedCorrectly = ((LedService *)servicesCollector.getService("LedService"))->runRgbLifeCycle();
        if(!executedCorrectly){
          delay(TIME_MS_FOR_ERROR_EXECUTION);
        }
      }

      if (s.ledSettings.enableStripWs2811)
      {
        executedCorrectly = ((LedService *)servicesCollector.getService("LedService"))->runWs2811LifeCycle();
        if(!executedCorrectly){
          delay(TIME_MS_FOR_ERROR_EXECUTION);
        }
      }
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