#ifndef BUTTON_PRESETS
#define BUTTON_PRESETS

#include <vector>
#include <Arduino.h>

//BUTTON_PRESETS
enum class BTN_PRESETS
{
  K1,
  K2,
  K3,
  K4,
  K5,
  K6,
  K7,
  K8,
  K9,
  K10,
  K11,
  K12,
  K13,
  K14,
  K15,
  K16,
  K17,
  K18,
  NONE
};

//FUNCTION FOR MAPPING
BTN_PRESETS mapStringToBpresets(String inputString);
String mapBpresetsToString(BTN_PRESETS input);

#endif  // BUTTON_PRESETS