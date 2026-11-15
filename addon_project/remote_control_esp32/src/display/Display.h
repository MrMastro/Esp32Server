#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// 'gfx' è un buffer in RAM (canvas 240x240): si disegna lì, poi displayFlush()
// manda l'intero frame al display in un solo trasferimento SPI (veloce, senza sfarfallio).
extern Arduino_GFX *gfx;

// Inizializza il display, avvia il bus SPI condiviso e accende la retroilluminazione.
bool displayBegin();

// Invia il contenuto del buffer al display.
void displayFlush();

// Messaggio centrato a schermo intero (boot, errori...), già inviato al display.
void displayMessage(const String &title, const String &subtitle = "");

// Scrive 'text' centrato orizzontalmente alla riga y.
void displayPrintCentered(const String &text, int16_t y, uint8_t size, uint16_t color);

#endif // DISPLAY_H
