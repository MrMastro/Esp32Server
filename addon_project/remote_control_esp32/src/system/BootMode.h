#ifndef BOOT_MODE_H
#define BOOT_MODE_H

// Modalità di avvio "una tantum" salvata nella flash (NVS / Preferences).
// Il comando seriale "microsd" chiede la modalità SD per il PROSSIMO avvio; all'avvio la richiesta
// viene letta e subito cancellata, quindi qualsiasi riavvio successivo (anche staccando la corrente)
// torna alla modalità normale.

void bootModeRequestSdMode();

// true se era stata richiesta la modalità SD; cancella comunque la richiesta.
bool bootModeConsumeSdMode();

#endif // BOOT_MODE_H
