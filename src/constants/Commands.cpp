#include "Commands.h"

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

String mapEnumToString(CMD input)
{
  for (const auto &entry : CMD_PAIR)
  {
    if (entry.first == input)
    {
      return entry.second;
    }
  }
  // Return a special value or throw an exception if the string is not found
  return mapEnumToString(CMD::UNKNOWN);
}
