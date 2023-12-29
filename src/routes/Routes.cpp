#include "Routes.h"

void initRoutes(MastroServer &srv)
{
  srv.setCustomApi("/api/sendCommand", HTTP_POST, commandController);
  srv.setCustomApi("/api/checkOk", HTTP_GET, getOk);
}
