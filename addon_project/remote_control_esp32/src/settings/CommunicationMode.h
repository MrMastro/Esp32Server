#ifndef COMMUNICATION_MODE_H
#define COMMUNICATION_MODE_H

#include <Arduino.h>

// Sottoinsieme di COMMUNICATION_MODE del progetto principale (Esp32Server/src/constants/CommunicationMode.h).
// BLUETOOTH_MODE non c'è: l'ESP32-C6 ha solo Bluetooth LE, non il Bluetooth classico (BluetoothSerial).
enum class COMMUNICATION_MODE
{
  AP_MODE,   // la scheda crea il proprio access point (ssidAP / passwordAP)
  WIFI_MODE, // si collega alla rete (ssidWIFI / passwordWIFI); se fallisce ripiega su AP_MODE
  UNKNOWN_MODE
};

COMMUNICATION_MODE communicationModeStringToEnum(String inputString);
String communicationModeEnumToString(COMMUNICATION_MODE inputEnum);

#endif // COMMUNICATION_MODE_H
