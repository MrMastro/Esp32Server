#include "Main.h"
#include "MastroServer.h"
#include "MastroLed.h"

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

#include "routes/Routes.h"
#include "utils/SerialSimple.h"
#include "./services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/LedService.h"

const int ledPin = 2;
bool isActiveLed = false;
MastroServer myServer;
MastroLed myRgbStript;
ServicesCollector servicesCollector;

// ################################################################################ //
//                              Setup and Loop Method                               //
// ################################################################################ //

void setup(void)
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  //activeLed(false, false); //todo active led
  myServer = MastroServer(wirlessMode, ssid, password, ssidAP, passwordAP, deviceName, devicePassword, ledPin);
  if(myServer.isAvaible()){
    WebSerial.begin(myServer.getWebServer(),"/webConsole");
  }

  //init services and ServiceCollector
  servicesCollector = ServicesCollector(&myServer);
  servicesCollector.addService(std::make_shared<CommandService>());
  servicesCollector.addService(std::make_shared<LedService>());
  servicesCollector.addService(std::make_shared<InfoService>());
  servicesCollector.attachSerial(&Serial,&WebSerial);
  servicesCollector.getService("LedService")->attachPin(ledPin);
 
  // Route handling
  initRoutes(myServer,servicesCollector);
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
    //servicesCollector.executeMethod("LedService","changeLed",simpleBooleanToJson(true));
    servicesCollector.executeMethod("CommandService","recvMsgAndExecute",dataString);
  }
}

void recvMsgBySerial(String data)
{
  servicesCollector.executeMethod("CommandService","recvMsgAndExecute",data);
}