#include "LedService.h"

LedService::LedService(uint16_t countPixels, uint8_t pin)
    : isLedOn(false), isAttachedLed(false), ws2811Step(STEP_LIFE_EFFECT::OFF), ws2811Effect(WS2811_EFFECT::NO_EFFECT) {}

LedService::LedService(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ledStriptInput)
{
  ledStript = ledStriptInput;
}

boolean LedService::isAvaible()
{
  return isAttachedLed;
}

boolean LedService::preparePin()
{
  logInfo("prepare pin of ledService");
  String s = "Pins size: {n}";
  s.replace("{n}", String(pins.size()));
  logInfo(s);
  for (int i = 0; i < pins.size(); ++i)
  {
    s = "Preparing pin: {pin} at position: {i}";
    int pin = pins.at(i);
    s.replace("{pin}", String(pin));
    s.replace("{i}", String(i));
    logInfo(s);
    if (i == 0)
    {
      ledPin = pin;
      pinMode(pin, OUTPUT);
    }
    else if (i == 1)
    {
      ws2811Pin = pin;
      numLeds = 32;
      ledStript->Begin();
      ledStript->Show();
      logInfo("Strip preparated");
      ws2811Step = STEP_LIFE_EFFECT::OFF;
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

void LedService::startEffect(WS2811_EFFECT effect, RgbColor colorRgb, int deltaTms, boolean actionRgbStript, boolean actionWs2811Stript)
{
  if (!isAttachedLed)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "effectPrograssiveBar");
    return;
  }
  String colorString = rgbColorToString(colorRgb);
  String msg = formatMsg("start: {}, colorRgb: {}, deltaTms: {}, actionRgb: {}, actionWs2811: {}",{WS2811EffectEnomToString(effect),colorString,String(deltaTms),String(actionRgbStript),String(actionWs2811Stript)});
  logInfo(msg);
  //todo create status for stript rgb and stript ws2811
  ws2811Step = STEP_LIFE_EFFECT::BEGIN_STEP;
  ws2811Effect = effect;
  deltaTmsEffect = deltaTms;
  colorEffect = colorRgb;
}

void LedService::stopEffect(WS2811_EFFECT effectInput, RgbColor colorRgb, int deltaTms,boolean actionRgb,boolean actionWs2811)
{
  String colorString = formatMsg("[{},{},{}]", {String(colorRgb.R),String(colorRgb.G),String(colorRgb.B)});
  String msg = formatMsg("stop: {}, colorRgb: {}, deltaTms: {}, actionRgb: {}, actionWs2811: {}",{WS2811EffectEnomToString(effectInput),colorString,String(deltaTms),String(actionRgb),String(actionWs2811)});
  logInfo(msg);
  //todo create status for stript rgb and stript ws2811
  ws2811Step = STEP_LIFE_EFFECT::END_STEP;
  ws2811Effect = (effectInput != WS2811_EFFECT::ACTUAL_EFFECT) ? effectInput : ws2811Effect;
  deltaTmsEffect = deltaTms;
  colorEffect = colorRgb;
}

void LedService::runEffectWs2811LifeCycle()
{
  if (!isAttachedLed)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "runEffectWs2811");
    return;
  }

  switch (ws2811Step)
  {
  case STEP_LIFE_EFFECT::BEGIN_STEP:
    ws2811Step = STEP_LIFE_EFFECT::LOOP_STEP;
    execWs2811Effect(ws2811Effect, STEP_LIFE_EFFECT::BEGIN_STEP, colorEffect, deltaTmsEffect);
    break;
  case STEP_LIFE_EFFECT::LOOP_STEP:
    execWs2811Effect(ws2811Effect, STEP_LIFE_EFFECT::LOOP_STEP, colorEffect, deltaTmsEffect);
    break;
  case STEP_LIFE_EFFECT::END_STEP:
    ws2811Step = STEP_LIFE_EFFECT::OFF;
    execWs2811Effect(ws2811Effect, STEP_LIFE_EFFECT::END_STEP, colorEffect, deltaTmsEffect);
    break;
  default: // OFF_STEP
    ledStript->ClearTo(RgbColor(0));
    ledStript->Show();
    break;
  }
}

void LedService::execWs2811Effect(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  String s = "\nPlays\nEffect {},\nStep: {},\nRgbColor: {},\ndeltaTimesMs: {}";
  String sEffect = WS2811EffectEnomToString(ws2811EffectInput);
  String sLifeStep = stepLifeEffectEnomToString(ws2811StepInput);
  String sColor =formatMsg("[ {} , {} , {} ]", {String(colorInput.R), String(colorInput.G), String(colorInput.B)});
  logInfo(formatMsg(s,{sEffect, sLifeStep, sColor, String(deltaTimeMsInput)}));

  // SWITCH ALL EFFECT
  switch (ws2811EffectInput)
  {
  case WS2811_EFFECT::CONSTANT_UNIQUE_COLOR:
    ws2811EffectConstantsUniqueColor(ws2811StepInput, colorInput, deltaTimeMsInput);
    break;
  case WS2811_EFFECT::PROGRESSIVE_BAR_UNIQUE_COLOR:
    ws2811EffectProgressiveBarUniqueColor(ws2811StepInput, colorInput, deltaTimeMsInput);
    break;
  default:
    break;
  }
}

// Function of effect
void LedService::ws2811EffectConstantsUniqueColor(STEP_LIFE_EFFECT ws2811StepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  for (int i = 0; i < ledStript->PixelCount(); i++)
  {
    ledStript->SetPixelColor(i, colorInput);
  }
  ledStript->Show();
}

void LedService::ws2811EffectProgressiveBarUniqueColor(STEP_LIFE_EFFECT ws2811StepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  if (ws2811StepInput == STEP_LIFE_EFFECT::BEGIN_STEP)
  {
    for (int pixel = 0; pixel < ledStript->PixelCount(); pixel++)
    {
      ledStript->SetPixelColor(pixel, colorInput);
      ledStript->Show();
      delay(deltaTimeMsInput);
    }
  }
  else if (ws2811StepInput == STEP_LIFE_EFFECT::LOOP_STEP)
  {
    for (int pixel = 0; pixel < ledStript->PixelCount(); pixel++)
    {
      ledStript->SetPixelColor(pixel, colorInput);
      delay(deltaTimeMsInput);
    }
    ledStript->Show();
  }
  else if (ws2811StepInput == STEP_LIFE_EFFECT::END_STEP)
  {
    for (int light = 255; light == 0; --light)
    {
      RgbColor darken = ledStript->GetPixelColor(0);
      darken.Darken(light);
      ledStript->ClearTo(darken);
      delay(deltaTimeMsInput);
      ledStript->Show();
    }
  }
}