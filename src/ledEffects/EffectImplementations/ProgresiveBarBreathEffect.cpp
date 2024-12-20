#include "ProgresiveBarBreathEffect.h"

ProgresiveBarBreathEffect::ProgresiveBarBreathEffect() : brightness(255), increasing(false), minBrightnessThresholdPercent(0), positionProgressBar(0), timeAccumulator(0) {} // Inizializza la luminosità massima

String ProgresiveBarBreathEffect::getName()
{
    return "PROGRESSIVE_BAR_BREATH_EFFECT";
}

int ProgresiveBarBreathEffect::getMaxColorsNumber()
{
    return 5;
}

int ProgresiveBarBreathEffect::getMinColorsNumber()
{
    return 1;
}

boolean ProgresiveBarBreathEffect::getCompatibilityRgb()
{
    return true;
}

boolean ProgresiveBarBreathEffect::getCompatibilityWs2811()
{
    return true;
}

boolean ProgresiveBarBreathEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
{
    if (driver == nullptr)
    {
        return false;
    }

    if (colorsInput.size() < getMinColorsNumber())
    {
        Serial.println("Errore la quantità in input non può essere minore della quantità necessaria");
        return false;
    }

    switch (stepInput)
    {
    case STEP_LIFE_LED_EFFECT::BEGIN_STEP:
    {
        increasing = true;
        brightness = 0;
        minBrightnessThresholdPercent = 25;
        positionProgressBar = 0; // Inizializza la posizione della barra
        timeAccumulator = 0;     // Inizializza l'accumulatore di tempo
        // Inizia con la dissolvenza dal nero (tutti spenti)
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            RgbColor color = RgbColor(0, 0, 0);
            driver->sendStripData(type, color, i);
        }
        driver->showData();
        delay(deltaTimeMsInput);
        break;
    }
    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
    {
        // Accumula il tempo
        timeAccumulator += 100;

        // Calcola il numero massimo di pixel
        int maxPixels = driver->getMaxNumPixel(type);

        // Se l'accumulatore ha superato il tempo per far avanzare la barra di 1 pixel
        if (timeAccumulator >= deltaTimeMsInput)
        {
            timeAccumulator = 0;
            positionProgressBar++;               // Incrementa la barra di un pixel

            // Assicurati che la barra non superi il numero massimo di pixel
            if (positionProgressBar >= maxPixels)
            {
                positionProgressBar = maxPixels; // Blocca al massimo pixel
            }
        }

        // Fai variare la luminosità dei LED con effetto "breath"
        for (size_t i = 0; i < maxPixels; i++)
        {
            RgbColor baseColor = colorsInput[i % colorsInput.size()];
            RgbColor fadedColor = RgbColor(uint8_t(baseColor.R * brightness),
                                           uint8_t(baseColor.G * brightness),
                                           uint8_t(baseColor.B * brightness));
            if (i < positionProgressBar) // Solo i LED fino alla posizione della barra devono essere accesi
            {
                driver->sendStripData(type, fadedColor, i);
            }
            else
            {
                driver->sendStripData(type, RgbColor(0, 0, 0), i); // Spegni il resto dei LED
            }
        }
        driver->showData();

        // Aggiorna il fattore di luminosità (effetto breath)
        if (increasing)
        {
            brightness += (0.01f / (deltaTimeMsInput / 1000.0f));
            if (brightness >= 1.0f)
            {
                brightness = 1.0f;
                increasing = false; // Inizia a decrescere
            }
        }
        else
        {
            brightness -= (0.01f / (deltaTimeMsInput / 1000.0f));
            if (brightness <= minBrightnessThresholdPercent / 100)
            {
                brightness = minBrightnessThresholdPercent / 100;
                increasing = true; // Inizia a crescere di nuovo
            }
        }
        break;
    }
    case STEP_LIFE_LED_EFFECT::END_STEP:
    {
        // Alla fine, imposta tutti i LED su spento
        increasing = false;
        brightness = 0;
        for (uint16_t i = 0; i < driver->getMaxNumPixel(type); i++)
        {
            driver->sendStripData(type, RgbColor(0, 0, 0), i);
        }
        driver->showData();
        break;
    }
    case STEP_LIFE_LED_EFFECT::OFF:
    {
        off(driver, type);
        break;
    }
    default:
    {
        break;
    }
    }

    return true;
}

void ProgresiveBarBreathEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    // Spegne tutti i LED
    driver->clear(type);
}