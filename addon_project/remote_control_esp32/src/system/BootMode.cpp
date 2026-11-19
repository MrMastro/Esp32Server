#include "BootMode.h"

#include <Preferences.h>

static const char *NVS_NAMESPACE = "bootmode";
static const char *KEY_SD_MODE = "sdmode";

void bootModeRequestSdMode()
{
  Preferences prefs;
  if (prefs.begin(NVS_NAMESPACE, false))
  {
    prefs.putBool(KEY_SD_MODE, true);
    prefs.end();
  }
}

bool bootModeConsumeSdMode()
{
  Preferences prefs;
  if (!prefs.begin(NVS_NAMESPACE, false))
    return false;
  bool requested = prefs.getBool(KEY_SD_MODE, false);
  if (requested)
    prefs.remove(KEY_SD_MODE); // vale per un solo avvio
  prefs.end();
  return requested;
}
