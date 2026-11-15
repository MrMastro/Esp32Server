#ifndef BOARD_PINS_H
#define BOARD_PINS_H

// Waveshare ESP32-C6-LCD-1.3 - pin dell'hardware di bordo (vedi README.md)

// Bus SPI condiviso (display + microSD)
#define PIN_SPI_MISO 5
#define PIN_SPI_MOSI 6
#define PIN_SPI_SCLK 7

// Display ST7789V2 240x240
#define PIN_LCD_CS  14
#define PIN_LCD_DC  15
#define PIN_LCD_RST 21
#define PIN_LCD_BL  22
#define LCD_WIDTH   240
#define LCD_HEIGHT  240

// microSD
#define PIN_SD_CS 4

// Joystick: valori di default, sovrascrivibili da /config/joystick.json
#define DEFAULT_PIN_JOY_SW 1
#define DEFAULT_PIN_JOY_VRY 2
#define DEFAULT_PIN_JOY_VRX 3

#endif // BOARD_PINS_H
