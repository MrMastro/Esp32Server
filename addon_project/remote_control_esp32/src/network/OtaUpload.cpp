#include "OtaUpload.h"
#include "system/Log.h"

#include <ArduinoOTA.h>
#include "BoardPins.h"
#include "display/Display.h"

static bool enabled = false;
static int lastShownPercent = -1;

static void drawProgress(int percent)
{
  gfx->fillScreen(RGB565_BLACK);
  displayPrintCentered("OTA", 60, 4, RGB565_CYAN);
  displayPrintCentered(String(percent) + "%", 115, 3, RGB565_WHITE);
  // barra di avanzamento
  const int16_t x = 20, y = 160, w = LCD_WIDTH - 40, h = 16;
  gfx->drawRect(x, y, w, h, RGB565_WHITE);
  gfx->fillRect(x + 2, y + 2, (w - 4) * percent / 100, h - 4, RGB565_CYAN);
  displayFlush();
}

void otaBegin(const SettingsModel &settings)
{
  enabled = settings.otaUpload;
  if (!enabled)
  {
    logInfo("[OTA] disattivato (ota_upload = false)");
    return;
  }

  ArduinoOTA.setPort(3232);
  ArduinoOTA.setHostname(settings.deviceName.c_str());
  ArduinoOTA.setPassword(settings.devicePassword.c_str());

  ArduinoOTA
      .onStart([]()
               {
                 logInfo("[OTA] inizio aggiornamento");
                 displaySetOn(true); // l'avanzamento si vede anche se lo schermo era spento
                 lastShownPercent = -1;
                 drawProgress(0);
               })
      .onProgress([](unsigned int progress, unsigned int total)
                  {
                    int percent = total ? (progress * 100) / total : 0;
                    // ridisegna solo ogni 2% per non rallentare la ricezione
                    if (percent - lastShownPercent >= 2 || percent == 100)
                    {
                      lastShownPercent = percent;
                      drawProgress(percent);
                    }
                  })
      .onEnd([]()
             {
               logInfo("[OTA] completato, riavvio");
               displayMessage("OTA OK", "riavvio...");
             })
      .onError([](ota_error_t error)
               {
                 logWarningf("[OTA] errore %u\n", error);
                 displayMessage("OTA errore", String("codice ") + error);
                 delay(2000);
               });

  ArduinoOTA.begin();
  logInfo("[OTA] attivo, hostname: " + settings.deviceName);
}

void otaHandle()
{
  if (enabled)
  {
    ArduinoOTA.handle();
  }
}

bool otaIsEnabled()
{
  return enabled;
}
