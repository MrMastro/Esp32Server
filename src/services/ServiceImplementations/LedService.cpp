#include "LedService.h"

LedService::LedService()
{
  isLedOn = false;
  isAttachedLed = false;
  ws2811Step = STEP_LIFE_EFFECT::OFF;
  ws2811Effect = WS2811_EFFECT::NO_EFFECT;
}

// deprecated
LedService::LedService(uint16_t countPixels, uint8_t pin)
{
  isLedOn = false;
  isAttachedLed = false;
  ws2811Step = STEP_LIFE_EFFECT::OFF;
  ws2811Effect = WS2811_EFFECT::NO_EFFECT;
}

LedService::LedService(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ledStriptInput, LEDStripDriver *rgbLedStriptInput)
{
  ws2811Stript = ledStriptInput;
  rgbStript = rgbLedStriptInput;
}

boolean LedService::isAvaible()
{
  return isAttachedLed;
}

void LedService::onInitServiceCollector()
{
  serialService = ((SerialService *)servicesCollector.getService("SerialService"));
}

boolean LedService::preparePin()
{
  serialService->logInfoln("prepare pin of ledService","LedService");
  String s = "Pins size: {n}";
  s.replace("{n}", String(pins.size()));
  serialService->logInfoln(s,"LedService");
  for (int i = 0; i < pins.size(); ++i)
  {
    s = "Preparing pin: {pin} at position: {i}";
    int pin = pins.at(i);
    s.replace("{pin}", String(pin));
    s.replace("{i}", String(i));
    serialService->logInfoln(s,"LedService");
    if (i == 0)
    {
      ledPin = pin;
      pinMode(pin, OUTPUT);
    }
    else
    {
      s = "Pin {pin} have been ignored, this service has already all the necessary pin";
      s.replace("{pin}", String(pin));
      serialService->logWarning(s,"LedService", "preparePin");
    }

    if (rgbStript != nullptr)
    {
      rgbStript->setColor(0, 0, 0);
      rgbStep = STEP_LIFE_EFFECT::OFF;
      serialService->logInfoln("rgbStript preparated","LedService");
    }
    else
    {
      serialService->logInfoln("rgbStript don't passed, is not prepared","LedService");
    }

    if (ws2811Stript != nullptr)
    {
      ws2811Stript->Begin();
      ws2811Stript->Show();
      ws2811Step = STEP_LIFE_EFFECT::OFF;
      serialService->logInfoln("ws2811Stript preparated","LedService");
    }
    else
    {
      serialService->logInfoln("ws2811Stript don't passed, is not prepared","LedService");
    }

    isAttachedLed = true;
  }
  serialService->logInfoln("Pin preparated","LedService");
  return isAttachedLed;
}

boolean LedService::changeLed(boolean active, boolean toggle)
{
  delay(50);
  if (!isAttachedLed)
  {
    //todo throwError(ERROR_CODE::SERVICE_ERROR, "led not attached", "changeLed");
    serialService->logError("Led not attached","LedService","changeLed");
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

void LedService::startEffect(WS2811_EFFECT effectInput, RgbColor colorRgb, int deltaTms, boolean actionRgbStript, boolean actionWs2811Stript)
{
  if (!isAttachedLed)
  {
    //todo throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "effectPrograssiveBar");3
    serialService->logError("ws2811 Stript not attached","LedService","changeLed");
    return;
  }
  String colorString = rgbColorToString(colorRgb);
  String effectInputString = WS2811EffectEnomToString(effectInput);
  String msg = formatMsg("start: {}, colorRgb: {}, deltaTms: {}, actionRgb: {}, actionWs2811: {}", {effectInputString, colorString, String(deltaTms), String(actionRgbStript), String(actionWs2811Stript)});
  serialService->logInfoln(msg,"LedService");
  // todo create status for stript rgb and stript ws2811

  if (actionRgbStript && matchRgbEffect(effectInputString))
  {
    rgbStep = STEP_LIFE_EFFECT::BEGIN_STEP;
    rgbEffect = rgbEffectStringToEnum(effectInputString);
  }

  if (actionWs2811Stript)
  {
    ws2811Step = STEP_LIFE_EFFECT::BEGIN_STEP;
    ws2811Effect = effectInput;
  }

  deltaTmsEffect = deltaTms;
  colorEffect = colorRgb;
}

boolean LedService::matchRgbEffect(String effect)
{
  boolean match = false;
  for (std::pair<RGB_EFFECT, String> pair : EFFECT_RGB_PAIR)
  {
    if (effect == pair.second)
    {
      match = true;
      break;
    }
  }
  return match;
}

void LedService::stopEffect(WS2811_EFFECT effectInput, RgbColor colorRgb, int deltaTms, boolean actionRgb, boolean actionWs2811)
{
  String colorString = formatMsg("[{},{},{}]", {String(colorRgb.R), String(colorRgb.G), String(colorRgb.B)});
  String effectInputString = WS2811EffectEnomToString(effectInput);
  String msg = formatMsg("stop: {}, colorRgb: {}, deltaTms: {}, actionRgb: {}, actionWs2811: {}", {effectInputString, colorString, String(deltaTms), String(actionRgb), String(actionWs2811)});
  serialService->logInfoln(msg, "LedService");
  // todo create status for stript rgb and stript ws2811

  if (actionRgb && matchRgbEffect(effectInputString))
  {
    rgbStep = STEP_LIFE_EFFECT::END_STEP;
    rgbEffect = rgbEffectStringToEnum(effectInputString);
  }

  ws2811Step = STEP_LIFE_EFFECT::END_STEP;
  ws2811Effect = (effectInput != WS2811_EFFECT::ACTUAL_EFFECT) ? effectInput : ws2811Effect;

  deltaTmsEffect = deltaTms;
  colorEffect = colorRgb;
}

void LedService::runEffectRgbLifeCycle()
{
  if (!isAttachedLed)
  {
    //todo throwError(ERROR_CODE::SERVICE_ERROR, "rgb Stript not attached", "runEffectRgb");
    serialService->logError("rgb Stript not attached","LedService","runEffectRgb");
    return;
  }

  switch (ws2811Step)
  {
  case STEP_LIFE_EFFECT::BEGIN_STEP:
    rgbStep = STEP_LIFE_EFFECT::LOOP_STEP;
    execRgbEffect(rgbEffect, STEP_LIFE_EFFECT::BEGIN_STEP, colorEffect, deltaTmsEffect);
    break;
  case STEP_LIFE_EFFECT::LOOP_STEP:
    execRgbEffect(rgbEffect, STEP_LIFE_EFFECT::LOOP_STEP, colorEffect, deltaTmsEffect);
    break;
  case STEP_LIFE_EFFECT::END_STEP:
    rgbStep = STEP_LIFE_EFFECT::OFF;
    execRgbEffect(rgbEffect, STEP_LIFE_EFFECT::END_STEP, colorEffect, deltaTmsEffect);
    break;
  default: // OFF_STEP
    rgbStript->setColor(0, 0, 0);
    break;
  }
}

void LedService::runEffectWs2811LifeCycle()
{
  if (!isAttachedLed)
  {
    //todo throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "runEffectWs2811");
    serialService->logError("ws2811 Stript not attached","LedService","runEffectWs2811LifeCycle");
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
    ws2811Stript->ClearTo(RgbColor(0));
    ws2811Stript->Show();
    break;
  }
}

std::vector<String> LedService::getAvaibleEffects()
{
  return getAllWS2811EffectNames();
}

void LedService::execRgbEffect(RGB_EFFECT rgbEffectInput, STEP_LIFE_EFFECT rgbStepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  String s = "\nPlays\nEffect {},\nStep: {},\nRgbColor: {},\ndeltaTimesMs: {}";
  String sEffect = rgbEffectEnomToString(rgbEffectInput);
  String sLifeStep = stepLifeEffectEnomToString(rgbStepInput);
  String sColor = formatMsg("[ {} , {} , {} ]", {String(colorInput.R), String(colorInput.G), String(colorInput.B)});
  serialService->logInfoln(formatMsg(s, {sEffect, sLifeStep, sColor, String(deltaTimeMsInput)}),"execRgbEffect");

  // SWITCH ALL EFFECT
  switch (rgbEffectInput)
  {
  case RGB_EFFECT::CONSTANT_UNIQUE_COLOR:
    effectConstantsUniqueColor(STRIPT_EXECUTION::RGB, rgbStepInput, colorInput, deltaTimeMsInput);
    break;
  case RGB_EFFECT::WAWE_UNIQUE_COLOR:
    effectWaweUniqueColor(STRIPT_EXECUTION::RGB, rgbStepInput, colorInput, deltaTimeMsInput);
    break;
  default:
    break;
  }
}

void LedService::execWs2811Effect(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  String s = "\nPlays\nEffect {},\nStep: {},\nRgbColor: {},\ndeltaTimesMs: {}";
  String sEffect = WS2811EffectEnomToString(ws2811EffectInput);
  String sLifeStep = stepLifeEffectEnomToString(ws2811StepInput);
  String sColor = formatMsg("[ {} , {} , {} ]", {String(colorInput.R), String(colorInput.G), String(colorInput.B)});
  serialService->logInfoln(formatMsg(s, {sEffect, sLifeStep, sColor, String(deltaTimeMsInput)}),"execWs2811Effect");

  // SWITCH ALL EFFECT
  switch (ws2811EffectInput)
  {
  case WS2811_EFFECT::CONSTANT_UNIQUE_COLOR:
    effectConstantsUniqueColor(STRIPT_EXECUTION::WS2811, ws2811StepInput, colorInput, deltaTimeMsInput);
    break;
  case WS2811_EFFECT::PROGRESSIVE_BAR_UNIQUE_COLOR:
    effectProgressiveBarUniqueColor(STRIPT_EXECUTION::WS2811, ws2811StepInput, colorInput, deltaTimeMsInput);
    break;
  default:
    break;
  }
}

// Function of effectWS2811
void LedService::effectConstantsUniqueColor(STRIPT_EXECUTION stript, STEP_LIFE_EFFECT stepInput, RgbColor colorInput, int deltaTimeMsInput)
{

  // rgb Stript
  if (rgbStript != nullptr && (stript == STRIPT_EXECUTION::RGB || stript == STRIPT_EXECUTION::ALL))
  {
    rgbStript->setColor(colorInput.R, colorInput.G, colorInput.B);
  }

  if (ws2811Stript != nullptr && (stript == STRIPT_EXECUTION::WS2811 || stript == STRIPT_EXECUTION::ALL))
  {
    // ws2811 stript
    for (int i = 0; i < ws2811Stript->PixelCount(); i++)
    {
      ws2811Stript->SetPixelColor(i, colorInput);
    }
    ws2811Stript->Show();
  }
}

void LedService::effectWaweUniqueColor(STRIPT_EXECUTION stript, STEP_LIFE_EFFECT stepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  RgbColor off = RgbColor(0);
  RgbColor color = colorInput;
  while (color.CalculateBrightness() > 0)
  {
    color.Darken(10);
    // rgb stript
    if (rgbStript != nullptr && (stript == STRIPT_EXECUTION::RGB || stript == STRIPT_EXECUTION::ALL))
    {
      rgbStript->setColor(color.R, color.G, color.B);
    }

    // Ws2811 Stript
    if (ws2811Stript != nullptr && (stript == STRIPT_EXECUTION::WS2811 || stript == STRIPT_EXECUTION::ALL))
    {
      for (int i = 0; i < ws2811Stript->PixelCount(); i++)
      {
        ws2811Stript->SetPixelColor(i, color);
      }
      ws2811Stript->Show();
    }

    // delay delta
    delay(deltaTimeMsInput);
  }

  while (color.CalculateBrightness() < colorInput.CalculateBrightness())
  {
    color.Lighten(10);
    // rgb stript
    if (rgbStript != nullptr && (stript == STRIPT_EXECUTION::RGB || stript == STRIPT_EXECUTION::ALL))
    {
      rgbStript->setColor(color.R, color.G, color.B);
    }

    // Ws2811 Stript
    if (ws2811Stript != nullptr && (stript == STRIPT_EXECUTION::WS2811 || stript == STRIPT_EXECUTION::ALL))
    {
      for (int i = 0; i < ws2811Stript->PixelCount(); i++)
      {
        ws2811Stript->SetPixelColor(i, color);
      }
      ws2811Stript->Show();
    }

    // delay delta
    delay(deltaTimeMsInput);
  }
}

void LedService::effectProgressiveBarUniqueColor(STRIPT_EXECUTION stript, STEP_LIFE_EFFECT stepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  // rgb stript not supported

  // ws2811 Stript
  if (ws2811Stript != nullptr && (stript == STRIPT_EXECUTION::WS2811 || stript == STRIPT_EXECUTION::ALL))
  {
    if (stepInput == STEP_LIFE_EFFECT::BEGIN_STEP)
    {
      for (int pixel = 0; pixel < ws2811Stript->PixelCount(); pixel++)
      {
        ws2811Stript->SetPixelColor(pixel, colorInput);
        ws2811Stript->Show();
        delay(deltaTimeMsInput);
      }
    }
    else if (stepInput == STEP_LIFE_EFFECT::LOOP_STEP)
    {
      for (int pixel = 0; pixel < ws2811Stript->PixelCount(); pixel++)
      {
        ws2811Stript->SetPixelColor(pixel, colorInput);
        delay(deltaTimeMsInput);
      }
      ws2811Stript->Show();
    }
    else if (stepInput == STEP_LIFE_EFFECT::END_STEP)
    {
      for (int light = 255; light == 0; --light)
      {
        RgbColor darken = ws2811Stript->GetPixelColor(0);
        darken.Darken(light);
        ws2811Stript->ClearTo(darken);
        delay(deltaTimeMsInput);
        ws2811Stript->Show();
      }
    }
  }
}