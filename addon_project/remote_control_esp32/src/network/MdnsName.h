#ifndef MDNS_NAME_H
#define MDNS_NAME_H

#include <Arduino.h>

// Nome mDNS (indirizzo "<nome>.local") ricavato dal deviceName di main_settings.json.
// Negli indirizzi di rete sono validi solo lettere, cifre e '-': "ESP32_Remote" -> "esp32-remote".
// Se dal deviceName non resta nulla di valido si usa "esp32-remote".
String mdnsHostnameFromDeviceName(const String &deviceName);

// Avvia mDNS con quel nome e annuncia il server HTTP/WebDAV sulla porta 80.
// Restituisce il nome usato (senza ".local"), stringa vuota se mDNS non è partito.
String mdnsBeginForWebDav(const String &deviceName);

#endif // MDNS_NAME_H
