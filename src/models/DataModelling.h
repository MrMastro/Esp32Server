#include <ArduinoJson.h>
#include <iostream>
#include <map>
#include "response/BasicResponse.h"

#ifndef DataModelling_H
#define DataModelling_H

boolean jsonToSimpleBoolean(String json);
String simpleBooleanToJson(boolean paramBool);

String dtoToJson(BasicResponse &data);
String dtoToJson(StatusInfo &info);
BasicResponse jsonToDto(String &json);
StatusInfo jsonToStatusInfo(String &json);

#endif  // DataModelling_H