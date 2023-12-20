#include "LedService.h"

LedService::LedService()
{
    isLedOn = false;
    isAttachedLed = false;
}

LedService::LedService(int ledPinInput)
{
    isLedOn = false;
    std::vector<int> pins(1);
    pins.push_back(ledPinInput);
    isAttachedLed = attachPins(pins);
}

boolean LedService::isAvaible()
{
    return isAttachedLed;
}

boolean LedService::attachPins(std::vector<int> pins)
{
  if(pins.size() == 1)
  {
    pinMode(pins.at(0), OUTPUT);
    ledPin = pins.at(0);
    isAttachedLed = true;
  }else{
    isAttachedLed = false;
  }
  return isAttachedLed;
}

String LedService::executeJson(String methodName, String param)
{
  if (methodName == "changeLed")
  {
    boolean active = jsonToSimpleBoolean(param);
    boolean toggle = false;
    return simpleBooleanToJson(changeLed(active,toggle));
  }
  else
  {
    return "Service Method not found";
  }
}

String LedService::executeJson(String methodName, std::vector<String> jsonParams)
{
  if (methodName == "changeLed")
  {
    boolean active = jsonToSimpleBoolean(jsonParams.at(0));
    boolean toggle = jsonToSimpleBoolean(jsonParams.at(1));
    return simpleBooleanToJson(changeLed(active,toggle));
  }
  else
  {
    return "Service Method not found";
  }
}

//Deprecated
// String LedService::executeStringMethod(String nameMethod, String param)
// {
//     return String();
// }

String LedService::getClassName() const
{
    return "LedService";
}

boolean LedService::changeLed(boolean active, boolean toggle)
{
  delay(50);
  if(!isAttachedLed){
    throwError(ERROR_CODE::SERVICE_ERROR, "led not attached");
  }
  if (toggle)
  {
    if (isLedOn)
    {
      digitalWrite(ledPin, LOW);
    }
    else
    {
      digitalWrite(ledPin, HIGH);
    }
    isLedOn = !isLedOn;
  }
  else if (active)
  {
    digitalWrite(ledPin, HIGH); // Accendi il LED
    isLedOn = true;
  }
  else
  {
    digitalWrite(ledPin, LOW); // Spegni il LED
    isLedOn = false;
  }
  delay(100);
  return isLedOn;
}