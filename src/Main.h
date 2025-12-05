#ifndef Main_H
#define Main_H
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <BluetoothSerial.h>
#include "MastroServer.h"
#include "constants/CommunicationMode.h"
#include "constants/JoystickDirections.h"
#include "ledEffects/DriverLed.h"



//#include "./services/ServicesCollector/ServicesCollector.h"
#include "services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/LedService.h"
#include "services/ServiceImplementations/SettingService.h"
#include "services/ServiceImplementations/SerialService.h"

#include "routes/Routes.h"

void setup_communication(SettingsModel sm);
void initServices(HardwareSerial* serialPointer);

void infoWebServer();

void apiServerTask(void *pvParameters);
void ledTask(void *pvParameters);
void webOtaServerTask(void *pvParameters);
void serialBtTask(void *pvParameters);
void serialCableTask(void *pvParameters);
void commandDelayedTask(void *pvParameters);

void test();
void recvMsgBySerial(String data);
void recvMsgBySerialWeb(uint8_t *data, size_t len);



#endif  // Main_H

// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> SI