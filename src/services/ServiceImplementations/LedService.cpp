#include "LedService.h"

LedService::LedService()
{
  isLedOn = false;
  isAttachedLed = false;
  rgbOrchestrator = EffectOrchestrator();
  ws2811Orchestrator = EffectOrchestrator();
  rgbOrchestratorEnabled = false;
  ws2811OrchestratorEnabled = false;
}

//Create a led service
LedService::LedService(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ledStriptInput, LEDStripDriver *rgbLedStriptInput, boolean enableRgb, boolean enableWs2811)
{
  DriverLed* driver = new DriverLed(ledStriptInput, rgbLedStriptInput);
  rgbOrchestrator = EffectOrchestrator(driver, TYPE_STRIP::RGB);
  ws2811Orchestrator = EffectOrchestrator(driver, TYPE_STRIP::WS2811);
  setOperativeRgbOrchestrator(enableRgb);
  setOperativeWs2811Orchestrator(enableWs2811);
}

boolean LedService::isAvaible()
{
  return isAttachedLed;
}

void LedService::onInitServiceCollector()
{
  serialService = ((SerialService *)servicesCollector.getService("SerialService"));
}

//Prepare pin that you have attached with method attach led, for led service prepare one led. this method is executed when you call attachPin.
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
    isAttachedLed = true;
  }
  serialService->logInfoln("Pin preparated","LedService");
  return isAttachedLed;
}

//Turn on, turn off or toggle the single led
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

void LedService::setOperativeRgbOrchestrator(boolean enable){
  rgbOrchestratorEnabled = enable;
}

void LedService::setOperativeWs2811Orchestrator(boolean enable){
  ws2811OrchestratorEnabled = enable;
}

//Metodo che tramite parametri da le direttive ai due orchestratori su come iniziare un effetto
void LedService::startEffect(EFFECT_LABEL effectInput, RgbColor colorRgb, int deltaTms, boolean actionRgbStript, boolean actionWs2811Stript)
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

  if(actionRgbStript)
  {
    rgbOrchestrator.startEffect(effectInput,{colorRgb},deltaTms);
  }

  if(actionWs2811Stript)
  {
    ws2811Orchestrator.startEffect(effectInput,{colorRgb},deltaTms);
  }
}

//Metodo che tramite parametri da le direttive ai due orchestratori su come finire un effetto
void LedService::stopEffect(EFFECT_LABEL effectInput, RgbColor colorRgb, int deltaTms, boolean actionRgb, boolean actionWs2811)
{
  String colorString = formatMsg("[{},{},{}]", {String(colorRgb.R), String(colorRgb.G), String(colorRgb.B)});
  String effectInputString = WS2811EffectEnomToString(effectInput);
  String msg = formatMsg("stop: {}, colorRgb: {}, deltaTms: {}, actionRgb: {}, actionWs2811: {}", {effectInputString, colorString, String(deltaTms), String(actionRgb), String(actionWs2811)});
  serialService->logInfoln(msg, "LedService");

  if(actionRgb)
  {
    rgbOrchestrator.stopEffect(effectInput,{colorRgb},deltaTms);
  }

  if(actionWs2811)
  {
    ws2811Orchestrator.stopEffect(effectInput,{colorRgb},deltaTms);
  }
}

void LedService::runRgbLifeCycle()
{
  if (!isAttachedLed)
  {
    //todo throwError(ERROR_CODE::SERVICE_ERROR, "rgb Stript not attached", "runEffectRgb");
    serialService->logError("rgb Stript not attached","LedService","runEffectRgb");
    return;
  }

  rgbOrchestrator.runLifeCycle();
}

void LedService::runWs2811LifeCycle()
{
  if (!isAttachedLed)
  {
    //todo throwError(ERROR_CODE::SERVICE_ERROR, "ws2811 Stript not attached", "runEffectWs2811");
    serialService->logError("ws2811 Stript not attached","LedService","runEffectWs2811LifeCycle");
    return;
  }

  ws2811Orchestrator.runLifeCycle();
}

//Return all avaible effect that the orchestrator can play
std::vector<String> LedService::getAvaibleEffects()
{
  return getAllWS2811EffectNames();
}