#include "./services/Service.h"
#include "models/response/BasicResponse.h"
#include <constants/constants.h>
#include <models/DataModelling.h>

String pong()
{
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    BasicResponse response = BasicResponse(s);
    return dtoToJson(response);
}

String pongSuccess()
{
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    BasicResponse response = BasicResponse(s);
    return dtoToJson(response);
}