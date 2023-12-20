#include "Service.h"
#include "ServicesCollector/ServicesCollector.h"

// void Service::attachCollector(ServicesCollector *collectorParam)
// {
//   collector = collectorParam;
// }

void Service::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
    serialPointer = serialPointerParam;
    webSerialPointer = webSerialPointerParam;
}

// String Service::executeMethodByCollector(String nameService, String nameMethod, String param)
// {
//     return collector->executeMethod(nameService,nameMethod,param);
// }

void Service::throwError(ERROR_CODE err, const char* detailMessage) {
  String result = "Error: ";
  String statusStr = ERROR_MAP.at(err);
  std::vector<String> parts = splitString(statusStr, ',');
  for (const auto& el : parts) {
    result+= (" - "+el);
  }
  differentSerialprintln(result, serialPointer, webSerialPointer);
}
