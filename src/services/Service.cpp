#include "Service.h"
#include "ServicesCollector/ServicesCollector.h"

String Service::executeJson(String methodName, std::vector<String> jsonParams)
{
  return executeJson("","");
}

void Service::setNameService(String name)
{
    nameService = name;
}

String Service::getNameService()
{
    return nameService;
}

String Service::executeJson(String methodName, String param)
{
  throwError(ERROR_CODE::SERVICE_NOT_IMPLEMENTED,"please create the implementation of this class in the <ServiceImplementations> directory", "executeJson");
  return "ERROR";
}

void Service::attachCollector(ServicesCollector *collectorParam)
{
  collector = collectorParam;
}

boolean Service::isAvaible()
{
    throwError(ERROR_CODE::SERVICE_NOT_IMPLEMENTED,"please create the implementation of this class in the <ServiceImplementations> directory and implements methot isAvaible()", "executeJson");
    return false;
}

void Service::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
}

boolean Service::attachPin(int pin)
{
  logInfo("attach pin don't necessary, this procedure is ignored, if your service use a single pin implements method attachPin(int pin)");
  return false;
}

String Service::executeMethodByCollector(String nameService, String nameMethod, String param)
{
  if (collector == nullptr)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "collector pointer of this service is null", "executeMethodByCollector");
    return "ERROR";
  }
  return collector->executeMethod(nameService, nameMethod, param);
}

void Service::throwError(ERROR_CODE err, const char *detailMessage, String context)
{
  logError(getError(err,detailMessage), context);
}

void Service::logInfo(String msg)
{
  String log = "[ LOG - SERVICE {nameService} ] {msg}";
  log.replace("{nameService}", nameService);
  log.replace("{msg}", msg);
  differentSerialprintln(log, serialPointer, webSerialPointer);
}

void Service::logWarning(String msg, String context)
{
  String log = "[ WARNING - SERVICE {nameService} on {context} ] {msg}";
  log.replace("{nameService}", nameService);
  log.replace("{context}", context);
  log.replace("{msg}", msg);
  differentSerialprintln(log, serialPointer, webSerialPointer);
}

void Service::logError(String msg, String context)
{
  String error = "[ ERROR - SERVICE {nameService} on {context} ] {msg}";
  error.replace("{nameService}", nameService);
  error.replace("{context}", context);
  error.replace("{msg}", msg);
  differentSerialprintln(error, serialPointer, webSerialPointer);
}
