#ifndef SD_STORAGE_H
#define SD_STORAGE_H

#include <Arduino.h>

// Cartella sulla microSD (FAT32) che contiene i JSON di configurazione
#define SD_CONFIG_DIR "/config"
#define SD_MAIN_SETTINGS SD_CONFIG_DIR "/main_settings.json"
#define SD_JOYSTICK_CONFIG SD_CONFIG_DIR "/joystick.json"

// Monta la microSD sul bus SPI condiviso con il display.
// Va chiamata DOPO l'inizializzazione del display (che avvia il bus SPI).
// Non stampa nulla: l'esito descrittivo finisce in 'info' (da stampare dopo le impostazioni).
bool sdBegin(String *info = nullptr);

#endif // SD_STORAGE_H
