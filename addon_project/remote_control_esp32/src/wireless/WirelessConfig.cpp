#include "WirelessConfig.h"
#include "system/Log.h"

#include <ArduinoJson.h>
#include <SD.h>

bool WirelessConfig::loadFromFile(const char *path)
{
  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    logInfof("[WirelessConfig] %s non trovato, uso i default\n", path);
    return false;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
  {
    logWarningf("[WirelessConfig] JSON non valido in %s: %s\n", path, error.c_str());
    return false;
  }

  sessionTimeoutS = doc["sessionTimeoutS"] | sessionTimeoutS;
  heartbeatActiveMs = doc["heartbeatActiveMs"] | heartbeatActiveMs;
  heartbeatIdleS = doc["heartbeatIdleS"] | heartbeatIdleS;
  missedHeartbeats = doc["missedHeartbeats"] | missedHeartbeats;
  rediscoveryS = doc["rediscoveryS"] | rediscoveryS;
  pairAttempts = doc["pairAttempts"] | pairAttempts;
  longPressS = doc["longPressS"] | longPressS;
  remoteModemSleep = doc["remoteModemSleep"] | remoteModemSleep;

  // valori minimi sensati (0 bloccherebbe la logica)
  heartbeatActiveMs = max<uint32_t>(heartbeatActiveMs, 100);
  heartbeatIdleS = max<uint16_t>(heartbeatIdleS, 1);
  missedHeartbeats = max<uint8_t>(missedHeartbeats, 1);
  rediscoveryS = max<uint16_t>(rediscoveryS, 5);
  pairAttempts = max<uint8_t>(pairAttempts, 1);

  logInfof("[WirelessConfig] caricato %s (sessione %u s, heartbeat %lu ms / %u s, missed %u, riscoperta %u s, modem sleep %s)\n",
                path, sessionTimeoutS, (unsigned long)heartbeatActiveMs, heartbeatIdleS, missedHeartbeats, rediscoveryS,
                remoteModemSleep ? "si" : "no");
  return true;
}
