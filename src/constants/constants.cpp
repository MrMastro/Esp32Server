#include "constants.h"

StatusInfo getStatusInfoByHttpCode(HTTP_CODE code)
{
    String statusStr = HTTP_CODE_MAP.at(code);
    std::vector<String> parts = splitString(statusStr,',');
    if (parts.size() == 2) {
        // Check if the conversion was successful
        return StatusInfo(parts[0], parts[1]);
    }

    // Handle incorrect format or conversion failure
    return StatusInfo{};
}


CMD mapStringToEnum(String inputString)
{
  inputString.toLowerCase();
  for (const auto &entry : CMD_PAIR)
  {
    if (entry.second == inputString)
    {
      return entry.first;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return CMD::UNKNOWN;
}

// CMD mapStringToEnum(String inputString)
// {
//   inputString.toUpperCase();
//   if (inputString.equalsIgnoreCase("led on"))
//   {
//     return CMD::LED_ON;
//   }
//   else if (inputString.equalsIgnoreCase("led off"))
//   {
//     return CMD::LED_OFF;
//   }
//   else if (inputString.equalsIgnoreCase("led toggle"))
//   {
//     return CMD::LED_TOGGLE;
//   }
//   else if (inputString.equalsIgnoreCase("info"))
//   {
//     return CMD::INFO;
//   }
//   else
//   {
//     // Valore predefinito in caso di stringa non riconosciuta
//     return CMD::OTHER;
//   }
// }