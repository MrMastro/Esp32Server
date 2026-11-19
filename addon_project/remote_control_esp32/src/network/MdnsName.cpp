#include "MdnsName.h"

#include <ESPmDNS.h>
#include "system/Log.h"

static const size_t MDNS_MAX_LABEL = 63; // lunghezza massima di un nome DNS
static const char *MDNS_FALLBACK_NAME = "esp32-remote";

String mdnsHostnameFromDeviceName(const String &deviceName)
{
  String name;
  for (size_t i = 0; i < deviceName.length() && name.length() < MDNS_MAX_LABEL; i++)
  {
    char c = deviceName[i];
    if (isalnum((unsigned char)c))
      name += (char)tolower((unsigned char)c);
    else if ((c == '_' || c == '-' || c == ' ' || c == '.') && name.length() > 0 && !name.endsWith("-"))
      name += '-'; // separatori -> un solo trattino
  }
  while (name.endsWith("-"))
    name.remove(name.length() - 1);
  return name.length() > 0 ? name : String(MDNS_FALLBACK_NAME);
}

String mdnsBeginForWebDav(const String &deviceName)
{
  String hostname = mdnsHostnameFromDeviceName(deviceName);
  if (!MDNS.begin(hostname.c_str()))
  {
    logWarning("[mDNS] avvio fallito per " + hostname + ".local");
    return "";
  }
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("webdav", "tcp", 80);
  logInfo("[mDNS] annunciato " + hostname + ".local");
  return hostname;
}
