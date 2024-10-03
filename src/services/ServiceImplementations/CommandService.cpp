#include "CommandService.h"

CommandService::CommandService()
{
  isOperative = false;
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
    ((LedService *)getServiceByCollector("LedService"))->startEffect(WS2811_EFFECT::PROGRESSIVE_BAR_UNIQUE_COLOR, RgbColor(0, 0, 255), 100, false, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::OFF_STRIPT:
    ((LedService *)getServiceByCollector("LedService"))->stopEffect(WS2811_EFFECT::ACTUAL_EFFECT, RgbColor(0, 0, 0), 100, true, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::CHANGE_COMMUNICATION_MODE:
    result = changeCommunicationMode(params);
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

boolean CommandService::validateParams(std::vector<String> params, int minQuantitaty)
{
  return params.size() >= minQuantitaty;
}

StatusInfo CommandService::changeCommunicationMode(std::vector<String> params)
{
  StatusInfo res = StatusInfo();
  if (params.size() < 1)
  {
    res = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    res.setDescription("inseret mode like first param");
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
  res.setDescription("communication mode is setted, please restarting....");
  return res;
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