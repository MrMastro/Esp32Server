#include "Routes.h"

void initRoutes(MastroServer &srv)
{
  // comando come parametro (?command=... o form) oppure body JSON {"command": "..."}
  srv.setCustomApi("/api/v1/sendCommand", HTTP_POST, commandController, commandControllerBody);
  srv.setCustomApi("/api/v1/checkOk", HTTP_GET, getOk);
  srv.setCustomApi("/api/v1/ws2811SetEffect", HTTP_POST, setEffectWs2811);
  srv.setCustomApi("/api/v1/ws2811StopEffect", HTTP_POST, stopEffectWs2811);
  srv.setCustomApi("/api/v2/ws2811SetEffect", HTTP_POST, setEffectWs2811_v2);
  srv.setCustomApi("/api/v2/ws2811StopEffect", HTTP_POST, stopEffectWs2811_v2);
  srv.setCustomApi("/api/v1/getAvaibleEffects", HTTP_GET, getAvaibleEffects);
  srv.setCustomApi("/api/v1/saveSettings",HTTP_POST, saveSettings);
  srv.setCustomApi("/api/v1/getSettings",HTTP_GET, getJsonSettings);
  srv.setCustomApi("/api/v1/login",HTTP_POST, login);
  srv.setCustomApi("/api/v1/saveInitialEffect",HTTP_POST, saveInitialSettings);
  srv.setCustomApi("/api/v1/getInfoHello",HTTP_GET, getInfoHello);
  srv.setCustomApi("/api/v1/remote/pair",HTTP_POST, remotePair);
}
