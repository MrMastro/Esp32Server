#ifndef LedService_H
#define LedService_H

#include <./services/Service.h>
#include <LEDStripDriver.h>
#include <NeoPixelBus.h>
#include "constants/LedEffects.h"
#include "SerialService.h"
#include "./effects/Effect.h"

// numPixel = 32
// pinStrip = 5

class LedService : public Service
{
public:
  LedService();
  LedService(uint16_t countPixels, uint8_t pin);
  LedService(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ledStriptInput, LEDStripDriver *rgbLedStriptInput);
  boolean isAvaible() override;
  boolean preparePin();
  boolean changeLed(boolean active, boolean toggle);
  // Service Methods
  void startEffect(WS2811_EFFECT effect, RgbColor colorRgb, int deltaTmsInput, boolean actionRgbStript, boolean actionWs2811Stript);
  void stopEffect(WS2811_EFFECT effect, RgbColor colorRgb, int deltaTms, boolean actionRgbStript, boolean actionWs2811Stript);
  void runRgbLifeCycle();
  void runWs2811LifeCycle();
  std::vector<String> getAvaibleEffects();

protected:
  void onInitServiceCollector() override;

private:
  SerialService *serialService;
  boolean isAttachedLed;
  int ledPin;
  bool isLedOn;

  EffectOrchestrator rgbOrchestrator;
  EffectOrchestrator ws2811Orchestrator;
};

#endif // LedService_H
