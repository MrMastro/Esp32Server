#include "./services/ServiceImplementations/CommandService.h"
#include "InfoService.cpp"
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
  isOperative = true;
}


StatusInfo CommandService::executeCommand(CMD cmd, String cmdString)
{
  StatusInfo result = getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  String content = "";
  String s = "executeCommand(cmd={cmd}, cmdString={cmdString})";
  s.replace("{cmd}", cmdString);
  s.replace("{cmdString}", cmdString);
  serialService->logInfoln(s,"CommandService");
  if (!isOperative)
  {
    //throwError(ERROR_CODE::SERVICE_ERROR, "Attach Serials first (Serial or WebSerial)", "executeCommand");
    return getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  }
  switch (cmd)
  {
  case CMD::LED_ON:
    ((LedService*) getServiceByCollector("LedService"))->changeLed(true,false);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::LED_OFF:
    ((LedService*) getServiceByCollector("LedService"))->changeLed(false,false);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::LED_TOGGLE:
    ((LedService*) getServiceByCollector("LedService"))->changeLed(true,true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::START_PROGRESS_BAR:
    ((LedService *) getServiceByCollector("LedService"))->startEffect(WS2811_EFFECT::PROGRESSIVE_BAR_UNIQUE_COLOR,RgbColor(0,0,255),100, false, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::OFF_STRIPT:
    ((LedService *) getServiceByCollector("LedService"))->stopEffect(WS2811_EFFECT::ACTUAL_EFFECT, RgbColor(0,0,0), 100, true, true);
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    break;
  case CMD::INFO_IP:
    content = ((InfoService *) getServiceByCollector("InfoService"))->getIp();
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    result.setDescription(content);
    break;
  case CMD::INFO:
    content = ((InfoService *) getServiceByCollector("InfoService"))->getInfo();
    result = getStatusInfoByHttpCode(HTTP_CODE::OK);
    result.setDescription(content);
    break;
  default:
    result = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
    break;
  }
  return result;
}

StatusInfo CommandService::recvMsgAndExecute(String data)
{
  String s = "recvMsgAndExecute(data={data})";
  s.replace("{data}", data);
  serialService->logInfoln(s,"CommandService");
  if (!isOperative)
  {
    //throwError(ERROR_CODE::SERVICE_ERROR, "Service not inizializer. attach serial and webSerial pointers with attachSerial method or use constructor with param non null", "recvMsgAndExecute");
    return getStatusInfoByHttpCode(HTTP_CODE::InternalServerError);
  }
  
  StatusInfo result = executeCommand(mapStringToEnum(data), data);
  if(result.getMessage() == getStatusInfoByHttpCode(HTTP_CODE::BadRequest).getMessage()){
    serialService->println(formatMsg(UKNOWN_COMMAND, {data}));
  }else
  {
    serialService->println(formatMsg(SUCCESS_COMMAND, {data, result.getDescription()}));    
  }

  return result;
}