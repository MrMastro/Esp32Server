#include "Main.h"

// ################################################################################ //
//                            Manage profile settings                               //
// ################################################################################ //
// Decomment the line below for apply default settings                              //                     
// #include "./settings/settingsDefault.h" //      <--- Default settings     //
// Comment the line below for apply default settings                                //
#include "./settings/mySettings.h"//               <--- Custom settings      //
// ################################################################################ //
//                     End of profile settings management                           //
// ################################################################################ //


int ledPin = 2;
// ################################################################################ //
//                              Setup and Loop Method                               //
// ################################################################################ //

void test()
{
  logInfo("Begin Test");
  servicesCollector.executeMethod("CommandService","recvMsgAndExecute","test");
}

void setup(void)
{
  Serial.begin(9600);
  //activeLed(false, false); //todo active led
  myServer = MastroServer(wirlessMode, ssid, password, ssidAP, passwordAP, deviceName, devicePassword, ledPin);
  if(myServer.isAvaible()){
    WebSerial.begin(myServer.getWebServer(),"/webConsole");
  }
  servicesCollector.attachSerial(&Serial,&WebSerial);
  servicesCollector.attachServer(&myServer);
  //init services and ServiceCollector
  //servicesCollector = ServicesCollector(&myServer);
  // Service init
  servicesCollector.addService(&commandService,"CommandService");
  servicesCollector.addService(&ledService,"LedService");
  //servicesCollector.addService(&infoService,"InfoService");
  // Attach pin
  servicesCollector.getService("LedService")->attachPin({2,5});
  
  // Route handling
  initRoutes(myServer);
  
  // Other
  WebSerial.msgCallback(recvMsgBySerialWeb);
  myRgbStript.setupLedRgb();
  delay(50);
}

void loop(void)
{
  myServer.handleOta();
  myRgbStript.loopLedRgb();
  delay(10);
  if(Serial.available()){
    recvMsgBySerial(Serial.readString());
  }
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
    servicesCollector.executeMethod("CommandService","recvMsgAndExecute",dataString);
  }
}

void recvMsgBySerial(String data)
{
  servicesCollector.executeMethod("CommandService","recvMsgAndExecute",data);
}

void logInfo(String msg)
{
  String log = "[ LOG - MAIN ] {msg}";
  log.replace("{msg}", msg);
  differentSerialprintln(log, "", &Serial, &WebSerial);
}