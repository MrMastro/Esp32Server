#include "Service.h"
#include "ServicesCollector/ServicesCollector.h"

void Service::attachCollector(ServicesCollector *collectorParam)
{
  collector = collectorParam;
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
  String result = "";
  String statusStr = ERROR_MAP.at(err);
  std::vector<String> parts = splitString(statusStr, ',');
  for (const auto &el : parts)
  {
    result += (" - " + el);
  }
  logError(result);
}

void Service::logInfo(String msg)
{
  differentSerialprintln(msg, serialPointer, webSerialPointer);
}

void Service::logError(String msg)
{
  String error = "ERROR: " + msg;
  differentSerialprintln(msg, serialPointer, webSerialPointer);
}
