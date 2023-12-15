#include "Services.h"
#include "model/response/BasicResponse.h"
#include <constants/constants.h>
#include <model/DataModelling.h>
#include "CommandService.h"
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>

CommandService::CommandService()
{
  isOperative = false;
}

CommandService::CommandService(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
  isOperative = true;
}

void CommandService::attach(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
  isOperative = true;
}

String CommandService::executeCommand(CMD cmd, String cmdString)
{
  String result ="";
  if(!isOperative){
    return;
  }
  switch (cmd)
  {
  case CMD::LED_ON:
    // activeLed(true, false); //todo
    result = "Led on";
    break;
  case CMD::LED_OFF:
    // activeLed(false, false); //todo
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
  if(!isOperative){
    throwError(ERROR_CODE::SERVICE_ERROR, "Service not inizializer. attach serial and webSerial pointers with attach or use constructor with param non null");
    return "ERROR";
  }
  String result = executeCommand(mapStringToEnum(data), data);
  differentSerialprintln(result, serialPointer, webSerialPointer);
  return result;
}