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

void CommandService::executeCommand(CMD cmd, String cmdString)
{
  if(!isOperative){
    return;
  }
  switch (cmd)
  {
  case CMD::LED_ON:
    // activeLed(true, false); //todo
    differentSerialprintln(String("Led on"), &Serial, &WebSerial);
    break;
  case CMD::LED_OFF:
    // activeLed(false, false); //todo
    differentSerialprintln("Led off", &Serial, &WebSerial);
    break;
  case CMD::LED_TOGGLE:
    // activeLed(true, true); //todo
    differentSerialprintln("Led toggle", &Serial, &WebSerial);
    break;
  case CMD::INFO:
    // differentSerialprintln(myServer.getIp()); // todo
    break;
  default:
    differentSerialprintln("Tasto non riconosciuto: " + cmdString, &Serial, &WebSerial);
    break;
  }
}

void CommandService::recvMsgAndExecute(uint8_t *data, size_t len)
{
  // WebSerial.println("Received Data...");
  if(!isOperative){
    throwError(ERROR_CODE::SERVICE_ERROR, "Service not start correctly");
    return;
  }
  String d = "";
  for (int i = 0; i < len; i++)
  {
    d += char(data[i]);
  }
  if (d.length() > 0)
  {
    executeCommand(mapStringToEnum(d), d);
  }
}