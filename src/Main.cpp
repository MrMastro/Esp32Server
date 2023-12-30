// todo NeoPixelBus
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
  myServer = MastroServer(wirlessMode, ssid, password, ssidAP, passwordAP, deviceName, devicePassword, ledPin);
  if (myServer.isAvaible())
  {
    WebSerial.begin(myServer.getWebServer(), "/webConsole");
  }
  servicesCollector.attachSerial(&Serial, &WebSerial);
  servicesCollector.attachServer(&myServer);
  // init services and ServiceCollector
  // servicesCollector = ServicesCollector(&myServer);

  //  Service init
  servicesCollector.addService(&commandService, "CommandService");
  servicesCollector.addService(&ledService, "LedService");
  servicesCollector.addService(&infoService, "InfoService");

  //  Attach pin
  servicesCollector.getService("LedService")->attachPin({2, 5});

  // Route handling
  initRoutes(myServer);

  // Other
  WebSerial.msgCallback(recvMsgBySerialWeb);
  myRgbStript.setupLedRgb();

  delay(50);

  // Thread running
  xTaskCreate(ledTask, "LedTaskExecution", 4096, NULL, 1, &LedTask);
  vTaskStartScheduler(); // Start the FreeRTOS scheduler
  
}

void loop(void)
{
  myServer.handleOta();

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

  ((LedService *) servicesCollector.getService("LedService"))->startEffect(WS2811_EFFECT::PROGRESSIVE_BAR_UNIQUE_COLOR,RgbColor(0,0,255),100);

  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      myRgbStript.loopLedRgb();
      delay(10);
      ((LedService *)servicesCollector.getService("LedService"))->runEffectWs2811LifeCycle();
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
    differentSerialprintln(log, "", &Serial, &WebSerial);
  }
}