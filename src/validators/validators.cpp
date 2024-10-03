#include "validators.h"

String validateCmd(CMD cmd, std::vector<String> params)
{
  switch (cmd)
  {
  case CMD::CHANGE_COMMUNICATION_MODE:
    return validateChangeCommunicationMode(params);
  case CMD::REBOOT:
    return validateReboot(params);
  case CMD::UNKNOWN:
  case CMD::NONE:
    return "Command don't allow or unknow";
  default:
    return String();
  }
};

String validateChangeCommunicationMode(std::vector<String> params)
{
  String error ="";
  if (params.size() < 1)
  {
    return "insert mode like first param";
  }

  String newCommunicationMode = params.at(0);
  COMMUNICATION_MODE cm = communicationModeStringToEnum(newCommunicationMode);

  if (cm == COMMUNICATION_MODE::UNKNOWN_MODE)
  {
    return ("Communication mode is not valid");
  }

  return String();

};

String validateReboot(std::vector<String> params)
{
  if(params.size()<1)
  {
    return String();
  }

  if(!isUnsignedLong(params.at(0)))
  {
    return "first param must be a number";
  }

  return String();
};

bool isUnsignedLong(const String& str) {
  char* end;
  unsigned long val = strtoul(str.c_str(), &end, 10);

  // Se end non punta alla fine della stringa, ci sono caratteri non validi
  return *end == '\0';
};
