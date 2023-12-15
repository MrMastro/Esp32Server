#include "Services.h"
#include "model/response/BasicResponse.h"
#include <constants/constants.h>
#include <model/DataModelling.h>

String pong()
{
    StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
    BasicResponse response = BasicResponse(s);
    return dtoToJson(response);
}