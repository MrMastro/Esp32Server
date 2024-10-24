#include "LiveFlameColorEffect.h"
#include <cstdlib> // Per rand() e srand()
#include <ctime>   // Per time()

LiveFlameColorEffect::LiveFlameColorEffect() : Effect(), currentFlameHeight(0), growthStep(1), growing(true)
{
    srand(static_cast<unsigned int>(time(0)));
}

String LiveFlameColorEffect::getName()
{
    return "LIVE_FLAME_COLOR_EFFECT";
}

int LiveFlameColorEffect::getMaxColorsNumber()
{
    return 5; // Massimo numero di colori per l'effetto fiamma
}

int LiveFlameColorEffect::getMinColorsNumber()
{
    return 1; // Almeno un colore è richiesto
}

boolean LiveFlameColorEffect::getCompatibilityRgb()
{
    return true; // Compatibile con RGB
}

boolean LiveFlameColorEffect::getCompatibilityWs2811()
{
    return true; // Compatibile con WS2811
}

void LiveFlameColorEffect::execStep(String effectInput, STEP_LIFE_LED_EFFECT stepInput, const std::vector<RgbColor> &colorsInput, int deltaTimeMsInput, DriverLed *driver, TYPE_STRIP type, SerialService *serialService)
{
    if (driver == nullptr)
    {
        return;
    }

    if (colorsInput.size() < getMinColorsNumber())
    {
        Serial.println("Errore: la quantità in input non può essere minore della quantità necessaria");
        return;
    }

    switch (stepInput)
    {
    case STEP_LIFE_LED_EFFECT::BEGIN_STEP:
        // Imposta i valori massimi e minimi dell'altezza della fiamma
        maxHeightFlamePercent = 100;         // Modifica questo valore a piacere (percentuale)
        minHeightFlamePercent = 10;          // Modifica questo valore a piacere (percentuale)
        minHeightFlameThresholdPercent = 95; // Imposta la soglia minima per l'altezza della fiamma

        initializeFlame(colorsInput, driver, type);
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::LOOP_STEP:
        updateFlame(driver, type, false);
        driver->showData(); // Mostra i cambiamenti
        delay(deltaTimeMsInput);
        break;

    case STEP_LIFE_LED_EFFECT::END_STEP:
        // Abbassa la fiamma fino a spegnerla
        while (currentFlameHeight > 0)
        {
            updateFlame(driver, type, true); // Aggiorna la fiamma
            driver->showData();              // Mostra i cambiamenti
            currentFlameHeight -= growthStep; // Riduci l'altezza della fiamma
            delay(deltaTimeMsInput);
        }
        off(driver, type); // Spegni tutti i LED
        break;

    case STEP_LIFE_LED_EFFECT::OFF:
        off(driver, type);
        break;

    default:
        break;
    }
}

void LiveFlameColorEffect::initializeFlame(const std::vector<RgbColor> &colorsInput, DriverLed *driver, TYPE_STRIP type)
{
    maxPixels = driver->getMaxNumPixel(type);
    flamePixels.clear();

    for (int i = 0; i < maxPixels; i++)
    {
        RgbColor color = colorsInput[rand() % colorsInput.size()];
        int intensity = rand() % 256; // Intensità casuale
        flamePixels.push_back({color, intensity});
        driver->sendStripData(type, RgbColor(color.R * intensity / 255, color.G * intensity / 255, color.B * intensity / 255), i);
    }

    // Imposta l'altezza iniziale della fiamma in base alla percentuale
    currentFlameHeight = (minHeightFlamePercent * maxPixels) / 100;
    growthStep = 1; // Inizia a crescere
}

void LiveFlameColorEffect::updateFlame(DriverLed *driver, TYPE_STRIP type, boolean consuming)
{
    if (consuming)
    {
        // Ignora le soglie minime se siamo in fase di consumo
        if (currentFlameHeight > 0)
        {
            // Logica per abbassare la fiamma
            currentFlameHeight -= growthStep; // Riduci l'altezza della fiamma
        }
        else
        {
            // Logica per non avere altezze negative, solo nulla
            currentFlameHeight = 0;
        }
    }
    else
    {
        // Normalmente gestiamo la crescita
        if (growing)
        {
            if (currentFlameHeight < (maxHeightFlamePercent * maxPixels) / 100)
            {
                currentFlameHeight += growthStep;
            }
            else
            {
                growing = false; // Inizia a decrescere
            }
        }
        else
        {
            // Limita la diminuzione dell'altezza della fiamma
            if (currentFlameHeight > (minHeightFlameThresholdPercent * maxPixels) / 100)
            {
                currentFlameHeight -= growthStep;
            }
            else
            {
                growing = true; // Inizia a crescere se si raggiunge la soglia minima
            }
        }
    }

    for (int i = 0; i < maxPixels; i++)
    {
        RgbColor baseColor = flamePixels[i].color;

        // Decidi se il pixel deve essere acceso o spento in base all'altezza della fiamma
        if (i < currentFlameHeight)
        {
            RgbColor newColor = varyColor(baseColor);
            flamePixels[i].intensity = rand() % 256; // Intensità casuale per il pixel della fiamma
            driver->sendStripData(type, RgbColor(newColor.R * flamePixels[i].intensity / 255, newColor.G * flamePixels[i].intensity / 255, newColor.B * flamePixels[i].intensity / 255), i);
        }
        else
        {
            // Spegni i pixel al di fuori dell'altezza della fiamma
            driver->sendStripData(type, RgbColor(0, 0, 0), i);
        }
    }
}

RgbColor LiveFlameColorEffect::varyColor(const RgbColor &baseColor)
{
    int variationRange = 10;
    int R = baseColor.R + (rand() % (2 * variationRange) - variationRange);
    int G = baseColor.G + (rand() % (2 * variationRange) - variationRange);
    int B = baseColor.B + (rand() % (2 * variationRange) - variationRange);
    R = constrain(R, 0, 255);
    G = constrain(G, 0, 255);
    B = constrain(B, 0, 255);
    return RgbColor(R, G, B);
}

void LiveFlameColorEffect::off(DriverLed *driver, TYPE_STRIP type)
{
    driver->clear(type);
}