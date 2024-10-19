#ifndef LedService_H
#define LedService_H

#include <./services/Service.h>
#include <LEDStripDriver.h>
#include <NeoPixelBus.h>
#include "constants/LedEffects.h"
#include "SerialService.h"
#include "./effects/Effect.h"
#include <effects/EffectOrchestrator.h>

// numPixel = 32
// pinStrip = 5

class LedService : public Service
{
public:
  LedService();
  LedService(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ledStriptInput, LEDStripDriver *rgbLedStriptInput, boolean enableRgb, boolean enableWs2811);
  LedService(DriverLed* driverInput, boolean enableRgb, boolean enableWs2811);
  boolean isAvaible() override;
  boolean preparePin();
  boolean changeLed(boolean active, boolean toggle);

  void setOperativeRgbOrchestrator(boolean enable);
  void setOperativeWs2811Orchestrator(boolean enable);
  boolean isOperativeRgbOrchestrator();
  boolean isOperative2811Orchestrator();


  // Service Methods
  void startEffect(String effect, RgbColor colorRgb, int deltaTmsInput, boolean actionRgbStript, boolean actionWs2811Stript);
  void stopEffect(String effect, RgbColor colorRgb, int deltaTms, boolean actionRgbStript, boolean actionWs2811Stript);
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
