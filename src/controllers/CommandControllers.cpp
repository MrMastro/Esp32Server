#include "CommandControllers.h"
#include "globals.h"

void commandController(AsyncWebServerRequest *request)
{

  String command = request->arg("command");
  globalVariable;
  //servicesCollector.executeMethod("CommandService", "recvMsgAndExecute", command);
  StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::OK);
  BasicResponse response = BasicResponse(s);
  String jsonResponse = dtoToJson(response);
  request->send(200, "application/json", jsonResponse);
}