#include "CommandService.h"

// This Service works to exec command Serial and exec scheduled cmd of esp32 (Ex: reboot after 5 sec)

CommandService::CommandService()
{
  isOperative = false;
  delayedCmdToDo = CMD::NONE;
}

boolean CommandService::isAvaible()
{
  return isOperative;
}

void CommandService::onInitServiceCollector()
{
  serialService = ((SerialService *)servicesCollector.getService("SerialService"));
  settingService = ((SettingService *)servicesCollector.getService("SettingService"));
  isOperative = true;
}

StatusInfo CommandService::executeCommand(CMD cmd, std::vector<String> params)
{
  StatusInfo result = getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  String content = "";
  String s = "executeCommand(cmd={cmd}, params={params})";
  s.replace("{cmd}", mapEnumToString(cmd));
  s.replace("{params}", vectorStringtoString(params));
  serialService->logInfoln(s, "CommandService");
  if (!isOperative)
  {
    // throwError(ERROR_CODE::SERVICE_ERROR, "Attach Serials first (Serial or WebSerial)", "executeCommand");
    return getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  }
  // In modalita' file girano solo WiFi, WebDAV e seriale: gli altri comandi non hanno senso
  if (fileModeActive() && cmd != CMD::REBOOT && cmd != CMD::INFO && cmd != CMD::INFO_IP && cmd != CMD::HELP &&
      cmd != CMD::MICROSD && cmd != CMD::FILEREMOTE)
  {
    result = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    result.setDescription("non disponibile in modalita' file: riavvia (reboot) per uscire");
    return result;
  }

  switch (cmd)
  {
  case CMD::LED_ON:
    ((LedService *)getServiceByCollector("LedService"))->changeLed(true, false);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::LED_OFF:
    ((LedService *)getServiceByCollector("LedService"))->changeLed(false, false);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::LED_TOGGLE:
    ((LedService *)getServiceByCollector("LedService"))->changeLed(true, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::START_PROGRESS_BAR:
    ((LedService *)getServiceByCollector("LedService"))->startEffect("PROGRESSIVE_BAR_UNIQUE_COLOR", RgbColor(0, 0, 255), 100, true, true, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::START_EFFECT:
    ((LedService *)getServiceByCollector("LedService"))->startEffect(params.at(0), RgbColor(0, 0, 0), 100, false, false, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::STOP_EFFECT:
    ((LedService *)getServiceByCollector("LedService"))->stopEffect(params.at(0), {RgbColor(0, 0, 0)}, 100, false, false, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::OFF_STRIP:
    ((LedService *)getServiceByCollector("LedService"))->stopEffect("PROGRESSIVE_BAR_UNIQUE_COLOR", RgbColor(0, 0, 0), 100, true, true, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::REBOOT:
    reboot(params);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::CHANGE_COMMUNICATION_MODE:
    result = changeCommunicationMode(params);
    break;
  case CMD::INFO_IP:
    content = fileModeActive() ? fileModeIp() : ((InfoService *)getServiceByCollector("InfoService"))->getIp();
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    result.setDescription(content);
    break;
  case CMD::INFO:
    content = fileModeActive() ? "modalita' file attiva: " + fileModeAddress() + " (IP " + fileModeIp() + ")"
                               : ((InfoService *)getServiceByCollector("InfoService"))->getInfo();
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    result.setDescription(content);
    break;
  case CMD::PROD:
    result = setDebugAndReboot(false);
    break;
  case CMD::DEBUG:
    result = setDebugAndReboot(true);
    break;
  case CMD::HELP:
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    result.setDescription(helpText());
    break;
  case CMD::MICROSD:
  case CMD::FILEREMOTE:
    result = requestFileMode();
    break;
  default:
    result = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    result.setDescription(UKNOWN_COMMAND);
    break;
  }
  return result;
}

StatusInfo CommandService::insertDelayedCommand(CMD cmd, std::vector<String> params, unsigned long millsDelayed)
{
  if (!isOperative)
  {
    // throwError(ERROR_CODE::SERVICE_ERROR, "Service not inizializer. attach serial and webSerial pointers with attachSerial method or use constructor with param non null", "recvMsgAndExecute");
    return getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  }

  String errorValidation = validateCmd(cmd,params);

  if(!errorValidation.isEmpty())
  {
    return getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
  }

  delayedCmdToDo = cmd;
  delayedParams = params;
  triggerTimeCmd = millis() + millsDelayed;
  return StatusInfo();
}

StatusInfo CommandService::changeCommunicationMode(std::vector<String> params)
{
  StatusInfo res = StatusInfo();
  if (params.size() < 1)
  {
    res = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    res.setDescription("insert mode like first param");
    return res;
  }

  String newCommunicationMode = params.at(0);
  COMMUNICATION_MODE cm = communicationModeStringToEnum(newCommunicationMode);

  if (cm == COMMUNICATION_MODE::UNKNOWN_MODE)
  {
    res = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    res.setDescription("Communication mode is not valid");
    return res;
  }

#ifdef EXCLUDE_BLUETOOTH
  if (cm == COMMUNICATION_MODE::BLUETOOTH_MODE)
  {
    res = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    res.setDescription("Bluetooth is excluded from this firmware (EXCLUDE_BLUETOOTH)");
    return res;
  }
#endif

  boolean operationDone = ((SettingService *)servicesCollector.getService("SettingService"))->changeSetting("communicationMode", newCommunicationMode);

  if (!operationDone)
  {
    res = getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
    res.setDescription("Error while set new communication");
    return res;
  }

  res = getStatusInfoByHttpCode(HTTP_CODE::OK);
  res.setDescription("communication mode is setted, restarting....");

  insertDelayedCommand(CMD::REBOOT, {}, 1000);

  return res;
}

// prod / debug: salva il flag debug in settings.json e riavvia (i log informativi escono solo con debug = true)
StatusInfo CommandService::setDebugAndReboot(bool debug)
{
  StatusInfo res;
  boolean operationDone = ((SettingService *)servicesCollector.getService("SettingService"))->changeSetting("debug", debug ? "true" : "false");
  if (!operationDone)
  {
    res = getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
    res.setDescription("Error while saving debug");
    return res;
  }
  res = getStatusInfoByHttpCode(HTTP_CODE::OK);
  res.setDescription(String("debug = ") + (debug ? "true" : "false") + " saved, restarting....");
  insertDelayedCommand(CMD::REBOOT, {}, 1000);
  return res;
}

// microsd / fileremote: riavvia in modalita' file (memoria flash come unita' di rete WebDAV).
// Il nome "microsd" e' lo stesso del telecomando: qui non c'e' microSD, si espone la flash (LittleFS).
StatusInfo CommandService::requestFileMode()
{
  StatusInfo res;
  if (fileModeActive())
  {
    res = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    res.setDescription("modalita' file gia' attiva: riavvia (reboot) per uscire");
    return res;
  }
  fileModeRequest();
  res = getStatusInfoByHttpCode(HTTP_CODE::OK);
  res.setDescription("riavvio in modalita' file (memoria flash come unita' di rete WebDAV); per uscire: reboot o stacca la corrente");
  insertDelayedCommand(CMD::REBOOT, {}, 1000);
  return res;
}

String CommandService::helpText()
{
  return String("lista dei comandi (parametri separati da virgola):\n") +
         "  led_on | led_off | led_toggle          LED di stato\n" +
         "  start_progress_bar | off_strip         barra di prova / spegne le strisce\n" +
         "  start_effect,<effetto>                 avvia un effetto sulla matrice\n" +
         "  stop_effect,<effetto>                  ferma un effetto sulla matrice\n" +
         "  change_communication_mode,<modo>       AP_MODE | WIFI_MODE | BLUETOOTH_MODE, salva e riavvia\n" +
         "  reboot[,ms]                            riavvia (subito o dopo ms millisecondi)\n" +
         "  info | info_ip                         stato del dispositivo / indirizzo IP\n" +
         "  prod                                   debug = false (solo warning, errori e risposte), salva e riavvia\n" +
         "  debug                                  debug = true (tutti i log), salva e riavvia\n" +
         "  microsd | fileremote                   modalita' file: memoria flash come unita' di rete (WebDAV), riavvia\n" +
         "  help                                   questa lista\n" +
         "  t                                      funzione di prova test()\n" +
         "  <trigger di un preset>                 avvia il preset LED con quel trigger (es. CENTER, UP, K1)";
}

void CommandService::reboot(std::vector<String> params)
{
  if(params.size()<1)
  {
    ESP.restart();
    return;
  }

  unsigned long millsDelayed = (unsigned long) params.at(0).toInt();
  if(millsDelayed == 0)
  {
    ESP.restart();
  }
  else
  {
    insertDelayedCommand(CMD::REBOOT, {}, millsDelayed);
  }
}

StatusInfo CommandService::recvMsgAndExecute(String data)
{
  String s = "recvMsgAndExecute(data={data})";
  s.replace("{data}", data);
  serialService->logInfoln(s, "CommandService");
  if (!isOperative)
  {
    // throwError(ERROR_CODE::SERVICE_ERROR, "Service not inizializer. attach serial and webSerial pointers with attachSerial method or use constructor with param non null", "recvMsgAndExecute");
    return getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  }
  std::vector<String> dataArray = splitString(data, ',');

  if (dataArray.size() == 0)
  {
    serialService->logError("Invalid data received", getNameService(), "recvMsgAndExecute");
    return getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  }

  String command = stringPop(dataArray);

  // String errorValidation = validateCmd(mapStringToEnum(command), dataArray);
  // if(!errorValidation.isEmpty())
  // {
  //   StatusInfo result = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
  //   result.setDescription(errorValidation);
  //   serialService->println(formatMsg(ERROR_COMMAND, {command, result.getDescription()}));
  //   return result;
  // }

  StatusInfo result = executeCommand(mapStringToEnum(command), dataArray);

  //Check other command after command enum (in modalita' file niente preset LED)
  if (!fileModeActive() && result.getMessage() == getStatusInfoByHttpCode(HTTP_CODE::BadRequest).getMessage())
  {
    //CONTROLLO LED PRESET
    const LedPresetListModel presets = settingService->getPresets();
    const LedPresetModel* execPreset1 = presets.getByTrigger(command);
    const SettingsModel s = settingService->getSettings();
    if(execPreset1 != nullptr){
      std::vector<RgbColor> rgbColors = getRgbColorsByLedColor(execPreset1->colors);
      ((LedService *)servicesCollector.getService("LedService"))->startEffect(execPreset1->effect, rgbColors, execPreset1->deltaT, s.ledSettings.enableStripRgb, s.ledSettings.enableStripWs2811, s.ledSettings.enableStripWs2811Matrix);
      result = getStatusInfoByHttpCode(HTTP_CODE::OK);
      return result;
    }else{
      result = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
      result.setDescription(UKNOWN_COMMAND);
    }
  }

  //Final check
  if (result.getMessage() == getStatusInfoByHttpCode(HTTP_CODE::BadRequest).getMessage())
  {
    serialService->println(formatMsg(ERROR_COMMAND, {command, result.getDescription()}));
  }
  else
  {
    serialService->println(formatMsg(SUCCESS_COMMAND, {command, result.getDescription()}));
  }

  return result;
}

StatusInfo CommandService::checkDelyedCmdAndExecute()
{
  unsigned long thisMoment = millis();
  if (delayedCmdToDo != CMD::NONE && thisMoment >= triggerTimeCmd)
  {
    StatusInfo result = executeCommand(delayedCmdToDo, delayedParams);
    delayedCmdToDo = CMD::NONE;
    serialService->println(formatMsg(SUCCESS_DELAYED_COMMAND, {triggerTimeCmd, result.getDescription()}));
    return result;
  }
  return getStatusInfoByHttpCode(HTTP_CODE::OK);
}