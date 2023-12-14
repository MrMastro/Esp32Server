#include "Services.h"
#include "model/response/BasicResponse.h"
#include <constants/constants.h>
#include <model/DataModelling.h>

String sendOk()
{
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    Serial.println("Descrizione di s:");
    Serial.println(s.getDescription());
    BasicResponse response = BasicResponse(s);
    Serial.println("Descrizione di response:");
    Serial.println(response.toString());
    return dtoToJson(response);
}