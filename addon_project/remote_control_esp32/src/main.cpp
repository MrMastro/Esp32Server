#include <Arduino.h>
#include <Arduino_GFX_Library.h>

// Pinout Waveshare ESP32-C6-LCD-1.3 (dal demo ufficiale Waveshare)
#define LCD_MOSI 6
#define LCD_SCLK 7
#define LCD_CS   14
#define LCD_DC   15
#define LCD_RST  21
#define LCD_BL   22

#define LCD_WIDTH  240
#define LCD_HEIGHT 240

Arduino_DataBus *bus = new Arduino_HWSPI(LCD_DC, LCD_CS, LCD_SCLK, LCD_MOSI, GFX_NOT_DEFINED);

// rotation = 0, IPS = true (inversione colori attiva), nessun offset
Arduino_GFX *gfx = new Arduino_ST7789(bus, LCD_RST, 0, true, LCD_WIDTH, LCD_HEIGHT, 0, 0, 0, 0);

void setup()
{
    Serial.begin(115200);

    if (!gfx->begin())
    {
        Serial.println("gfx->begin() fallito!");
    }

    pinMode(LCD_BL, OUTPUT);
    digitalWrite(LCD_BL, HIGH); // retroilluminazione accesa

    gfx->fillScreen(RGB565_BLACK);

    const char *msg = "Hello World";
    gfx->setTextSize(3);
    gfx->setTextColor(RGB565_WHITE);

    int16_t x1, y1;
    uint16_t w, h;
    gfx->getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);
    gfx->setCursor((LCD_WIDTH - w) / 2, (LCD_HEIGHT - h) / 2);
    gfx->print(msg);

    Serial.println("Hello World a schermo");
}

void loop()
{
    delay(1000);
}
