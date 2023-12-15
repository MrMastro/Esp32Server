#include "LedService.h"
#include <exceptions/exceptions.h>

LedService::LedService()
{
    isLedOn = false;
    isAttachedLed = false;
}

LedService::LedService(int ledPinInput)
{
    isLedOn = false;
    isAttachedLed = attach(ledPinInput);
    if(isAttachedLed)
        ledPin = ledPinInput;
}

boolean LedService::attach(int ledPin)
{
    pinMode(ledPin, OUTPUT);
    return true;
}

boolean LedService::changeLed(bool active, bool toggle)
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