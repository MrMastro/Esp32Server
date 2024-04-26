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
    else
    {
      s = "Pin {pin} have been ignored, this service has already all the necessary pin";
      s.replace("{pin}", String(pin));
      logWarning(s, "preparePin");
    }

    if (rgbStript != nullptr)
    {
      rgbStript->setColor(0, 0, 0);
      rgbStep = STEP_LIFE_EFFECT::OFF;
      logInfo("rgbStript preparated");
    }
    else
    {
      logInfo("rgbStript don't passed, is not prepared");
    }

    if (rgbStript != nullptr)
    {
      ws2811Stript->Begin();
      ws2811Stript->Show();
      ws2811Step = STEP_LIFE_EFFECT::OFF;
      logInfo("ws2811Stript preparated");
    }
    else
    {
      logInfo("2s2811Stript don't passed, is not prepared");
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

void LedService::startEffect(WS2811_EFFECT effectWS2811, RgbColor colorRgb, int deltaTms, boolean actionRgbStript, boolean actionWs2811Stript)
{
  if (!isAttachedLed)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "effectPrograssiveBar");
    return;
  }
  String colorString = rgbColorToString(colorRgb);
  String msg = formatMsg("start: {}, colorRgb: {}, deltaTms: {}, actionRgb: {}, actionWs2811: {}", {WS2811EffectEnomToString(effectWS2811), colorString, String(deltaTms), String(actionRgbStript), String(actionWs2811Stript)});
  logInfo(msg);
  // todo create status for stript rgb and stript ws2811

  if (actionRgbStript)
  {
    rgbStep = STEP_LIFE_EFFECT::BEGIN_STEP;
    rgbEffect = RGB_EFFECT::WAWE_UNIQUE_COLOR; // todo add parameter for effectWS2811 by FE (for now is default)
  }

  if (actionWs2811Stript)
  {
    ws2811Step = STEP_LIFE_EFFECT::BEGIN_STEP;
    ws2811Effect = effectWS2811;
  }

  deltaTmsEffect = deltaTms;
  colorEffect = colorRgb;
}

void LedService::stopEffect(WS2811_EFFECT effectInput, RgbColor colorRgb, int deltaTms, boolean actionRgb, boolean actionWs2811)
{
  String colorString = formatMsg("[{},{},{}]", {String(colorRgb.R), String(colorRgb.G), String(colorRgb.B)});
  String msg = formatMsg("stop: {}, colorRgb: {}, deltaTms: {}, actionRgb: {}, actionWs2811: {}", {WS2811EffectEnomToString(effectInput), colorString, String(deltaTms), String(actionRgb), String(actionWs2811)});
  logInfo(msg);
  // todo create status for stript rgb and stript ws2811

  rgbStep = STEP_LIFE_EFFECT::END_STEP;
  rgbEffect = RGB_EFFECT::WAWE_UNIQUE_COLOR; // todo add parameter for effectWS2811 by FE (for now is default)

  ws2811Step = STEP_LIFE_EFFECT::END_STEP;
  ws2811Effect = (effectInput != WS2811_EFFECT::ACTUAL_EFFECT) ? effectInput : ws2811Effect;

  deltaTmsEffect = deltaTms;
  colorEffect = colorRgb;
}

void LedService::runEffectRgbLifeCycle()
{
  if (!isAttachedLed)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "rgb Stript not attached", "runEffectRgb");
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
    ws2811Stript->ClearTo(RgbColor(0));
    ws2811Stript->Show();
    break;
  }
}

void LedService::execRgbEffect(RGB_EFFECT rgbEffectInput, STEP_LIFE_EFFECT rgbStepInput, RgbColor colorInput, int deltaTimeMsInput)
{
  String s = "\nPlays\nEffect {},\nStep: {},\nRgbColor: {},\ndeltaTimesMs: {}";
  String sEffect = rgbEffectEnomToString(rgbEffectInput);
  String sLifeStep = stepLifeEffectEnomToString(rgbStepInput);
  String sColor = formatMsg("[ {} , {} , {} ]", {String(colorInput.R), String(colorInput.G), String(colorInput.B)});
  logInfo(formatMsg(s, {sEffect, sLifeStep, sColor, String(deltaTimeMsInput)}));

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
  logInfo(formatMsg(s, {sEffect, sLifeStep, sColor, String(deltaTimeMsInput)}));

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