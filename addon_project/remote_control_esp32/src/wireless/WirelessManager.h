#ifndef WIRELESS_MANAGER_H
#define WIRELESS_MANAGER_H

#include <Arduino.h>
#include "WirelessConfig.h"

// Gestione del collegamento con i dispositivi (fase 3, docs/WIRELESS_JOYSTICK.md):
//  - task di accoppiamento: accoppia via HTTP i dispositivi messi in coda (uno alla volta, fuori dal lock);
//  - task wireless: heartbeat adattivo a ogni dispositivo accoppiato, HEARTBEAT_ACK (pallino pieno),
//    UNKNOWN_SESSION e heartbeat persi -> riaccoppiamento, riscoperta periodica dei non collegati.
// Richiede wirelessLinkBegin() gia' chiamata.

void wirelessManagerBegin(const WirelessConfig &config);

// Mette in coda l'accoppiamento di tutti i dispositivi di espDevices.json (avvio, comando "pair").
// Restituisce quanti sono stati accodati (quelli gia' in coda non si ripetono).
int wirelessRequestPairAll();

// Stato delle sessioni per la seriale (una riga per sessione)
String wirelessManagerStatus();

// Pressione lunga di SW (fase 5):
//  - tutti i dispositivi collegati (pallini pieni) -> disconnette tutti;
//  - qualcuno non collegato -> accoppia subito solo quelli scollegati (gli altri restano come sono);
//  - disconnessi -> rifa' la connessione (accoppiamento da capo, come all'accensione).
// Non bloccante: il lavoro (DISCONNECT + attesa degli ACK, oppure accoppiamento) lo fa il task wireless.
void wirelessRequestConnectToggle();

// true se disconnesso (o in disconnessione) con la pressione lunga di SW
bool wirelessManagerDisconnected();

// true se la pressione lunga di SW ora collegherebbe (disconnessi, o qualche dispositivo non collegato),
// false se disconnetterebbe (tutti collegati). Per la scritta a schermo. Da chiamare DENTRO il lock.
bool wirelessManagerLongPressConnects();

// longPressS di wireless.json in millisecondi
uint32_t wirelessManagerLongPressMs();

#endif // WIRELESS_MANAGER_H
