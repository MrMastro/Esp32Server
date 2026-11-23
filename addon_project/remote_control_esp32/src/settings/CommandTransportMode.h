#ifndef COMMAND_TRANSPORT_MODE_H
#define COMMAND_TRANSPORT_MODE_H

#include <Arduino.h>

// Come le direzioni del joystick arrivano ai dispositivi ("commandTransportMode" in main_settings.json):
enum class COMMAND_TRANSPORT_MODE
{
  ESP_NOW,  // collegamento continuo ESP-NOW: accoppiamento, heartbeat, sessioni (docs/WIRELESS_JOYSTICK.md)
  API_CALL, // niente ESP-NOW: a ogni direzione (tranne CENTER / CENTER_BUTTON) una POST /api/v1/sendCommand
  UNKNOWN_MODE
};

COMMAND_TRANSPORT_MODE commandTransportModeStringToEnum(String inputString);
String commandTransportModeEnumToString(COMMAND_TRANSPORT_MODE inputEnum);

#endif // COMMAND_TRANSPORT_MODE_H
