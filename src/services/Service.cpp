#include "Service.h"
#include "ServicesCollector/ServicesCollector.h"

String Service::getClassName() const
{
    return String();
}

void Service::attachCollector(ServicesCollector *collectorParam)
{
  collector = collectorParam;
}

boolean Service::isAvaible()
{
    return true;
}

void Service::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
}

boolean Service::attachPin(int pin)
{
  logInfo("attach pin don't necessary, this procedure is ignored");
  return false;
}

String Service::executeMethodByCollector(String nameService, String nameMethod, String param)
{
  if (collector == nullptr)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "collector pointer of this service is null");
    return "ERROR";
  }
  return collector->executeMethod(nameService, nameMethod, param);
}

void Service::throwError(ERROR_CODE err, const char *detailMessage)
{
  logError(getError(err,detailMessage));
}

void Service::logInfo(String msg)
{
  String log = "[ LOG - SERVICE {nameService} ] {msg}";
  log.replace("{nameService}", getClassName());
  log.replace("{msg}", msg);
  differentSerialprintln(log, serialPointer, webSerialPointer);
}

void Service::logError(String msg)
{
  String error = "[ ERROR - SERVICE {nameService} ] {msg}";
  error.replace("{nameService}", getClassName());
  error.replace("{msg}", msg);
  differentSerialprintln(error, serialPointer, webSerialPointer);
}
