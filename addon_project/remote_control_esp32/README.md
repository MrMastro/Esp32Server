# remote_control_esp32

Telecomando basato su **Waveshare ESP32-C6-LCD-1.3** (ESP32-C6, display 1.3" 240x240 ST7789V2, LED RGB WS2812B, slot microSD, 4MB flash).

Progetto PlatformIO (piattaforma pioarduino, Arduino core 3.x).

## Build e upload

- Compilare da **PowerShell** o dai pulsanti PlatformIO di VS Code, **non da Git Bash**: lo script `idf_tools.py` di pioarduino rifiuta gli ambienti MSYS/MinGW e la toolchain non viene installata.
- Upload via USB-C. Se la porta non viene vista: tenere premuto BOOT, premere RST, rilasciare BOOT.

## Pin della scheda

### Occupati dall'hardware di bordo (non usare)

| GPIO | Uso |
| --- | --- |
| 4 | microSD CS |
| 5 | SPI MISO (microSD) |
| 6 | SPI MOSI (display + microSD) |
| 7 | SPI SCLK (display + microSD) |
| 8 | LED RGB WS2812B |
| 9 | Pulsante BOOT |
| 12, 13 | USB D-/D+ (seriale e upload) |
| 14 | LCD CS |
| 15 | LCD DC |
| 21 | LCD RST |
| 22 | LCD retroilluminazione |

Il display e la microSD condividono lo stesso bus SPI (GPIO 5/6/7) con CS separati.

### Connettore esterno a 9 pin

GPIO esposti (letti sulla serigrafia della scheda): **1, 2, 3, 12, 13, 16, 17, 19 (?), 23**

- 12, 13: USB, usarli disabilita la seriale USB e l'upload.
- 16, 17: UART0 TX/RX.
- Il pin letto come "29" è da verificare (sull'ESP32-C6 i GPIO 24-30 sono riservati alla flash: probabilmente è 19 o 20).
- ADC disponibile solo su GPIO 0-6 → sul connettore: 1, 2, 3.

## Joystick analogico

| Joystick | Scheda |
| --- | --- |
| SW | **GPIO 1** (digitale, `INPUT_PULLUP`) |
| VRy | **GPIO 2** (ADC) |
| VRx | **GPIO 3** (ADC) |
| VCC | **3V3** (non 5V: l'ADC accetta al massimo ~3.3V) |
| GND | GND |

ADC a 12 bit (0-4095), centro tipico ~1930/1960, zona morta 400.

Mappatura assi ereditata dal progetto principale (`Esp32Server/src/Main.cpp`):

- asse X → UP / DOWN
- asse Y → RIGHT / LEFT
- il pulsante SW fa da **toggle**: quando è attivo, al comando si aggiunge il suffisso `_BUTTON`.

## microSD

Formattare in **FAT32** da Windows (preferibilmente schede ≤ 32GB). exFAT non è garantito.

Le configurazioni JSON stanno nella cartella **`/config`** della microSD (es. `/config/joystick.json`).
Non si usa la cartella `data/` di PlatformIO né il filesystem in flash (LittleFS/SPIFFS): per cambiare
una configurazione basta modificare il file sulla SD dal PC, senza ricaricare nulla sulla scheda.
