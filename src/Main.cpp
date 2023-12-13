#include "Main.h"
#include "MastroServer.h"
#include "MastroLed.h"
#include "utils/SerialSimple.h"
#include "model/Cmd.h"

// ################################################################################ //
//                            Manage profile settings                               //
// ################################################################################ //
// Decomment the line below for apply default settings                              //                     
// #include "./profileSettings/settingsDefault.h"                                   //
// Comment the line below for apply default settings                                //
#include "./profileSettings/mySettings.h"// <--- Custom settings                    //

// ################################################################################ //
//                     End of profile settings management                           //
// ################################################################################ //

const int ledPin = 2;
bool isActiveLed = false;
MastroServer myServer;
MastroLed myRgbStript;

void recvMsg(uint8_t *data, size_t len)
{
  // WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++)
  {
    d += char(data[i]);
  }

  if (d.length() > 0)
  {
    Cmd cmd;
    cmd = mapStringToEnum(d);
    switch (cmd)
    {
    case Cmd::LED_ON:
      activeLed(true, false);
      differentSerialprintln(String("Led on"),&Serial,&WebSerial);
      break;
    case Cmd::LED_OFF:
      activeLed(false, false);
      differentSerialprintln("Led off",&Serial,&WebSerial);
      break;
    case Cmd::LED_TOGGLE:
      activeLed(true, true);
      differentSerialprintln("Led toggle",&Serial,&WebSerial);
      break;
    case Cmd::INFO:
      differentSerialprintln(myServer.getIp());
      break;
    default:
      if(d.toInt()>0)
      {
        myRgbStript.setModeValue(d.toInt());
      }else{
        differentSerialprintln("Tasto non riconosciuto: " + d ,&Serial, &WebSerial);
      }
      
      break;
    }
  }
}

boolean activeLed(bool active, bool toggle)
{
  delay(50);
  if (toggle)
  {
    if (isActiveLed)
    {
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, HIGH);
    }
    isActiveLed = !isActiveLed;
  }
  else if (active)
  {
    digitalWrite(ledPin, HIGH); // Accendi il LED
    isActiveLed = true;
  }
  else
  {
    digitalWrite(ledPin, LOW); // Spegni il LED
    isActiveLed = false;
  }
  delay(100);
  return isActiveLed;
}


// ################################################################################ //
//                               Custom api method                                  //
// ################################################################################ //

// Function to handle /text route
void handleTextRequest(AsyncWebServerRequest *request)
{
  String message = myServer.getOneElementJsonString("Status", "OK");
  request->send(200, "application/json", message);
}

void handleJsonMultiple(AsyncWebServerRequest *request)
{
  // allocate the memory for the document
  DynamicJsonDocument doc(1024);

  String keys[] = {"uno","due","tre"};
  String values[] = {"1","ValoreDue","tre"};

  String json = myServer.getJsonStringByKeysAndValues(keys,values,3);
  request->send(200, "application/json", json);
}

void handleMainLed(AsyncWebServerRequest *request)
{
  // allocate the memory for the document
  DynamicJsonDocument doc(1024);
  bool result = activeLed(true,true);
  String json = myServer.getOneElementJsonString("lightLed", result?"true":"false");
  request->send(200, "application/json", json);
}

// ################################################################################ //
//                              Setup and Loop Method                               //
// ################################################################################ //

void setup(void)
{
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);
  activeLed(false, false);
  myServer = MastroServer(wirlessMode, ssid, password, ssidAP, passwordAP, deviceName, devicePassword, ledPin);
  myRgbStript.setupLedRgb();
  // Route handling
  delay(50);
  myServer.setCustomApi("/try", HTTP_GET, handleTextRequest);
  myServer.setCustomApi("/multipleTry", HTTP_GET, handleJsonMultiple);
  myServer.setCustomApi("/api/handleLed", HTTP_POST, handleMainLed);
}

void loop(void)
{
  myServer.handleOta();
  myRgbStript.loopLedRgb();
  delay(10);
}