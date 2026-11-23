#include "RemoteControlScreen.h"
#include "system/Log.h"

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
static const char *modeLabel = nullptr; // in basso a destra: "ESP-NOW" / "API-CALL"
static uint16_t modeLabelColor = RGB565_CYAN;

// Joystick wireless: disconnessione, barra della pressione lunga, messaggio temporaneo
static bool disconnected = false;
static float holdProgress = -1.0f;
static const char *holdLabel = nullptr;
static int shownHoldStep = -1; // barra disegnata (in passi da 1/40): si ridisegna solo se cambia
static String messageLine1 = "";
static String messageLine2 = "";
static uint16_t messageColor = RGB565_WHITE;
static uint32_t messageUntilMs = 0;
static bool messageShown = false;
static const int HOLD_STEPS = 40;
static const char *busyLabel = nullptr;  // slider "lavoro in corso" (nullptr = nascosto)
static int shownSliderStep = -1;
static bool forceNextFrame = false; // ridisegno senza attendere FRAME_MS (comparsa dello slider)
static bool sliderLogged = false;
static const uint32_t SLIDER_PERIOD_MS = 1200; // andata e ritorno del cursore dello slider
static const int SLIDER_STEPS = 40;

void remoteControlScreenSetNetworkInfo(const String &info)
{
  networkInfo = info;
  firstFrame = true;
}

void remoteControlScreenSetModeLabel(const char *label, uint16_t color)
{
  modeLabel = label;
  modeLabelColor = color;
  firstFrame = true;
}

void remoteControlScreenSetDisconnected(bool value)
{
  if (disconnected != value)
    firstFrame = true;
  disconnected = value;
}

void remoteControlScreenSetHold(float progress, const char *label)
{
  holdProgress = progress;
  holdLabel = label;
}

void remoteControlScreenSetBusy(const char *label)
{
  if (label != nullptr && busyLabel == nullptr)
  {
    forceNextFrame = true; // lo slider deve sostituire la barra subito, al giro stesso
    sliderLogged = false;
  }
  busyLabel = label;
}

bool remoteControlScreenBusy()
{
  return busyLabel != nullptr;
}

// Posizione del cursore dello slider (0..SLIDER_STEPS, avanti e indietro); -1 se nascosto
static int sliderStep()
{
  if (busyLabel == nullptr)
    return -1;
  uint32_t phase = millis() % SLIDER_PERIOD_MS;
  uint32_t half = SLIDER_PERIOD_MS / 2;
  uint32_t pos = phase < half ? phase : SLIDER_PERIOD_MS - phase; // 0..half..0
  return (int)(pos * SLIDER_STEPS / half);
}

void remoteControlScreenShowMessage(const String &line1, const String &line2, uint16_t color, uint32_t durationMs)
{
  messageLine1 = line1;
  messageLine2 = line2;
  messageColor = color;
  messageUntilMs = millis() + durationMs;
  messageShown = false;
  firstFrame = true;
}

static int holdStep()
{
  if (holdProgress < 0)
    return -1;
  return (int)(min(holdProgress, 1.0f) * HOLD_STEPS);
}

static void drawMessage()
{
  logInfo("[Screen] t=" + String(millis()) + " messaggio: " + messageLine1 + " " + messageLine2);
  gfx->fillScreen(RGB565_BLACK);
  if (messageLine2.length() == 0)
  {
    displayPrintCentered(messageLine1, 105, 3, messageColor);
  }
  else
  {
    displayPrintCentered(messageLine1, 85, 3, messageColor);
    displayPrintCentered(messageLine2, 125, 3, messageColor);
  }
  displayFlush();
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

  if (disconnected)
    displayPrintCentered("DISCONNESSO", 135, 2, RGB565_RED);

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

  // modalita' di comunicazione in basso al centro (coperta dalla barra / dallo slider della pressione lunga)
  if (modeLabel != nullptr)
    displayPrintCentered(modeLabel, 222, 2, modeLabelColor);

  // pressione lunga di SW in corso: scritta e barra che si riempie fino alla disconnessione / connessione
  int step = holdStep();
  if (step >= 0)
  {
    gfx->fillRect(0, 196, LCD_WIDTH, LCD_HEIGHT - 196, RGB565_BLACK);
    displayPrintCentered(holdLabel != nullptr ? holdLabel : "", 198, 2, RGB565_YELLOW);
    const int16_t barX = 10, barY = 220, barW = LCD_WIDTH - 20, barH = 14;
    gfx->drawRect(barX, barY, barW, barH, RGB565_YELLOW);
    gfx->fillRect(barX + 2, barY + 2, (int16_t)((barW - 4) * step / HOLD_STEPS), barH - 4, RGB565_YELLOW);
  }

  // lavoro in corso (dopo la pressione lunga): stessa scritta e, al posto della barra, uno slider animato
  int slider = sliderStep();
  if (slider >= 0)
  {
    gfx->fillRect(0, 196, LCD_WIDTH, LCD_HEIGHT - 196, RGB565_BLACK);
    displayPrintCentered(busyLabel, 198, 2, RGB565_YELLOW);
    const int16_t barX = 10, barY = 220, barW = LCD_WIDTH - 20, barH = 14, knobW = 50;
    gfx->drawRect(barX, barY, barW, barH, RGB565_YELLOW);
    int16_t knobX = barX + 2 + (int16_t)((barW - 4 - knobW) * slider / SLIDER_STEPS);
    gfx->fillRect(knobX, barY + 2, knobW, barH - 4, RGB565_YELLOW);
  }

  displayFlush();
}

void remoteControlScreenUpdate(Joystick &joystick, J_DIRECTION direction, const RemoteControlScreenOptions &options)
{
  if (options.logDirections && joystick.changed())
  {
    logInfo("[Joystick] direction: " + mapJdirectionsToString(direction));
  }

  // schermo spento: si continua a leggere il joystick ma non si disegna; alla riaccensione
  // il primo giro ridisegna tutto
  if (!displayIsOn())
  {
    firstFrame = true;
    return;
  }

  uint32_t now = millis();
  if (now - lastFrameMs < FRAME_MS && !forceNextFrame)
  {
    return;
  }
  forceNextFrame = false;

  // messaggio temporaneo: copre la schermata finche' non scade, poi ridisegno completo
  if (messageUntilMs != 0)
  {
    if ((int32_t)(now - messageUntilMs) < 0)
    {
      if (!messageShown)
      {
        drawMessage();
        messageShown = true;
        lastFrameMs = now;
      }
      return;
    }
    messageUntilMs = 0;
    firstFrame = true;
  }

  bool rawChanged = options.showRawValues &&
                    (abs(joystick.rawX() - shownX) > RAW_CHANGE_THRESHOLD ||
                     abs(joystick.rawY() - shownY) > RAW_CHANGE_THRESHOLD);
  bool dirty = firstFrame || statusBarChanged() || deviceDotsChanged() || direction != shownDirection ||
               joystick.switchPressed() != shownSwitch || rawChanged || holdStep() != shownHoldStep ||
               sliderStep() != shownSliderStep;
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
  shownHoldStep = holdStep();
  shownSliderStep = sliderStep();
  uint32_t drawStart = millis();
  drawFrame(joystick, direction, options);
  if (busyLabel != nullptr && !sliderLogged)
  {
    sliderLogged = true;
    logInfo("[Screen] t=" + String(drawStart) + " slider a schermo (disegno " + String(millis() - drawStart) + " ms)");
  }
}

void remoteControlOnDisplay(Joystick &joystick, J_DIRECTION direction, bool showRawValues)
{
  RemoteControlScreenOptions options;
  options.showRawValues = showRawValues;
  remoteControlScreenUpdate(joystick, direction, options);
}
