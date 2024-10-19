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
    ((LedService *)getServiceByCollector("LedService"))->startEffect(EFFECT_LABEL::PROGRESSIVE_BAR_UNIQUE_COLOR, RgbColor(0, 0, 255), 100, false, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::OFF_STRIPT:
    ((LedService *)getServiceByCollector("LedService"))->stopEffect(EFFECT_LABEL::ACTUAL_EFFECT, RgbColor(0, 0, 0), 100, true, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::REBOOT:
    reboot(params);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::CHANGE_COMMUNICATION_MODE:
    result = changeCommunicationMode(params);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::INFO_IP:
    content = ((InfoService *)getServiceByCollector("InfoService"))->getIp();
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    result.setDescription(content);
    break;
  case CMD::INFO:
    content = ((InfoService *)getServiceByCollector("InfoService"))->getInfo();
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    result.setDescription(content);
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

  if (!settingService->changeSetting("communicationMode", newCommunicationMode))
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

  String errorValidation = validateCmd(mapStringToEnum(command), dataArray);
  if(!errorValidation.isEmpty())
  {
    StatusInfo result = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    result.setDescription(errorValidation);
    serialService->println(formatMsg(ERROR_COMMAND, {command, result.getDescription()}));
    return result;
  }

  StatusInfo result = executeCommand(mapStringToEnum(command), dataArray);

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