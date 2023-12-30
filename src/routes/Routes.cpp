#include "Routes.h"

void initRoutes(MastroServer &srv)
{
  srv.setCustomApi("/api/sendCommand", HTTP_POST, commandController);
  srv.setCustomApi("/api/checkOk", HTTP_GET, getOk);
  srv.setCustomApi("/api/ws2811SetEffect", HTTP_POST, setEffectWs2811);
}
