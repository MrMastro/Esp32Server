#ifndef LedService_H
#define LedService_H

#include <services/Service.h>
#include <LEDStripDriver.h>
#include <NeoPixelBus.h>
#include "constants/LedEffects.h"
#include "SerialService.h"
#include "ledEffects/Effect.h"
#include "ledEffects/EffectOrchestrator.h"

// numPixel = 32
// pinStrip = 5

class LedService : public Service
{
public:
  LedService();
  //LedService(NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ledStriptInput, LEDStripDriver *rgbLedStriptInput, boolean enableRgb, boolean enableWs2811);
  LedService(DriverLed* driverInput, boolean enableRgb, boolean enableWs2811, boolean enableWs2811Matrix);
  boolean isAvaible() override;
  boolean preparePin();
  boolean changeLed(boolean active, boolean toggle);

  void setOperativeRgbOrchestrator(boolean enable);
  void setOperativeWs2811Orchestrator(boolean enable);
  void setOperativeWs2811MatrixOrchestrator(boolean enable);
  
  boolean isOperativeRgbOrchestrator();
  boolean isOperative2811Orchestrator();
  boolean isOperative2811MatrixOrchestrator();


  // Service Methods
  void startEffect(String effect, std::vector<RgbColor> colorsRgb, int deltaTmsInput, boolean actionRgbStript, boolean actionWs2811Stript, boolean actionWs2811Matrix);
  void stopEffect(String effectInput, RgbColor colorRgb, int deltaTms, boolean actionRgb, boolean actionWs2811, boolean actionWs2811Matrix);
  void startEffect(String effectInput, RgbColor colorRgb, int deltaTms, boolean actionRgbStript, boolean actionWs2811Stript, boolean actionWs2811Matrix);
  void stopEffect(String effect, std::vector<RgbColor> colorsRgb, int deltaTmsInput, boolean actionRgbStript, boolean actionWs2811Stript, boolean actionWs2811Matrix);
  boolean runRgbLifeCycle();
  boolean runWs2811LifeCycle();
  boolean runWs2811MatrixLifeCycle();
  std::vector<Effect*> getAvaibleEffects();

protected:
  void onInitServiceCollector() override;

private:
  SerialService *serialService;
  boolean isAttachedLed;
  int ledPin;
  bool isLedOn;

  EffectOrchestrator rgbOrchestrator;
  EffectOrchestrator ws2811Orchestrator;
  EffectOrchestrator ws2811MatrixOrchestrator;
};

#endif // LedService_H
