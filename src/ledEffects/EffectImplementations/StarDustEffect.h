#ifndef STARDUST_H
#define STARDUST_H

#include "../Effect.h"
#include <Arduino.h>

class StarDustEffect : public Effect {
public:
    StarDustEffect();

    String getName();
    int getMaxColorsNumber();
    int getMinColorsNumber();
    boolean getCompatibilityRgb();
    boolean getCompatibilityWs2811();

    void execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService);
    void off(DriverLed *driver, TYPE_STRIP type);

private:
    struct Star {
        RgbColor color;  // Colore base del pixel
        int intensity;   // Intensità corrente del pixel (0-255)
    };

    std::vector<Star> stars;  // Vettore che contiene lo stato di tutti i pixel
    int maxPixels;            // Numero massimo di pixel supportati dalla striscia
    int cycleStep;            // Contatore per la gestione del ciclo di inversione (progressione verso i colori originali)
    bool randomizeCycle;      // Flag per determinare se usare la randomizzazione del ciclo

    int maxBrightPixels;       // Numero massimo di pixel che possono essere molto luminosi
    int currentBrightPixels;
    int maxWhiteBrightnessPercent;    // Massima luminosità bianca raggiungibile (da 0 a 100)

    // Funzioni di supporto
    void initializeStars(const std::vector<RgbColor> &colorsInput, DriverLed *driver, TYPE_STRIP type);  // Inizializza i pixel
    void updateStars(DriverLed *driver, TYPE_STRIP type);  // Aggiorna i pixel ad ogni ciclo

    // Funzioni di gestione del colore
    RgbColor varyColor(const RgbColor &baseColor);  // Varia leggermente il colore in modo casuale ma simile
    RgbColor transitionToLighterColor(const RgbColor &baseColor, float progress);  // Schiarisce il colore mantenendo la tonalità originale
    bool isBright(const RgbColor & color); // Verifica se un colore è troppo chiaro
};
#endif // STARDUST_H