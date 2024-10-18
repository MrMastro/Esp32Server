#include "FadeEffect.h"

FadeEffect::FadeEffect() : brightness(255), increasing(false) {}  // Inizializza la luminosità massima

String FadeEffect::getName() {
    return "FADE_EFFECT";
}

void FadeEffect::execStep(WS2811_EFFECT ws2811EffectInput, STEP_LIFE_EFFECT ws2811StepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed* driver, TYPE_STRIP type) {
    
    if (driver == nullptr) {
        return;
    }

    switch (ws2811StepInput) {
        case STEP_LIFE_EFFECT::BEGIN_STEP:
            increasing = true;
            brightness = 0;
            // Inizia con la dissolvenza dal nero (tutti spenti)
            for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++) {
                RgbColor color = RgbColor(0, 0, 0);
                driver->sendStriptData(type, color, i);
            }
            driver->showData();
            break;

        case STEP_LIFE_EFFECT::LOOP_STEP:
            // Fai variare la luminosità dei LED
            for (size_t i = 0; i < driver->getMaxNumPixel(type); i++) {
                RgbColor baseColor = colorsInput[i % colorsInput.size()];
                // Applica il fattore di luminosità al colore base
                RgbColor fadedColor = RgbColor(uint8_t(baseColor.R * brightness),
                                               uint8_t(baseColor.G * brightness),
                                               uint8_t(baseColor.B * brightness));
                driver->sendStriptData(type, fadedColor, i);
            }
            driver->showData();

            // Aggiorna il fattore di luminosità
            if (increasing) {
                brightness += (0.01f * deltaTimeMsInput / 1000.0f);  // Aumenta la luminosità
                if (brightness >= 1.0f) {
                    brightness = 1.0f;
                    increasing = false;  // Inizia a decrescere
                }
            } else {
                brightness -= (0.01f * deltaTimeMsInput / 1000.0f);  // Diminuisci la luminosità
                if (brightness <= 0.0f) {
                    brightness = 0.0f;
                    increasing = true;  // Inizia a crescere
                }
            }
            break;

        case STEP_LIFE_EFFECT::END_STEP:
            // Alla fine, imposta tutti i LED su spento
            increasing = false;
            brightness = 0;
            for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++) {
                driver->sendStriptData(type, RgbColor(0, 0, 0), i);
            }
            driver->showData();
            break;

        case STEP_LIFE_EFFECT::OFF:
            off(driver, type);
            break;

        default:
            break;
    }
}

void FadeEffect::off(DriverLed* driver, TYPE_STRIP type) {
    // Spegne tutti i LED
    driver->clear(type);
}