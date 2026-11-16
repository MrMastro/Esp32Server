#include "StatusBar.h"

#include "BoardPins.h"
#include "display/Display.h"

static const WifiStatus *wifiStatus = nullptr;
static NETWORK_STATE shownState = NETWORK_STATE::OFF;

void statusBarBegin(const WifiStatus *status)
{
  wifiStatus = status;
}

static NETWORK_STATE currentState()
{
  return wifiStatus ? wifiCurrentState(*wifiStatus) : NETWORK_STATE::OFF;
}

bool statusBarChanged()
{
  return currentState() != shownState;
}

void statusBarDraw()
{
  shownState = currentState();

  const char *label;
  uint16_t color;
  switch (shownState)
  {
  case NETWORK_STATE::AP:
    label = "AP";
    color = RGB565_CYAN;
    break;
  case NETWORK_STATE::WIFI_CONNECTED:
    label = "WIFI";
    color = RGB565_GREEN;
    break;
  case NETWORK_STATE::WIFI_DISCONNECTED:
    label = "WIFI";
    color = RGB565_RED;
    break;
  default:
    label = "OFF";
    color = RGB565_DARKGREY;
    break;
  }

  // testo dimensione 2 (12x16 px per carattere), allineato a destra con margine di 6px
  const uint8_t size = 2;
  const int16_t w = strlen(label) * 6 * size;
  const int16_t x = LCD_WIDTH - 6 - w;
  const int16_t y = 10;

  gfx->fillRect(x - 4, y - 3, w + 8, 8 * size + 6, RGB565_BLACK);
  gfx->drawRoundRect(x - 4, y - 3, w + 8, 8 * size + 6, 4, color);
  gfx->setTextSize(size);
  gfx->setTextColor(color);
  gfx->setCursor(x, y);
  gfx->print(label);
}

int16_t statusBarLeftEdge()
{
  // stesso calcolo di statusBarDraw() con l'etichetta più lunga ("WIFI", dimensione 2)
  const int16_t widest = 4 * 6 * 2;
  return LCD_WIDTH - 6 - widest - 4;
}
