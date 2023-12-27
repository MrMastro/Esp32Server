#include "LedService.h"

// Create a CRGB array to store the color of each LED
// CRGB leds[NUM_LEDS]; fastLEd

LedService::LedService()
{
  isLedOn = false;
  isAttachedLed = false;
}

LedService::LedService(int ledPinInput)
{
  logInfo("Attache led...");
  isLedOn = false;
  isAttachedLed = attachPin(ledPinInput);
  logInfo("Attached");
}

boolean LedService::isAvaible()
{
  return isAttachedLed;
}

boolean LedService::preparePin()
{
  logInfo("prepare pin of ledService");
  for (int i = 0; i < pins.size(); ++i)
  {
    String s = "Preparing pin: {pin}";
    int pin = pins.at(i);
    s.replace("{pin}", String(pin));
    if (i == 0)
    {
      ledPin = pin;
      pinMode(pin, OUTPUT);
    }
    else if (i == 1)
    {
      ws2811Pin = pin;
      numLeds = 32;
      ledStript = Adafruit_NeoPixel(numLeds, ws2811Pin, NEO_BRG + NEO_KHZ800);
      pinMode(pin, OUTPUT);
    }
    else
    {
      s = "Pin {pin} have been ignored, this service has already all the necessary pin";
      s.replace("{pin}", String(pin));
      logWarning(s, "preparePin");
    }
    isAttachedLed = true;
  }
  logInfo("Pin preparated");
  return isAttachedLed;
}

String LedService::executeJson(String methodName, String param)
{
  if (methodName == "changeLed")
  {
    boolean active = jsonToSimpleBoolean(param);
    boolean toggle = false;
    return simpleBooleanToJson(changeLed(active, toggle));
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
    return simpleBooleanToJson(changeLed(active, toggle));
  }
  else
  {
    return "Service Method not found";
  }
}

boolean LedService::changeLed(boolean active, boolean toggle)
{
  delay(50);
  if (!isAttachedLed)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "led not attached", "changeLed");
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
    digitalWrite(ledPin, HIGH);
    isLedOn = true;
  }
  else
  {
    digitalWrite(ledPin, LOW);
    isLedOn = false;
  }
  delay(100);
  return isLedOn;
}

void LedService::effectPrograssiveBar(uint32_t colorRgb, int deltaTms)
{
  if (!isAttachedLed)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "effectPrograssiveBar");
  }
  for (int pixel = 0; pixel < numLeds; pixel++)
  {
    ledStript.setPixelColor(pixel, colorRgb);
    ledStript.show();

    delay(deltaTms);
  }
}

void LedService::shotdownLed()
{
  if (!isAttachedLed)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "shotdownLed");
  }
  ledStript.clear();
  ledStript.show();
}
