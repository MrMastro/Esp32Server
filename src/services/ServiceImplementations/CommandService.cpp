#include "./services/ServiceImplementations/CommandService.h"

CommandService::CommandService()
{
  isOperative = false;
}

boolean CommandService::isAvaible()
{
  return isOperative;
}

void CommandService::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
  logInfo("Attach Serial of CommandService");
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
  isOperative = true;
}

String CommandService::executeJson(String methodName, String param)
{
  String s = "executeJson(nameService={nameService}, nameMethod={nameMethod}, param={param})";
  s.replace("{nameService}", nameService);
  s.replace("{param}", param);
  logInfo(s);
  if (methodName == "recvMsgAndExecute")
  {
    return recvMsgAndExecute(param);
  }
  else
  {
    return "Service Method not found";
  }
}

String CommandService::executeJson(String methodName, std::vector<String> jsonParams)
{
  String s = "executeJson(nameService={nameService}, nameMethod={nameMethod}, param=vector)";
  s.replace("{nameService}", nameService);
  s.replace("{param}", jsonParams.at(0));
  logInfo(s);
  if (methodName == "recvMsgAndExecute")
  {
    return recvMsgAndExecute(jsonParams.at(0));
  }
  else
  {
    return "Service Method not found";
  }
}

String CommandService::executeCommand(CMD cmd, String cmdString)
{
  String s = "executeCommand(cmd={cmd},cmdString={cmdString})";
  s.replace("{cmd}", cmdString);
  s.replace("{cmdString}", cmdString);
  logInfo(s);
  String result = "";
  if (!isOperative)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "Attach Serials first (Serial or WebSerial)", "executeCommand");
    return "ERROR";
  }
  switch (cmd)
  {
  case CMD::LED_ON:
    ((LedService*) getServiceByCollector("LedService"))->changeLed(true,false);
    result = "Led on";
    break;
  case CMD::LED_OFF:
    ((LedService*) getServiceByCollector("LedService"))->changeLed(false,false);
    result = "Led off";
    break;
  case CMD::LED_TOGGLE:
    ((LedService*) getServiceByCollector("LedService"))->changeLed(true,true);
    result = "Led toggle";
    break;
  case CMD::START_PROGRESS_BAR:
    ((LedService *) getServiceByCollector("LedService"))->effectPrograssiveBar(0,100);
    result = "Effect progressive bar blu (WIP)";
    break;
  case CMD::OFF_STRIPT:
    ((LedService *) getServiceByCollector("LedService"))->shotdownLed();
    result = "Stript off (WIP)";
    break;
  case CMD::INFO:
    return getServerIpByCollector();
    // differentSerialprintln(myServer.getIp()); // todo
    break;
  default:
    result = "Comando non riconosciuto: " + cmdString;
    break;
  }
  return result;
}

String CommandService::recvMsgAndExecute(String data)
{
  String s = "recvMsgAndExecute(data={data})";
  s.replace("{data}", data);
  logInfo(s);
  // WebSerial.println("Received Data...");
  if (!isOperative)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "Service not inizializer. attach serial and webSerial pointers with attachSerial method or use constructor with param non null", "recvMsgAndExecute");
    return "ERROR";
  }
  String result = executeCommand(mapStringToEnum(data), data);
  differentSerialprintln(result, "", serialPointer, webSerialPointer);
  return result;
}