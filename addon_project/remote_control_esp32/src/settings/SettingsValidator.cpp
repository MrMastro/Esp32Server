#include "SettingsValidator.h"

#include "CommunicationMode.h"
#include "DeviceMode.h"

static const char *AUTH_NAME = "deviceName";
static const char *AUTH_PASSWORD = "devicePassword";

// Chiavi di main_settings.json modificabili tramite API.
// deviceName e devicePassword non ci sono: nella richiesta servono per l'autenticazione.
static const char *EDITABLE_KEYS[] = {
  "mode",
  "communicationMode",
  "debug",
  "ssidAP",
  "passwordAP",
  "ssidWIFI",
  "passwordWIFI",
  "ota_upload",
  "commandTransportMode",
};

static SettingsValidation fail(int code, const String &error)
{
  SettingsValidation v;
  v.ok = false;
  v.httpCode = code;
  v.error = error;
  return v;
}

static bool isEditable(const String &key)
{
  for (const char *k : EDITABLE_KEYS)
  {
    if (key.equals(k))
      return true;
  }
  return false;
}

// Controlla tipo e formato del valore per la chiave. Ritorna "" se valido, altrimenti l'errore.
static String validateValue(const String &key, JsonVariantConst value)
{
  if (key == "debug" || key == "ota_upload")
  {
    return value.is<bool>() ? "" : key + " deve essere true o false";
  }

  if (!value.is<const char *>())
  {
    return key + " deve essere una stringa";
  }
  String s = value.as<String>();

  if (key == "mode")
  {
    return deviceModeStringToEnum(s) != DEVICE_MODE::UNKNOWN ? "" : "mode non valida (valori: remote_control)";
  }
  if (key == "commandTransportMode")
  {
    return commandTransportModeStringToEnum(s) != COMMAND_TRANSPORT_MODE::UNKNOWN_MODE
               ? ""
               : "commandTransportMode non valida (valori: ESP_NOW, API_CALL)";
  }
  if (key == "communicationMode")
  {
    return communicationModeStringToEnum(s) != COMMUNICATION_MODE::UNKNOWN_MODE
               ? ""
               : "communicationMode non valida (valori: AP_MODE, WIFI_MODE)";
  }
  if (key == "ssidAP" || key == "ssidWIFI")
  {
    return (s.length() >= 1 && s.length() <= 32) ? "" : key + " deve avere da 1 a 32 caratteri";
  }
  if (key == "passwordAP" || key == "passwordWIFI")
  {
    // vuota = rete aperta; altrimenti WPA2 richiede 8-63 caratteri
    return (s.length() == 0 || (s.length() >= 8 && s.length() <= 63)) ? "" : key + " deve essere vuota o avere da 8 a 63 caratteri";
  }
  return "chiave non gestita";
}

SettingsValidation validateSettingsRequest(const JsonDocument &request, const SettingsModel &current)
{
  if (!request.is<JsonObjectConst>())
  {
    return fail(400, "il corpo deve essere un oggetto JSON");
  }
  JsonObjectConst obj = request.as<JsonObjectConst>();

  // autenticazione
  if (!obj[AUTH_NAME].is<const char *>() || !obj[AUTH_PASSWORD].is<const char *>())
  {
    return fail(400, "deviceName e devicePassword sono obbligatori");
  }
  if (current.deviceName != obj[AUTH_NAME].as<const char *>() ||
      current.devicePassword != obj[AUTH_PASSWORD].as<const char *>())
  {
    return fail(401, "deviceName o devicePassword errati");
  }

  // esattamente una chiave da modificare
  String key = "";
  for (JsonPairConst kv : obj)
  {
    String k = kv.key().c_str();
    if (k == AUTH_NAME || k == AUTH_PASSWORD)
      continue;
    if (key.length() > 0)
    {
      return fail(400, "si può modificare una sola chiave per richiesta");
    }
    key = k;
  }
  if (key.length() == 0)
  {
    return fail(400, "nessuna chiave da modificare");
  }
  if (!isEditable(key))
  {
    return fail(400, "chiave non modificabile: " + key);
  }

  String valueError = validateValue(key, obj[key]);
  if (valueError.length() > 0)
  {
    return fail(400, valueError);
  }

  SettingsValidation v;
  v.ok = true;
  v.httpCode = 200;
  v.key = key;
  return v;
}
