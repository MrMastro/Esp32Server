#ifndef Main_H
#define Main_H
#include <Arduino.h>
#include <LittleFS.h>
#include <NeoPixelBus.h>

#include "MastroServer.h"

#include "./services/ServicesCollector/ServicesCollector.h"
#include "./services/ServiceImplementations/CommandService.h"
#include "./services/ServiceImplementations/LedService.h"
#include "./services/ServiceImplementations/SettingService.h"
#include "./services/ServiceImplementations/SerialService.h"

#include "routes/Routes.h"

void ledTask(void *pvParameters);
void test();
void recvMsgBySerial(String data);
void recvMsgBySerialWeb(uint8_t *data, size_t len);
void logInfoln(String msg);
#endif  // Main_H

// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> SI