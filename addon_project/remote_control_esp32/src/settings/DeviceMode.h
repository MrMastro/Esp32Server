#ifndef DEVICE_MODE_H
#define DEVICE_MODE_H

#include <Arduino.h>

// Funzione del dispositivo, scelta dal campo "mode" di main_settings.json.
// Per aggiungere una modalità: nuovo valore qui + stringa in DeviceMode.cpp + gestione in main.cpp.
enum class DEVICE_MODE
{
  REMOTE_CONTROL, // "remote_control"
  UNKNOWN
};

DEVICE_MODE deviceModeStringToEnum(String inputString);
String deviceModeEnumToString(DEVICE_MODE inputEnum);

#endif // DEVICE_MODE_H
