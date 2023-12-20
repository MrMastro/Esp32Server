#include "Service.h"

void Service::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
    serialPointer = serialPointerParam;
    webSerialPointer = webSerialPointerParam;
}

void Service::throwError(ERROR_CODE err, const char* detailMessage) {
  String result = "Error: ";
  String statusStr = ERROR_MAP.at(err);
  std::vector<String> parts = splitString(statusStr, ',');
  for (const auto& el : parts) {
    result+= (" - "+el);
  }
  differentSerialprintln(result, serialPointer, webSerialPointer);
}
