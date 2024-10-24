#include "StarDustEffect.h"

#include <cstdlib> // Per rand() e srand()
#include <ctime>   // Per time()

StarDustEffect::StarDustEffect() : cycleStep(0), randomizeCycle(false), currentBrightPixels(0) {
    srand(static_cast<unsigned int>(time(0)));
}

String StarDustEffect::getName() {
    return "STARDUST_EFFECT";
}

int StarDustEffect::getMaxColorsNumber() {
    return 5;
}

int StarDustEffect::getMinColorsNumber() {
    return 1;
}

boolean StarDustEffect::getCompatibilityRgb() {
    return false;
}

boolean StarDustEffect::getCompatibilityWs2811() {
    return true;
}

void StarDustEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService) {
    if (driver == nullptr) {
        return;
    }

    if (colorsInput.size() < getMinColorsNumber()) {
        Serial.println("Errore: la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    switch (stepInput) {
    case STEP_LIFE_LED_EFFECT::BEGIN_STEP:
        initializeStars(colorsInput, driver, type);
        cycleStep = 0;  // Reset del ciclo alla fase iniziale
        randomizeCycle = (rand() % 2 == 0);  // Attiva o meno il ciclo randomico
        maxBrightPixels = rand() % (maxPixels / 2) + 1;  // Inizializza il numero massimo di pixel luminosi in modo dinamico
        currentBrightPixels = 0;  // Reset del conteggio dei pixel luminosi
        maxWhiteBrightnessPercent = 70;  // Imposta il valore massimo per la luminosità bianca
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
        updateStars(driver, type);
        driver->showData();  // Mostra i cambiamenti
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::END_STEP:
        off(driver, type);
        break;

    case STEP_LIFE_LED_EFFECT::OFF:
        off(driver, type);
        break;

    default:
        break;
    }
}

void StarDustEffect::initializeStars(const std::vector<RgbColor> &colorsInput, DriverLed *driver, TYPE_STRIP type) {
    maxPixels = driver->getMaxNumPixel(type);
    stars.clear();

    for (int i = 0; i < maxPixels; i++) {
        RgbColor color = colorsInput[rand() % colorsInput.size()];
        int intensity = rand() % 256;  // Intensità casuale
        stars.push_back({color, intensity});
        driver->sendStripData(type, RgbColor(color.R * intensity / 255, color.G * intensity / 255, color.B * intensity / 255), i);
    }
}

void StarDustEffect::updateStars(DriverLed *driver, TYPE_STRIP type) {
    float progress = (cycleStep % 100) / 100.0;

    currentBrightPixels = 0;  // Reset del conteggio dei pixel luminosi

    for (int i = 0; i < stars.size(); i++) {
        RgbColor baseColor = stars[i].color;

        if (currentBrightPixels < maxBrightPixels) {
            RgbColor newColor = transitionToLighterColor(baseColor, progress);
            if (isBright(newColor)) {
                currentBrightPixels++;  // Incrementa il conteggio dei pixel luminosi
            }
            stars[i].intensity = rand() % 256;
            driver->sendStripData(type, RgbColor(
                (newColor.R * stars[i].intensity / 255) * (maxWhiteBrightnessPercent / 100.0), 
                (newColor.G * stars[i].intensity / 255) * (maxWhiteBrightnessPercent / 100.0), 
                (newColor.B * stars[i].intensity / 255) * (maxWhiteBrightnessPercent / 100.0)), i);
        } else {
            // Mantiene il colore originale se il limite di pixel luminosi è stato raggiunto
            stars[i].intensity = rand() % 256;
            driver->sendStripData(type, RgbColor(
                baseColor.R * stars[i].intensity / 255, 
                baseColor.G * stars[i].intensity / 255, 
                baseColor.B * stars[i].intensity / 255), i);
        }
    }

    if (cycleStep >= 100) {
        cycleStep = 0;  // Reset del ciclo
        randomizeCycle = (rand() % 2 == 0);  // Randomizza la transizione
    } else {
        cycleStep++;
    }
}

RgbColor StarDustEffect::transitionToLighterColor(const RgbColor &baseColor, float progress) {
    int maxLuminance = 220;
    int R = baseColor.R + progress * (maxLuminance - baseColor.R);
    int G = baseColor.G + progress * (maxLuminance - baseColor.G);
    int B = baseColor.B + progress * (maxLuminance - baseColor.B);
    return RgbColor(R, G, B);
}

bool StarDustEffect::isBright(const RgbColor &color) {
    return (color.R > 200 && color.G > 200 && color.B > 200);
}

void StarDustEffect::off(DriverLed *driver, TYPE_STRIP type) {
    driver->clear(type);
}

RgbColor StarDustEffect::varyColor(const RgbColor &baseColor) {
    int variationRange = 10;
    int R = baseColor.R + (rand() % (2 * variationRange) - variationRange);
    int G = baseColor.G + (rand() % (2 * variationRange) - variationRange);
    int B = baseColor.B + (rand() % (2 * variationRange) - variationRange);
    R = constrain(R, 0, 255);
    G = constrain(G, 0, 255);
    B = constrain(B, 0, 255);
    return RgbColor(R, G, B);
}


// RgbColor StarDustEffect::varyColor(const RgbColor &baseColor) {
//     // Funzione per variare leggermente i colori, mantenendo variazioni simili
//     int variationRange = 10; // Il range massimo di variazione per ciascun canale RGB

//     int R = baseColor.R + (rand() % (2 * variationRange) - variationRange); // Variazione da -10 a +10
//     int G = baseColor.G + (rand() % (2 * variationRange) - variationRange);
//     int B = baseColor.B + (rand() % (2 * variationRange) - variationRange);

//     // Clamp dei valori per assicurarsi che rimangano validi (0-255)
//     R = constrain(R, 0, 255);
//     G = constrain(G, 0, 255);
//     B = constrain(B, 0, 255);

//     // Assicura che il colore risultante rimanga vicino a quello originale (niente variazioni estreme)
//     return RgbColor(R, G, B);
// }