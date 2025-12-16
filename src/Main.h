#ifndef Main_H
#define Main_H
#include <Arduino.h>
#include <NeoPixelBus.h>
#include <BluetoothSerial.h>

#include "models/LedPresetModel/LedPresetModelList.h"

#include "MastroServer.h"
#include "constants/CommunicationMode.h"
#include "constants/JoystickDirections.h"
#include "ledEffects/DriverLed.h"



//#include "./services/ServicesCollector/ServicesCollector.h"
#include "services/ServiceImplementations/CommandService.h"
#include "services/ServiceImplementations/LedService.h"
#include "services/ServiceImplementations/SettingService.h"
#include "services/ServiceImplementations/FileManagerService.h"
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
void recvButtonByJoystick(J_DIRECTION direction);
void calibrateJoystick(int &CENTER_X, int &CENTER_Y, int samples = 50);
void readJoystickAndSendCommand( int &xValue, int &yValue, int &swState, int &lastSwState, bool &activeButton, J_DIRECTION &lastDirection, int CENTER_X = 1930, int CENTER_Y = 1960, const int DEAD_ZONE = 400);
void setSleepJoystick(bool &sleep);


#endif  // Main_H

// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> NO
// NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> SI