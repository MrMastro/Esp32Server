#include "DeviceDots.h"

#include "BoardPins.h"
#include "display/Display.h"
#include "devices/EspDevices.h"

// 10 pallini (ESP_DEVICES_MAX) = 9*22 + 2*8 = 214 px: entrano nei 240 px dello schermo
static const int16_t DOT_RADIUS = 8;
static const int16_t DOT_SPACING = 22; // distanza tra i centri

static uint32_t shownVersion = UINT32_MAX;

bool deviceDotsChanged()
{
  return espDevicesStateVersion() != shownVersion;
}

// Metà inferiore del cerchio piena (righe dal centro in giù)
static void fillLowerHalfCircle(int16_t cx, int16_t cy, int16_t r, uint16_t color)
{
  for (int16_t dy = 0; dy <= r; dy++)
  {
    int16_t halfWidth = (int16_t)sqrtf((float)(r * r - dy * dy));
    gfx->drawFastHLine(cx - halfWidth, cy + dy, 2 * halfWidth + 1, color);
  }
}

static void drawDot(int16_t cx, int16_t cy, ESP_DEVICE_STATUS status, uint16_t color)
{
  switch (status)
  {
  case ESP_DEVICE_STATUS::CONNECTED:
    gfx->fillCircle(cx, cy, DOT_RADIUS, color);
    break;
  case ESP_DEVICE_STATUS::ONLINE:
    fillLowerHalfCircle(cx, cy, DOT_RADIUS, color);
    gfx->drawCircle(cx, cy, DOT_RADIUS, color);
    break;
  default: // OFFLINE
    gfx->drawCircle(cx, cy, DOT_RADIUS, color);
    break;
  }
}

void deviceDotsDraw(int16_t centerY, int16_t left, int16_t right)
{
  shownVersion = espDevicesStateVersion();

  const std::vector<EspDevice> &devices = espDevicesList();
  const std::map<String, EspDeviceState> &states = espDevicesState();

  // quanti pallini entrano tra left e right, poi riga centrata in quello spazio
  const int16_t maxDots = (right - left - 2 * DOT_RADIUS) / DOT_SPACING + 1;
  const int16_t count = min<int16_t>((int16_t)devices.size(), maxDots);
  if (count <= 0)
    return;
  const int16_t rowWidth = (count - 1) * DOT_SPACING + 2 * DOT_RADIUS;
  int16_t cx = left + (right - left - rowWidth) / 2 + DOT_RADIUS;

  for (int16_t i = 0; i < count; i++)
  {
    const EspDevice &device = devices[i];
    ESP_DEVICE_STATUS status = ESP_DEVICE_STATUS::OFFLINE;
    auto it = states.find(device.macAdress);
    if (it != states.end())
      status = it->second.status;

    drawDot(cx, centerY, status, device.color565);
    cx += DOT_SPACING;
  }
}
