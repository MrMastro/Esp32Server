#include "Main.h"
#include "MastroServer.h"
#include "MastroLed.h"
#include "routes/Routes.h"
#include "utils/SerialSimple.h"
#include "services/Services.h"

// ################################################################################ //
//                            Manage profile settings                               //
// ################################################################################ //
// Decomment the line below for apply default settings                              //                     
// #include "./settings/settingsDefault.h" //      <--- Default settings     //
// Comment the line below for apply default settings                                //
#include "./settings/mySettings.h"//               <--- Custom settings      //
#include "services/CommandService.h"
// ################################################################################ //
//                     End of profile settings management                           //
// ################################################################################ //

const int ledPin = 2;
bool isActiveLed = false;
MastroServer myServer;
MastroLed myRgbStript;
CommandService myCommandManager;

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
  //init services
  myCommandManager = CommandService(&Serial,&WebSerial);
  // Route handling
  initRoutes(myServer);
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
    myCommandManager.recvMsgAndExecute(dataString);
  }
}

void recvMsgBySerial(String data)
{
  myCommandManager.recvMsgAndExecute(data);
}