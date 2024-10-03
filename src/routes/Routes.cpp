#include "Routes.h"

void initRoutes(MastroServer &srv)
{
  srv.setCustomApi("/api/sendCommand", HTTP_POST, commandController);
  srv.setCustomApi("/api/checkOk", HTTP_GET, getOk);
  srv.setCustomApi("/api/ws2811SetEffect", HTTP_POST, setEffectWs2811);
  srv.setCustomApi("/api/ws2811StopEffect", HTTP_POST, stopEffectWs2811);
  srv.setCustomApi("/api/saveSettings",HTTP_POST, saveSettings);
}
