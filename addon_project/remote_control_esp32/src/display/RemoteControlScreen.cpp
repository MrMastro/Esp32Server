#include "RemoteControlScreen.h"

#include "BoardPins.h"
#include "display/Display.h"
#include "display/StatusBar.h"
#include "display/DeviceDots.h"

// Ridisegna al massimo ogni FRAME_MS, e solo se qualcosa è cambiato
static const uint32_t FRAME_MS = 33; // ~30 fps
// I valori grezzi oscillano di poco anche a riposo: si ridisegna solo oltre questa soglia
static const int RAW_CHANGE_THRESHOLD = 20;

static uint32_t lastFrameMs = 0;
static bool firstFrame = true;
static J_DIRECTION shownDirection = J_DIRECTION::NONE;
static bool shownSwitch = false;
static int shownX = -1000;
static int shownY = -1000;
static String networkInfo = "";

void remoteControlScreenSetNetworkInfo(const String &info)
{
  networkInfo = info;
  firstFrame = true;
}

static void drawFrame(Joystick &joystick, J_DIRECTION direction, const RemoteControlScreenOptions &options)
{
  gfx->fillScreen(RGB565_BLACK);

  // intestazione: IP (e centro calibrato) a sinistra, WIFI/AP a destra
  statusBarDraw();
  gfx->setTextSize(1);
  gfx->setTextColor(RGB565_YELLOW);
  gfx->setCursor(8, 10);
  gfx->print(networkInfo);
  if (options.showRawValues)
  {
    gfx->setTextColor(RGB565_DARKGREY);
    gfx->setCursor(8, 22);
    gfx->printf("centro X:%d Y:%d", joystick.centerX(), joystick.centerY());
  }

  // riga dei dispositivi conosciuti (fino a 10 pallini), a tutta larghezza
  deviceDotsDraw(46, 6, LCD_WIDTH - 6);
  gfx->drawFastHLine(0, 62, LCD_WIDTH, RGB565_DARKGREY);

  // direzione: dimensione del carattere più grande che entra in larghezza
  String text = mapJdirectionsToString(direction);
  uint8_t size = 4;
  while (size > 1 && (int)text.length() * 6 * size > LCD_WIDTH - 10)
  {
    size--;
  }
  uint16_t color = joystick.buttonMode() ? RGB565_ORANGE : RGB565_GREEN;
  displayPrintCentered(text, 110 - 4 * size, size, color);

  gfx->drawFastHLine(0, 160, LCD_WIDTH, RGB565_DARKGREY);

  // valori grezzi (opzionali) e stato del tasto
  gfx->setTextSize(2);
  int16_t switchY = 175;
  if (options.showRawValues)
  {
    gfx->setTextColor(RGB565_WHITE);
    gfx->setCursor(10, 175);
    gfx->printf("X:%4d  Y:%4d", joystick.rawX(), joystick.rawY());
    switchY = 200;
  }
  gfx->setTextColor(RGB565_WHITE);
  gfx->setCursor(10, switchY);
  gfx->printf("SW:%s", joystick.switchPressed() ? "ON" : "OFF");
  gfx->setTextColor(joystick.buttonMode() ? RGB565_ORANGE : RGB565_DARKGREY);
  gfx->setCursor(110, switchY);
  gfx->print(joystick.buttonMode() ? "BUTTON" : "-");

  if (options.bottomLabel != nullptr)
  {
    displayPrintCentered(options.bottomLabel, 222, 2, RGB565_CYAN);
  }

  displayFlush();
}

void remoteControlScreenUpdate(Joystick &joystick, J_DIRECTION direction, const RemoteControlScreenOptions &options)
{
  if (options.logDirections && joystick.changed())
  {
    Serial.println("[Joystick] direction: " + mapJdirectionsToString(direction));
  }

  // schermo spento: si continua a leggere il joystick ma non si disegna; alla riaccensione
  // il primo giro ridisegna tutto
  if (!displayIsOn())
  {
    firstFrame = true;
    return;
  }

  uint32_t now = millis();
  if (now - lastFrameMs < FRAME_MS)
  {
    return;
  }

  bool rawChanged = options.showRawValues &&
                    (abs(joystick.rawX() - shownX) > RAW_CHANGE_THRESHOLD ||
                     abs(joystick.rawY() - shownY) > RAW_CHANGE_THRESHOLD);
  bool dirty = firstFrame || statusBarChanged() || deviceDotsChanged() || direction != shownDirection ||
               joystick.switchPressed() != shownSwitch || rawChanged;
  if (!dirty)
  {
    return;
  }

  lastFrameMs = now;
  firstFrame = false;
  shownDirection = direction;
  shownSwitch = joystick.switchPressed();
  shownX = joystick.rawX();
  shownY = joystick.rawY();
  drawFrame(joystick, direction, options);
}

void remoteControlOnDisplay(Joystick &joystick, J_DIRECTION direction, bool showRawValues)
{
  RemoteControlScreenOptions options;
  options.showRawValues = showRawValues;
  remoteControlScreenUpdate(joystick, direction, options);
}
