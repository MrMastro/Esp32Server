#include "validators.h"
#include <models/InitialSettingSaveModel/InitialSettingSaveModel.h>
#include <constants/LedEffects.h>
#include <ledEffects/Effect.h>
#include <constants/constants.h>
#include <models/LedEffectRequest/LedColorRequest.h>

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

String validateSettings(const SettingsModel &s) {
    // Verifica che il nome del dispositivo non sia vuoto
    if (s.deviceName.isEmpty()) {
        return "Error: Device name cannot be empty.";
    }

    // Verifica che la password del dispositivo non sia vuota
    if (s.devicePassword.isEmpty()) {
        return "Error: Device password cannot be empty.";
    }

    // Verifica che il modo di comunicazione sia valido
    COMMUNICATION_MODE cm = communicationModeStringToEnum(s.communicationMode);

    if (cm == COMMUNICATION_MODE::UNKNOWN_MODE)
    {
      return ("communicationMode is not valid");
    }

    // Verifica che l'effetto iniziale non sia vuoto
    if (s.initialEffect.isEmpty()) {
        return "Error: Initial effect cannot be empty.";
    }

    if(!isPresentEffect(s.initialEffect)){
      return "Error: Initial effect is not allow";
    }

    // Verifica che i valori di deltaT siano nel range accettabile
    if (s.initialDeltaT < 0) {
        return "Error: Initial deltaT cannot be negative.";
    }

    // Verifica che i valori RGB siano nel range [0, 255]
    if (s.initialR < 0 || s.initialR > 255) {
        return "Error: Initial R value must be between 0 and 255.";
    }
    if (s.initialG < 0 || s.initialG > 255) {
        return "Error: Initial G value must be between 0 and 255.";
    }
    if (s.initialB < 0 || s.initialB > 255) {
        return "Error: Initial B value must be between 0 and 255.";
    }

    // Verifica che le credenziali della rete AP non siano vuote
    if (s.ssidAP.isEmpty() || s.passwordAP.isEmpty()) {
        return "Error: SSID and password for AP mode cannot be empty.";
    }

    // Verifica che le credenziali della rete WIFI non siano vuote
    if (s.ssidWIFI.isEmpty() || s.passwordWIFI.isEmpty()) {
        return "Error: SSID and password for WIFI mode cannot be empty.";
    }

    // Se tutte le validazioni sono superate, restituisce una stringa vuota
    return String();
}

String validateInitialSettings(const InitialSettingSaveModel &s) {

    // Verifica che l'effetto iniziale non sia vuoto
    if (s.initialEffect.isEmpty()) {
        return "Error: Initial effect cannot be empty.";
    }

    if(!isPresentEffect(s.initialEffect)){
      return "Error: Initial effect is not allow";
    }

    // Verifica che i valori di deltaT siano nel range accettabile
    if (s.initialDeltaT < 0) {
        return "Error: Initial deltaT cannot be negative.";
    }

    // Verifica che i valori RGB siano nel range [0, 255]
    if (s.initialR < 0 || s.initialR > 255) {
        return "Error: Initial R value must be between 0 and 255.";
    }
    if (s.initialG < 0 || s.initialG > 255) {
        return "Error: Initial G value must be between 0 and 255.";
    }
    if (s.initialB < 0 || s.initialB > 255) {
        return "Error: Initial B value must be between 0 and 255.";
    }

    // Se tutte le validazioni sono superate, restituisce una stringa vuota
    return String();
}


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

String validateColors(std::vector<LedColorRequest> colors)
{
  if(colors.size()<1)
  {
    return "colors cannot be empty";
  }

  for (size_t i = 0; i < colors.size(); i++)
  {
    if (colors.at(i).r < 0 || colors.at(i).r > 255 || colors.at(i).g < 0 || colors.at(i).g > 255 || colors.at(i).b < 0 || colors.at(i).b > 255)
    {
      return COLOR_OUT_OF_RANGE_ERROR;
    }
  }
  

  return String();
};