#include "Main.h"
#include "MastroServer.h"
#include "MastroLed.h"

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

enum Cmd
{
  LED_ON,
  LED_OFF,
  LED_TOGGLE,
  INFO,
  OTHER
};

// Funzione per mappare una stringa a un valore enumerativo
Cmd mapStringToEnum(String inputString)
{
  inputString.toUpperCase();
  if (inputString.equalsIgnoreCase("led on"))
  {
    return LED_ON;
  }
  else if (inputString.equalsIgnoreCase("led off"))
  {
    return LED_OFF;
  }
  else if (inputString.equalsIgnoreCase("led toggle"))
  {
    return LED_TOGGLE;
  }
  else if (inputString.equalsIgnoreCase("info"))
  {
    return INFO;
  }
  else
  {
    // Valore predefinito in caso di stringa non riconosciuta
    return OTHER; // Puoi scegliere un valore predefinito diverso se preferisci
  }
}

void recvMsg(uint8_t *data, size_t len)
{
  // WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++)
  {
    d += char(data[i]);
  }
  // WebSerial.println(d.length());
  if (d.length() > 0)
  {
    Cmd cmd;
    cmd = mapStringToEnum(d);
    switch (cmd)
    {
    case LED_ON:
      activeLed(true, false);
      println(String("Led on"));
      break;
    case LED_OFF:
      activeLed(false, false);
      println("Led off");
      break;
    case LED_TOGGLE:
      activeLed(true, true);
      println("Led toggle");
      break;
    case INFO:
      println(myServer.getIp());
      break;
    default:
      if(d.toInt()>0)
      {
        myRgbStript.setModeValue(d.toInt());
      }else{
        println("Tasto non riconosciuto: " + d);
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

void println(String msg)
{
  WebSerial.println(msg);
}

// Function to handle /text route
void handleTextRequest(AsyncWebServerRequest *request)
{
  String message = "Hello, this is a text for try api!";
  request->send(200, "text/plain", message);
}

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
}

void loop(void)
{
  myServer.handleOta();
  myRgbStript.loopLedRgb();
  delay(10);
}

//todo set custom api
/* 

    webServer.on("/api/handleLed", HTTP_POST, [](AsyncWebServerRequest *request)
                 {
                    bool result = activeLed(true,true);
                    String msg = result ? "{\"lightLed\": true}": "{\"lightLed\": false}";
                    
    request->send(200, "application/json", msg); });

*/
