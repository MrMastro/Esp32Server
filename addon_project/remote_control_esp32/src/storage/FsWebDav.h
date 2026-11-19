#ifndef FS_WEBDAV_H
#define FS_WEBDAV_H

// Server WebDAV minimale su un file system Arduino (microSD, LittleFS, ...), pensato per
// "Connetti unità di rete" di Esplora risorse di Windows (http://<nome>.local/ o http://<ip>/).
//
// STESSO FILE nei due progetti (tenerli allineati):
//   - telecomando:        addon_project/remote_control_esp32/src/storage/FsWebDav.*   (microSD, core 3.x)
//   - progetto generale:  src/fileMode/FsWebDav.*                                     (LittleFS, core 2.x)
//
// Metodi: OPTIONS, PROPFIND (Depth 0/1), GET, HEAD, PUT, DELETE, MKCOL, MOVE, COPY,
// LOCK / UNLOCK (finti: Windows li richiede per scrivere), PROPPATCH (accettato senza effetto).
// Nessuna autenticazione: Windows rifiuta di default la Basic auth su HTTP in chiaro.
// Da usare solo in una modalità dedicata (riavvio apposito), mai insieme al programma normale.

#include <Arduino.h>
#include <FS.h>
#include <WebServer.h>
#include <functional>

typedef std::function<uint64_t()> FsWebDavBytesFn;

// Registra il gestore WebDAV su 'server' (prima di server.begin()). Radice WebDAV = radice di 'fs'.
// totalBytes / usedBytes (facoltativi): spazio del file system, mostrato da Esplora risorse.
void fsWebDavBegin(WebServer &server, fs::FS &fs, FsWebDavBytesFn totalBytes = nullptr, FsWebDavBytesFn usedBytes = nullptr);

#endif // FS_WEBDAV_H
