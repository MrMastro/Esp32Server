#include "Service.h"
#include "ServicesCollector/ServicesCollector.h"
#include <cstdarg>

// String Service::executeJson(String methodName, std::vector<String> jsonParams)
// {
//   return executeJson("","");
// }

void Service::setNameService(String name)
{
  nameService = name;
}

String Service::getNameService()
{
  return nameService;
}

// String Service::executeJson(String methodName, String param)
// {
//   throwError(ERROR_CODE::SERVICE_NOT_IMPLEMENTED,"please create the implementation of this class in the <ServiceImplementations> directory", "executeJson");
//   return "ERROR";
// }

void Service::attachCollector(ServicesCollector *collectorParam)
{
  collector = collectorParam;
}

boolean Service::isAvaible()
{
  throwError(ERROR_CODE::SERVICE_NOT_IMPLEMENTED, "please create the implementation of isAvaible() in this service ", "isAvaible");
  return false;
}

void Service::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
}

boolean Service::attachPin(std::vector<int> values)
{
  pins = values;
  preparePin();
  return true;
}

boolean Service::preparePin()
{
  logInfoln("prepare pin don't necessary, override this method and use pins vector");
  return false;
}

// String Service::executeMethodByCollector(String nameService, String nameMethod, String param)
// {
//   if (collector == nullptr)
//   {
//     throwError(ERROR_CODE::SERVICE_ERROR, "collector pointer of this service is null", "executeMethodByCollector");
//     return "ERROR";
//   }
//   return collector->executeMethod(nameService, nameMethod, param);
// }

Service *Service::getServiceByCollector(String nameService)
{
  if (collector == nullptr)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "attach serviceCollector first", "getServiceByCollector");
    return nullptr;
  }
  return collector->getService(nameService);
}

String Service::getServerIpByCollector()
{
  MastroServer *pointer = collector->getServer();
  if (pointer == nullptr)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "server point is null", "getServer");
    return "ERROR";
  }
  return pointer->getIp();
}

MastroServer *Service::getServerByCollector()
{
  MastroServer *pointer = collector->getServer();
  if (pointer == nullptr)
  {
    throwError(ERROR_CODE::SERVICE_ERROR, "server point is null", "getServer");
    return nullptr;
  }
  return pointer;
}

void Service::throwError(ERROR_CODE err, const char *detailMessage, String context)
{
  logError(getError(err, detailMessage), context);
}

void Service::logInfoln(String msg)
{
  if (settings.debug)
  {
    String log = "[ LOG - SERVICE {nameService} ] {msg}";
    log.replace("{nameService}", nameService);
    log.replace("{msg}", msg);
    differentSerialprintln(log, "\033[32m", serialPointer, webSerialPointer); // set green color
  }
}

void Service::logWarning(String msg, String context)
{
  String log = "[ WARNING - SERVICE {nameService} on {context} ] {msg}";
  log.replace("{nameService}", nameService);
  log.replace("{context}", context);
  log.replace("{msg}", msg);
  differentSerialprintln(log, "\033[33m", serialPointer, webSerialPointer);
}

void Service::logError(String msg, String context)
{
  String error = "[ ERROR - SERVICE {nameService} on {context} ] {msg}";
  error.replace("{nameService}", nameService);
  error.replace("{context}", context);
  error.replace("{msg}", msg);
  differentSerialprintln(error, "\033[31m", serialPointer, webSerialPointer);
}
