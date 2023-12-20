#include "exceptions.h"
#include <utils/FunctionUtils.h>
#include <utils/SerialSimple.h>


void throwError(ERROR_CODE err, const char* detailMessage) {
  String result = "Error: ";
  String statusStr = ERROR_MAP.at(err);
  std::vector<String> parts = splitString(statusStr, ',');
  for (const auto& el : parts) {
    result+= (" - "+el);
  }
  Serial.println(result);
}

String getError(ERROR_CODE err, const char* detailMessage) {
  String result = "Error: ";
  String statusStr = ERROR_MAP.at(err);
  std::vector<String> parts = splitString(statusStr, ',');
  for (const auto& el : parts) {
    result+= (" - "+el);
  }
  return result;
}