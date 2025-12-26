#include "ButtonPresets.h"

const std::pair<BTN_PRESETS, String> BTN_PRESETS_PAIR[] = {
  {BTN_PRESETS::K1,  "K1"},
  {BTN_PRESETS::K2,  "K2"},
  {BTN_PRESETS::K3,  "K3"},
  {BTN_PRESETS::K4,  "K4"},
  {BTN_PRESETS::K5,  "K5"},
  {BTN_PRESETS::K6,  "K6"},
  {BTN_PRESETS::K7,  "K7"},
  {BTN_PRESETS::K8,  "K8"},
  {BTN_PRESETS::K9,  "K9"},
  {BTN_PRESETS::K10, "K10"},
  {BTN_PRESETS::K11, "K11"},
  {BTN_PRESETS::K12, "K12"},
  {BTN_PRESETS::K13, "K13"},
  {BTN_PRESETS::K14, "K14"},
  {BTN_PRESETS::K15, "K15"},
  {BTN_PRESETS::K16, "K16"},
  {BTN_PRESETS::K17, "K17"},
  {BTN_PRESETS::K18, "K18"},
  {BTN_PRESETS::NONE, "NONE"},
};

BTN_PRESETS mapStringToBpresets(String inputString)
{
  inputString.toUpperCase();

  for (const auto &entry : BTN_PRESETS_PAIR)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }

  return BTN_PRESETS::NONE;
}

String mapBpresetsToString(BTN_PRESETS input)
{
  for (const auto &pair : BTN_PRESETS_PAIR)
  {
    if (pair.first == input)
    {
      return pair.second;
    }
  }

  return "NONE";
}
