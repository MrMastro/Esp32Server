#include "./services/Service.h"
#include "./constants/constants.h"
#include <models/DataModelling.h>
#include "CommandService.h"
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>

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
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
  isOperative = true;
}

String CommandService::executeJson(String methodName, String param)
{
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
  if (methodName == "recvMsgAndExecute")
  {
    return recvMsgAndExecute(jsonParams.at(0));
  }
  else
  {
    return "Service Method not found";
  }
}

String CommandService::getClassName() const
{
  return "CommandService";
}

String CommandService::executeCommand(CMD cmd, String cmdString)
{
  String result = "";
  if (!isOperative)
  {
    return "Error";
  }
  switch (cmd)
  {
  case CMD::LED_ON:
    result = "Led on";
    executeMethodByCollector("LedService","changeLed",simpleBooleanToJson(true));
    break;
  case CMD::LED_OFF:
    executeMethodByCollector("LedService","changeLed",simpleBooleanToJson(false));
    result = "Led off";
    break;
  case CMD::LED_TOGGLE:
    // activeLed(true, true); //todo
    result = "Led toggle";
    break;
  case CMD::INFO:
    // differentSerialprintln(myServer.getIp()); // todo
    break;
  default:
    result = "Tasto non riconosciuto: " + cmdString, serialPointer, webSerialPointer;
    break;
  }
  return result;
}

String CommandService::recvMsgAndExecute(String data)
{
  // WebSerial.println("Received Data...");
  if (!isOperative)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "Service not inizializer. attach serial and webSerial pointers with attachSerial method or use constructor with param non null");
    return "ERROR";
  }
  String result = executeCommand(mapStringToEnum(data), data);
  differentSerialprintln(result, serialPointer, webSerialPointer);
  return result;
}