#include "SdStorage.h"

#include <SD.h>
#include <SPI.h>
#include "BoardPins.h"

bool sdBegin()
{
  // SPI.begin() è già stato chiamato dal display con i pin corretti: qui lo si riusa.
  if (!SD.begin(PIN_SD_CS, SPI, 20000000))
  {
    Serial.println("[SD] microSD non trovata o non formattata FAT32");
    return false;
  }

  Serial.printf("[SD] montata, tipo %d, %llu MB\n", SD.cardType(), SD.cardSize() / (1024ULL * 1024ULL));
  if (!SD.exists(SD_CONFIG_DIR))
  {
    Serial.println("[SD] cartella " SD_CONFIG_DIR " assente");
  }
  return true;
}
