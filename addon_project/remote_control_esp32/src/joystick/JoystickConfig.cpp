#include "JoystickConfig.h"
#include "system/Log.h"

#include <ArduinoJson.h>
#include <SD.h>

bool JoystickConfig::loadFromFile(const char *path)
{
  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    logInfof("[JoystickConfig] %s non trovato, uso i default\n", path);
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
  {
    logWarningf("[JoystickConfig] JSON non valido in %s: %s\n", path, error.c_str());
    return false;
  }

  enableJoystick = doc["enableJoystick"] | enableJoystick;
  pinSwitch = doc["pinSwitch"] | pinSwitch;
  pinAnalogX = doc["pinAnalogX"] | pinAnalogX;
  pinAnalogY = doc["pinAnalogY"] | pinAnalogY;
  centerX = doc["centerX"] | centerX;
  centerY = doc["centerY"] | centerY;
  deadZone = doc["deadZone"] | deadZone;
  autoCalibrate = doc["autoCalibrate"] | autoCalibrate;

  logInfof("[JoystickConfig] caricato %s (SW=%d X=%d Y=%d deadZone=%d)\n",
                path, pinSwitch, pinAnalogX, pinAnalogY, deadZone);
  return true;
}
