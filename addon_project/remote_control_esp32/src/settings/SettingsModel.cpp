#include "SettingsModel.h"

#include <SD.h>

SettingsModel SettingsModel::getDefault()
{
  SettingsModel defaultSettings;
  defaultSettings.mode = "remote_control";
  defaultSettings.deviceName = "ESP32_Device";
  defaultSettings.devicePassword = "esp32password";
  defaultSettings.communicationMode = "AP_MODE";
  defaultSettings.debug = false;

  defaultSettings.ssidAP = "ESP32_AP";
  defaultSettings.passwordAP = "ap_password";
  defaultSettings.ssidWIFI = "Home_Network";
  defaultSettings.passwordWIFI = "wifi_password";

  return defaultSettings;
}

void SettingsModel::fromJson(const JsonDocument &doc)
{
  mode = doc["mode"] | mode;
  deviceName = doc["deviceName"] | deviceName;
  devicePassword = doc["devicePassword"] | devicePassword;
  communicationMode = doc["communicationMode"] | communicationMode;
  debug = doc["debug"] | debug;
  ssidAP = doc["ssidAP"] | ssidAP;
  passwordAP = doc["passwordAP"] | passwordAP;
  ssidWIFI = doc["ssidWIFI"] | ssidWIFI;
  passwordWIFI = doc["passwordWIFI"] | passwordWIFI;
}

SettingsModel SettingsModel::load(const char *path, bool sdAvailable, bool *loadedFromFile)
{
  SettingsModel settings = getDefault();
  if (loadedFromFile)
    *loadedFromFile = false;

  if (!sdAvailable)
  {
    Serial.println("[Settings] SD assente: uso le impostazioni di default");
    return settings;
  }

  File file = SD.open(path, FILE_READ);
  if (!file)
  {
    Serial.printf("[Settings] %s non trovato: uso le impostazioni di default\n", path);
    return settings;
  }

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, file);
  file.close();

  if (error)
  {
    Serial.printf("[Settings] JSON non valido in %s (%s): uso le impostazioni di default\n", path, error.c_str());
    return settings;
  }

  settings.fromJson(doc);
  if (loadedFromFile)
    *loadedFromFile = true;
  Serial.printf("[Settings] caricato %s\n", path);
  return settings;
}

void SettingsModel::print() const
{
  Serial.println("[Settings] ----------------------------");
  Serial.println("  mode:              " + mode);
  Serial.println("  deviceName:        " + deviceName);
  Serial.println("  communicationMode: " + communicationMode);
  Serial.println("  debug:             " + String(debug ? "true" : "false"));
  Serial.println("  ssidAP:            " + ssidAP);
  Serial.println("  ssidWIFI:          " + ssidWIFI);
  Serial.println("  (password nascoste)");
  Serial.println("[Settings] ----------------------------");
}
