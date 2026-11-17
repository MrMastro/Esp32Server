#ifndef LOG_H
#define LOG_H

#include <Arduino.h>

// Log sulla seriale, come SerialService del progetto principale:
//  - logInfo*:    solo con "debug": true in main_settings.json (come logInfoln)
//  - logWarning*: sempre (warning ed errori, come logWarning / logError)
// Le risposte ai comandi seriali e la configurazione stampata all'avvio restano Serial.print*: escono sempre.
// Meno stampe = task piu' reattivi (ogni print sulla USB del C6 costa tempo).

// Da chiamare appena caricate le impostazioni (prima vale false: i log informativi non escono)
void logSetDebug(bool enabled);
bool logDebugEnabled();

void logInfo(const String &msg);              // con a capo
void logInfoRaw(const String &msg);           // senza a capo (es. i puntini di attesa)
void logInfof(const char *format, ...);       // come printf

void logWarning(const String &msg);           // sempre
void logWarningf(const char *format, ...);    // sempre, come printf

#endif // LOG_H
