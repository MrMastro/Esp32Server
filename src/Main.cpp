#include "Main.h"

// ################################################################################ //
//                            Manage profile settings                               //
// ################################################################################ //
// Decomment the line below for apply default settings                              //
// #include "./settings/settingsDefault.h" //      <--- Default settings     //
// Comment the line below for apply default settings                                //
#include "./settings/mySettings.h" //               <--- Custom settings      //
// ################################################################################ //
//                     End of profile settings management                           //
// ################################################################################ //
TaskHandle_t LedTask;
boolean doTest = false;
int ledPin = 2;
boolean requestInAction = false;
// ################################################################################ //
//                              Setup and Loop Method                               //
// ################################################################################ //

void setup(void)
{
  Serial.begin(9600);
  mastroServer = MastroServer(wirlessMode, ssid, password, ssidAP, passwordAP, deviceName, devicePassword, ledPin);
  if (mastroServer.isAvaible())
  {
    WebSerial.begin(mastroServer.getWebServer(), "/webConsole");
  }
  servicesCollector.attachSerial(&Serial, &WebSerial);
  servicesCollector.attachServer(&mastroServer);
  // init services and ServiceCollector
  // servicesCollector = ServicesCollector(&myServer);

  //  Service init

  logInfo("Service init");
  servicesCollector.addService(&commandService, "CommandService");
  servicesCollector.addService(&ledService, "LedService");
  servicesCollector.addService(&infoService, "InfoService");

  //  Attach pin
  servicesCollector.getService("LedService")->attachPin({ledPin});

  // Route handling
  initRoutes(mastroServer);

  // Other
  WebSerial.msgCallback(recvMsgBySerialWeb);
  myRgbStript.setupLedRgb();

  delay(50);
  logInfo("Init procedure completed");

  // Thread running
  xTaskCreate(ledTask, "LedTaskExecution", 4096, NULL, 1, &LedTask);
  //vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!
  
}

void loop(void)
{
  mastroServer.handleOta();

  if (!servicesCollector.isBusyForServiceApi())
  {
    if (Serial.available())
    {
      recvMsgBySerial(Serial.readString());
    }
  }
  else
  {
    yield();
  }
}

void ledTask(void *pvParameters)
{
  logInfo("LedTask Running");

  //Initial effect (commented for disable initial effect)
  WS2811_EFFECT firstEffect = WS2811EffectStringToEnum(initialEffect);
  ((LedService *) servicesCollector.getService("LedService"))->startEffect(firstEffect,RgbColor(0,0,255),100,true,true);

  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      myRgbStript.loopLedRgb();
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
  logInfo("Test");
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

void logInfo(String msg)
{
  if (DEBUG)
  {
    String log = "[ LOG - MAIN ] {msg}";
    log.replace("{msg}", msg);
    differentSerialprintln(log, "\033[32m", &Serial, &WebSerial);
  }
}