#include <ArduinoJson.h>
#include <iostream>
#include <map>
#include "response/BasicResponse.h"

#ifndef DataModelling_H
#define DataModelling_H

String dtoToJson(BasicResponse &data);
String dtoToJson(StatusInfo &info);
BasicResponse jsonToDto(String &json);
StatusInfo jsonToStatusInfo(String &json);

#endif  // DataModelling_H