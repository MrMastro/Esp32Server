#include "JoystickDisplayTest.h"

#include "BoardPins.h"
#include "display/Display.h"

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

void testJoystickSetNetworkInfo(const String &info)
{
  networkInfo = info;
  firstFrame = true;
}

static void drawFrame(Joystick &joystick, J_DIRECTION direction)
{
  gfx->fillScreen(RGB565_BLACK);

  // intestazione
  gfx->setTextSize(2);
  gfx->setTextColor(RGB565_CYAN);
  gfx->setCursor(10, 10);
  gfx->print("JOYSTICK TEST");
  gfx->setTextSize(1);
  gfx->setTextColor(RGB565_DARKGREY);
  gfx->setCursor(10, 33);
  gfx->printf("centro X:%d Y:%d", joystick.centerX(), joystick.centerY());
  gfx->setTextColor(RGB565_YELLOW);
  gfx->setCursor(10, 45);
  gfx->print(networkInfo);
  gfx->drawFastHLine(0, 60, LCD_WIDTH, RGB565_DARKGREY);

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

  // valori grezzi e stato del tasto
  gfx->setTextSize(2);
  gfx->setTextColor(RGB565_WHITE);
  gfx->setCursor(10, 175);
  gfx->printf("X:%4d  Y:%4d", joystick.rawX(), joystick.rawY());
  gfx->setCursor(10, 200);
  gfx->printf("SW:%s", joystick.switchPressed() ? "ON" : "OFF");
  gfx->setTextColor(joystick.buttonMode() ? RGB565_ORANGE : RGB565_DARKGREY);
  gfx->setCursor(110, 200);
  gfx->print(joystick.buttonMode() ? "BUTTON" : "-");

  displayFlush();
}

void testJoystickOnDisplay(Joystick &joystick)
{
  // la lettura avviene a ogni giro di loop, così nessuna pressione viene persa
  J_DIRECTION direction = joystick.read();

  if (joystick.changed())
  {
    Serial.println("[Test] Joystick direction: " + mapJdirectionsToString(direction));
  }

  uint32_t now = millis();
  if (now - lastFrameMs < FRAME_MS)
  {
    return;
  }

  bool dirty = firstFrame || direction != shownDirection || joystick.switchPressed() != shownSwitch ||
               abs(joystick.rawX() - shownX) > RAW_CHANGE_THRESHOLD ||
               abs(joystick.rawY() - shownY) > RAW_CHANGE_THRESHOLD;
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
  drawFrame(joystick, direction);
}
