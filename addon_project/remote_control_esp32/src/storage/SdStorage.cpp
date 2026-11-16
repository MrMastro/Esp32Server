#include "SdStorage.h"

#include <SD.h>
#include <SPI.h>
#include "BoardPins.h"

bool sdBegin(String *info)
{
  // SPI.begin() è già stato chiamato dal display con i pin corretti: qui lo si riusa.
  if (!SD.begin(PIN_SD_CS, SPI, 20000000))
  {
    if (info)
      *info = "[SD] microSD non trovata o non formattata FAT32";
    return false;
  }

  if (info)
  {
    *info = "[SD] montata, tipo " + String(SD.cardType()) + ", " +
            String((uint32_t)(SD.cardSize() / (1024ULL * 1024ULL))) + " MB";
    if (!SD.exists(SD_CONFIG_DIR))
    {
      *info += " - cartella " SD_CONFIG_DIR " assente";
    }
  }
  return true;
}
