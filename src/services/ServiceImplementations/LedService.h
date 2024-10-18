#ifndef LedService_H
#define LedService_H

#include <./services/Service.h>
#include <LEDStripDriver.h>
#include <NeoPixelBus.h>
#include "constants/LedEffects.h"
#include "SerialService.h"

// numPixel = 32
// pinStrip = 5

enum class STRIPT_EXECUTION
{
  ALL,
  RGB,
  WS2811
};

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
  void runEffectRgbLifeCycle();
  void runEffectWs2811LifeCycle();
  std::vector<String> getAvaibleEffects();

protected:
  void onInitServiceCollector() override;

private:
  SerialService *serialService;
  boolean isAttachedLed;
  int ledPin;
  bool isLedOn;
  
  //Driver
  LEDStripDriver *rgbStript;
  NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Stript;

  //Effect Parameters: Effect for each stript 
  WS2811_EFFECT ws2811Effect;
  RGB_EFFECT rgbEffect;

  //Effect Parameters: Common parameters
  int deltaTmsEffect;
  RgbColor colorEffect;

  //LifeCycle
  STEP_LIFE_EFFECT ws2811Step;
  STEP_LIFE_EFFECT rgbStep;

  //ExecLifeCycle
  void execRgbEffect(RGB_EFFECT rgbEffectInput, STEP_LIFE_EFFECT rgbStepInput, RgbColor colorInput, int deltaTimeMsInput);
  void execWs2811Effect(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT stepInput, RgbColor colorInput, int deltaTimeMsInput);
  
  // Effect
  void effectConstantsUniqueColor(STRIPT_EXECUTION mode, STEP_LIFE_EFFECT stepInput, RgbColor colorInput, int deltaTimeMsInput);
  void effectWaweUniqueColor(STRIPT_EXECUTION mode, STEP_LIFE_EFFECT stepInput, RgbColor colorInput, int deltaTimeMsInput);
  void effectProgressiveBarUniqueColor(STRIPT_EXECUTION mode, STEP_LIFE_EFFECT stepInput, RgbColor colorInput, int deltaTimeMsInput);

  //Utility
  boolean matchRgbEffect(String effectWS2811);
};

#endif // LedService_H
