#include "Display.h"
#include "BoardPins.h"

// 80MHz come nel demo Waveshare (la microSD usa la sua frequenza nelle proprie transazioni)
#define LCD_SPI_FREQ 80000000

// Bus SPI hardware condiviso con la microSD (is_shared_interface = true)
static Arduino_DataBus *bus = new Arduino_HWSPI(PIN_LCD_DC, PIN_LCD_CS, PIN_SPI_SCLK, PIN_SPI_MOSI, PIN_SPI_MISO, &SPI, true);

// Pannello fisico: rotation = 0, IPS = true (colori invertiti), nessun offset
static Arduino_GFX *panel = new Arduino_ST7789(bus, PIN_LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT, 0, 0, 0, 0);

// Buffer in RAM (240*240*2 = ~113KB) su cui si disegna
Arduino_GFX *gfx = new Arduino_Canvas(LCD_WIDTH, LCD_HEIGHT, panel);

bool displayBegin()
{
  // CS della SD alto, così la scheda non risponde mentre si inizializza il display
  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);

  if (!gfx->begin(LCD_SPI_FREQ))
  {
    Serial.println("[Display] begin() fallito (memoria per il buffer?)");
    return false;
  }

  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, HIGH);
  gfx->fillScreen(RGB565_BLACK);
  displayFlush();
  return true;
}

void displayFlush()
{
  gfx->flush();
}

void displayPrintCentered(const String &text, int16_t y, uint8_t size, uint16_t color)
{
  // font di default: 6x8 px per carattere
  int16_t w = text.length() * 6 * size;
  gfx->setTextSize(size);
  gfx->setTextColor(color);
  gfx->setCursor((LCD_WIDTH - w) / 2, y);
  gfx->print(text);
}

void displayMessage(const String &title, const String &subtitle)
{
  gfx->fillScreen(RGB565_BLACK);
  displayPrintCentered(title, 95, 3, RGB565_WHITE);
  if (subtitle.length() > 0)
  {
    displayPrintCentered(subtitle, 135, 2, RGB565_LIGHTGREY);
  }
  displayFlush();
}
