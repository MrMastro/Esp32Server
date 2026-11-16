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
  defaultSettings.otaUpload = false;

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
  otaUpload = doc["ota_upload"] | otaUpload;
}

void SettingsModel::toJson(JsonDocument &doc) const
{
  doc["mode"] = mode;
  doc["deviceName"] = deviceName;
  doc["devicePassword"] = devicePassword;
  doc["communicationMode"] = communicationMode;
  doc["debug"] = debug;
  doc["ssidAP"] = ssidAP;
  doc["passwordAP"] = passwordAP;
  doc["ssidWIFI"] = ssidWIFI;
  doc["passwordWIFI"] = passwordWIFI;
  doc["ota_upload"] = otaUpload;
}

SettingsModel SettingsModel::load(const char *path, bool sdAvailable, bool *loadedFromFile, String *loadInfo)
{
  SettingsModel settings = getDefault();
  String info;
  bool fromFile = false;

  if (!sdAvailable)
  {
    info = "[Settings] SD assente: impostazioni di default";
  }
  else
  {
    File file = SD.open(path, FILE_READ);
    if (!file)
    {
      info = String("[Settings] ") + path + " non trovato: impostazioni di default";
    }
    else
    {
      JsonDocument doc;
      DeserializationError error = deserializeJson(doc, file);
      file.close();

      if (error)
      {
        info = String("[Settings] JSON non valido in ") + path + " (" + error.c_str() + "): impostazioni di default";
      }
      else
      {
        settings.fromJson(doc);
        fromFile = true;
        info = String("[Settings] caricato ") + path;
      }
    }
  }

  if (loadedFromFile)
    *loadedFromFile = fromFile;
  if (loadInfo)
    *loadInfo = info;
  return settings;
}

String SettingsModel::toJsonString(bool hidePasswords) const
{
  JsonDocument doc;
  toJson(doc);
  if (hidePasswords)
  {
    doc["devicePassword"] = "********";
    doc["passwordAP"] = "********";
    doc["passwordWIFI"] = "********";
  }
  String out;
  serializeJsonPretty(doc, out);
  return out;
}

bool SettingsModel::saveKey(const char *path, const String &key, JsonVariantConst value) const
{
  // 1) parte dal file esistente (mantiene anche eventuali campi extra)
  JsonDocument doc;
  bool fromFile = false;
  File in = SD.open(path, FILE_READ);
  if (in)
  {
    fromFile = !deserializeJson(doc, in);
    in.close();
  }
  if (!fromFile)
  {
    doc.clear();
    toJson(doc);
  }

  // 2) modifica la chiave
  doc[key] = value;

  // 3) scrive su un file temporaneo e poi lo sostituisce all'originale,
  //    così un'interruzione a metà non lascia un main_settings.json troncato
  String tmpPath = String(path) + ".tmp";
  File out = SD.open(tmpPath, FILE_WRITE);
  if (!out)
  {
    Serial.println("[Settings] impossibile scrivere " + tmpPath);
    return false;
  }
  size_t written = serializeJsonPretty(doc, out);
  out.close();
  if (written == 0)
  {
    SD.remove(tmpPath);
    return false;
  }

  SD.remove(path);
  if (!SD.rename(tmpPath, path))
  {
    Serial.println("[Settings] rename fallito: impostazioni rimaste in " + tmpPath);
    return false;
  }

  Serial.printf("[Settings] salvata la chiave '%s' in %s\n", key.c_str(), path);
  return true;
}
