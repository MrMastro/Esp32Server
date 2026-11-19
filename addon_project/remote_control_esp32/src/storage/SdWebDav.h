#ifndef SD_WEBDAV_H
#define SD_WEBDAV_H

#include <WebServer.h>

// Server WebDAV minimale sulla microSD (radice della SD = radice WebDAV), pensato per
// "Connetti unità di rete" di Esplora risorse di Windows (http://<ip>/).
// Metodi: OPTIONS, PROPFIND (Depth 0/1), GET, HEAD, PUT, DELETE, MKCOL, MOVE, COPY,
// LOCK / UNLOCK (finti: Windows li richiede per scrivere), PROPPATCH (accettato senza effetto).
// Nessuna autenticazione: Windows rifiuta di default la Basic auth su HTTP in chiaro.
// Da usare solo nella modalità SD (comando seriale "microsd"), mai insieme al programma normale.

// Registra il gestore WebDAV su 'server' (prima di server.begin()).
void sdWebDavBegin(WebServer &server);

#endif // SD_WEBDAV_H
