#ifndef DataModelling_H
#define DataModelling_H

#include <ArduinoJson.h>
#include <iostream>
#include "response/BasicResponse/BasicResponse.h"
#include "response/BasicResponseString/BasicResponseString.h"

boolean jsonToSimpleBoolean(String json);
String simpleBooleanToJson(boolean paramBool);

String dtoToJson(BasicResponse &data);
String dtoToJson(BasicResponseString data);
String dtoToJson(StatusInfo &info);
BasicResponse jsonToDto(String &json);
StatusInfo jsonToStatusInfo(String &json);

#endif  // DataModelling_H