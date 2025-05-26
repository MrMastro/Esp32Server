#ifndef LIVE_FLAME_COLOR_EFFECT_H
#define LIVE_FLAME_COLOR_EFFECT_H

#include "../Effect.h"
#include <vector>
#include <Arduino.h>

class LiveFlameColorEffect : public Effect {
public:
    LiveFlameColorEffect();

    String getName();
    int getMaxColorsNumber();
    int getMinColorsNumber();
    boolean getCompatibilityRgb();
    boolean getCompatibilityWs2811();
    boolean getCompatibilityWs2811Matrix();
    boolean execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService);
    void off(DriverLed *driver, TYPE_STRIP type);

private:
    struct FlamePixel {
        RgbColor color;
        int intensity;
    };

    std::vector<FlamePixel> flamePixels;
    int maxPixels;                       // Numero massimo di pixel nella striscia
    int currentFlameHeight;              // Altezza corrente della fiamma
    int growthStep;                      // Passo di crescita della fiamma
    int maxHeightFlamePercent;           // Massima altezza della fiamma in percentuale
    int minHeightFlamePercent;           // Minima altezza della fiamma in percentuale
    int minHeightFlameThresholdPercent;  // Soglia minima per l'altezza della fiamma in percentuale
    bool growing;                        // Booleano per la crescita o la decrescita

    void initializeFlame(const std::vector<RgbColor> &colorsInput, DriverLed *driver, TYPE_STRIP type);
    void updateFlame(DriverLed *driver, TYPE_STRIP type, boolean consuming);

    RgbColor varyColor(const RgbColor &baseColor);  // Metodo per variare leggermente il colore base
};


#endif // LIVE_FLAME_COLOR_EFFECT_H