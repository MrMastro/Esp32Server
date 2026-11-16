#ifndef OTA_UPLOAD_H
#define OTA_UPLOAD_H

#include <Arduino.h>
#include "settings/SettingsModel.h"

// OTA via WiFi con ArduinoOTA (upload da PlatformIO con l'ambiente "-ota").
// Hostname = deviceName, password = devicePassword, porta 3232 (come il progetto principale).
// Attivo solo se ota_upload = true in main_settings.json.
void otaBegin(const SettingsModel &settings);

// Da chiamare a ogni giro di loop (non fa nulla se l'OTA non è attivo).
void otaHandle();

bool otaIsEnabled();

#endif // OTA_UPLOAD_H
