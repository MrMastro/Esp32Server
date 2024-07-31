#ifndef Main_H
#define Main_H
#include <Arduino.h>
#include <LittleFS.h>

// ################################################################################ //
//                            Manage profile settings                               //
// ################################################################################ //
// Decomment the line below for apply default settings                              //
// #include "./settings/settingsDefault.h" //      <--- Default settings     //
// Comment the line below for apply default settings                                //
#include "./settings/settingsDefault.h" //               <--- Custom settings      //
// ################################################################################ //
//                     End of profile settings management                           //
// ################################################################################ //

#include "MastroServer.h"
#include "MastroLed.h"

#include "./services/ServicesCollector/ServicesCollector.h"
#include "./services/ServiceImplementations/CommandService.h"
#include "./services/ServiceImplementations/LedService.h"

#include "routes/Routes.h"


//#include "services/ServiceImplementations/InfoService.cpp"

MastroLed myRgbStript; //LEDStripDriver(Din: 19, Cin: 18);
CommandService commandService;
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> SI
NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> ws2811Strip(32, 5);
LEDStripDriver rgbStript(19, 18);
LedService ledService(&ws2811Strip, nullptr);
InfoService infoService;

void ledTask(void *pvParameters);

void recvMsgBySerial(String data);
void recvMsgBySerialWeb(uint8_t *data, size_t len);
void logInfoln(String msg);
#endif  // Main_H