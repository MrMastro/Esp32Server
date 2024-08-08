#ifndef Main_H
#define Main_H
#include <Arduino.h>
#include <LittleFS.h>

#include "MastroServer.h"
#include "MastroLed.h"

#include "./services/ServicesCollector/ServicesCollector.h"
#include "./services/ServiceImplementations/CommandService.h"
#include "./services/ServiceImplementations/LedService.h"
#include "./services/ServiceImplementations/SettingService.h"

#include "routes/Routes.h"

MastroLed myRgbStript; //LEDStripDriver(Din: 19, Cin: 18);
CommandService commandService;
SettingService settingService;
NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> ws2811Strip(32, 5);
LEDStripDriver rgbStript(19, 18);
LedService ledService(&ws2811Strip, nullptr);
InfoService infoService;

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