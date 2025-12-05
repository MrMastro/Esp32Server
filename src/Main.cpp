#include "Main.h"

boolean AP_MODE_PAUSE_LED = false;
int ledPin = 2;
SettingsModel s;

AsyncWebServer webServer(80);
BluetoothSerial SerialBT;

// Deprecated: MastroLed myRgbStript; // LEDStripDriver(Din: 19, Cin: 18);
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *ws2811Matrix = nullptr;
NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod> *ws2811Strip = nullptr;
LEDStripDriver *rgbStrip = nullptr;
DriverLed *myDriver;

CommandService commandService;
SettingService settingService;
LedService ledService;
InfoService infoService;
SerialService serialService;

// AsyncWebServer server(80);
//  ################################################################################ //
//                               Setup and Loop Method                               //
//  ################################################################################ //

// other baud rate: 115200
// baud rate: 9600

// Definizione delle priorità
#define PRIORITY_SERVER               (configMAX_PRIORITIES - 3)
#define PRIORITY_BLUETOOTH_SERVER     (configMAX_PRIORITIES - 3)
#define PRIORITY_LED_EFFECTS          (configMAX_PRIORITIES - 6)
#define PRIORITY_DELAYED_TASK         (configMAX_PRIORITIES - 7)
#define PRIORITY_SERIAL               (configMAX_PRIORITIES - 10)

#define STACK_SIZE_SERVER             8192
#define STACK_SIZE_BLUETOOTH_SERVER   8192
#define STACK_SIZE_LED_EFFECTS        4096
#define STACK_SIZE_DELAYED_TASK       4096
#define STACK_SIZE_SERIAL             8192

// #define STACK_SIZE_SERVER             10000
// #define STACK_SIZE_BLUETOOTH_SERVER   10000
// #define STACK_SIZE_LED_EFFECTS        10000
// #define STACK_SIZE_DELAYED_TASK       10000
// #define STACK_SIZE_SERIAL             10000



void setup_communication(SettingsModel sm)
{
  serialService.logInfoln("setup_communication method", "MAIN");
  boolean unknonwMode = false;
  boolean defaultMode = false;

  // Task for serial COM
  serialService.logInfoln("Create SerialCableTaskExecution for comunicate with serial com", "MAIN");
  xTaskCreate(serialCableTask, "SerialCableTaskExecution", STACK_SIZE_SERIAL, NULL, PRIORITY_SERIAL, NULL);

  // Task for communication
  COMMUNICATION_MODE mode = communicationModeStringToEnum(sm.communicationMode);
  switch (mode)
  {

  case COMMUNICATION_MODE::AP_MODE:
    serialService.logInfoln("Communication mode is AP, init AP web Server", "MAIN");
    mastroServer = MastroServer(&webServer, "AP", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
    AP_MODE_PAUSE_LED = true;
    initRoutes(mastroServer);
    infoWebServer();
    break;

  case COMMUNICATION_MODE::WIFI_MODE:
    serialService.logInfoln("Communication mode is WIFI, init WIFI web Server", "MAIN");
    mastroServer = MastroServer(&webServer, "WIFI", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
    initRoutes(mastroServer);
    infoWebServer();
    break;

  case COMMUNICATION_MODE::BLUETOOTH_MODE:
    serialService.logInfoln("Communication mode is BLUETOOTH, init Bluetooth and Bluetooth Serial", "MAIN");
    serialService.initSerialBtBegin(sm.deviceName, &SerialBT);
    xTaskCreate(serialBtTask, "SerialBluetoothTaskExecution", STACK_SIZE_BLUETOOTH_SERVER, NULL, PRIORITY_BLUETOOTH_SERVER, NULL);
    break;

  // Case WIP or Defaults
  case COMMUNICATION_MODE::UNKNOWN_MODE:
    serialService.logWarning("Communication mode is UNKNOWN", "MAIN", "setup_communication");
    defaultMode = true;
    unknonwMode = true;
    break;
  case COMMUNICATION_MODE::HYBRID_BLUETOOTH_AP:
    serialService.logWarning("Communication mode is HYBRID_BLUETOOTH_AP, this communication is yet as WIP", "MAIN", "setup_communication");
    delay(1000);
    mastroServer = MastroServer(&webServer, "AP", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
    initRoutes(mastroServer);
    infoWebServer();
    defaultMode = true;
    unknonwMode = false;
    break;
  case COMMUNICATION_MODE::HYBRID_BLUETOOTH_WIFI:
    serialService.logWarning("Communication mode is HYBRID_BLUETOOTH_WIFI, this communication is yet as WIP", "MAIN", "setup_communication");
    delay(1000);
    mastroServer = MastroServer(&webServer, "WIFI", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
    initRoutes(mastroServer);
    infoWebServer();
    defaultMode = true;
    unknonwMode = false;
    break;
  default:
    defaultMode = true;
    unknonwMode = true;
    break;
  }
  if (defaultMode)
  {
    if (unknonwMode)
    {
      serialService.logWarning("the mode passing is unknown. Loading ap mode as default", "MAIN", "setup_communication");
      mastroServer = MastroServer(&webServer, "AP", sm.ssidWIFI, sm.passwordWIFI, sm.ssidAP, sm.passwordAP, sm.deviceName, sm.devicePassword, sm.debug, ledPin);
      initRoutes(mastroServer);
      infoWebServer();
    }
    else
    {
      serialService.logWarning("Hybrid Communcation isn't yet avaible in this version (WIP).", "MAIN", "setup_communication");
    }
  }
}

void infoWebServer()
{

  if (!mastroServer.isAvaible())
  {
    serialService.logError("Server not initializated", "MAIN", "setup_webServer");
    return;
  }

  Serial.println("\n");
  Serial.println("SERVER MODE: " + mastroServer.getWifiCommunicationMode());
  Serial.println("IP: " + infoService.getIp());
}

void initServices(HardwareSerial *serialPointer)
{
  serialService.attachSerial(&Serial);
  serialService.logInfoln("Service init", "MAIN");
  servicesCollector.addService(&serialService, "SerialService");
  servicesCollector.addService(&settingService, "SettingService");
  settingService.loadSettings(SETTINGS_FILE_LOCATION_PATH);
  s = settingService.getSettings();
  serialService.setSettings(&s);

  // init LedService
  if(s.ledSettings.enableStripRgb){
    rgbStrip = new LEDStripDriver(s.ledSettings.pinLedDinRgb, s.ledSettings.pinLedCinRgb);
  }

  if(s.ledSettings.enableStripWs2811){
    ws2811Strip = new NeoPixelBus<NeoBrgFeature, Neo800KbpsMethod>(s.ledSettings.numLedWs2811, s.ledSettings.pinLedWs2811);
  }

  if(s.ledSettings.enableStripWs2811Matrix){
    ws2811Matrix = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(s.ledSettings.numLedWs2811Matrix, s.ledSettings.pinLedWs2811Matrix);
  }

  myDriver = new DriverLed(ws2811Matrix, ws2811Strip, rgbStrip);
  ledService = LedService(myDriver, s.ledSettings.enableStripRgb, s.ledSettings.enableStripWs2811, s.ledSettings.enableStripWs2811Matrix);

  servicesCollector.attachServer(&mastroServer);
  servicesCollector.addService(&commandService, "CommandService", &s);
  servicesCollector.addService(&ledService, "LedService", &s);
  servicesCollector.addService(&infoService, "InfoService", &s);

  //  Attach pin
  serialService.logInfoln("Attach pin", "MAIN");
  servicesCollector.getService("LedService")->attachPin({ledPin});
}

void setup(void)
{
  Serial.begin(115200);
  Serial.println("\n");
  Serial.println("Started");
  delay(10);
  Serial.print("\n");
  initServices(&Serial);
  delay(10);

  delay(1000);

  Serial.println("");
  Serial.println("Load settings:");
  Serial.println(s.toJson());

  // init communication (Server wifi/ap or Bluetooth)
  //xTaskCreate(apiServerTask, "apiServerTaskExecution", STACK_SIZE_SERVER, NULL, PRIORITY_SERVER, NULL);
  setup_communication(s);

  delay(100);

  // Thread running
  if (mastroServer.isAvaible())
  {
    // xTaskCreate(webOtaServerTask, "WebOtaServerTaskExecution", 10000, NULL, 1, NULL); commented, cause instability
  }
  xTaskCreate(ledTask, "LedTaskExecution", STACK_SIZE_LED_EFFECTS, NULL, PRIORITY_LED_EFFECTS, NULL);
  xTaskCreate(commandDelayedTask, "commandDelayedTaskExecution", STACK_SIZE_DELAYED_TASK, NULL, PRIORITY_DELAYED_TASK, NULL);
  //vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!

  serialService.logInfoln("Init procedure completed", "MAIN");
}

// loop is used for print debug
void loop(void)
{
}

void apiServerTask(void *pvParameters) {
  // Il task può restare in esecuzione finché il sistema non è spento o resetta
  while (true) {
      // Puoi mettere qui del codice per monitorare o gestire il server, se necessario.
      // Ma questo task non termina mai a meno che non venga fermato o reset.

      // FreeRTOS richiede che i task chiamino vTaskDelay() periodicamente
      vTaskDelay(1); // Questo fa "pausare" il task per un breve periodo, lasciando spazio ad altri task
  }
}

// Serial bt task check input for bluetooth message
void serialBtTask(void *pvParameters)
{
  serialService.logInfoln("Serial Bluetooth Task execution", "MAIN");
  serialService.logInfoln("Start listining bluetooth serial", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      if (serialService.availableSerialBt())
      {
        String msgBt = serialService.getMsgbyBluetooth();
        recvMsgBySerial(msgBt);
      }
    }
    else
    {
      yield();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// Serial Cable task check input for usb Serial message
void serialCableTask(void *pvParameters)
{
  serialService.logInfoln("Serial Task execution", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      // todo change with if (serialService.availableSerial())
      if (serialService.availableSerial())
      {
        String msg = serialService.getMsgbySerial();
        if (msg.equals("t"))
        {
          test();
        }
        else
        {
          Serial.println("Serial has msg: " + msg);
          recvMsgBySerial(msg);
        }
      }
    }
    else
    {
      yield();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void webOtaServerTask(void *pvParameters)
{
  serialService.logInfoln("Web Task execution (OTA)", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      mastroServer.handleOta();
    }
    else
    {
      yield();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void commandDelayedTask(void *pvParameters)
{
  serialService.logInfoln("Delayed Command Task execution", "MAIN");
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      commandService.checkDelyedCmdAndExecute();
    }
    else
    {
      yield();
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void ledTask(void *pvParameters)
{
  serialService.logInfoln("Led Task execution", "MAIN");
  String msg = "";

  std::vector<RgbColor> rgbColors = getRgbColorsByRequest(s.initialColors);

  // Initial effect (commented for disable initial effect)
  if (!isPresentEffect(s.initialEffect))
  {
    msg = formatMsg(" {} | time: {} | colors: {}  - None initial effect applied ", {s.initialEffect, vectorRgbColorToString(rgbColors)});
    serialService.logInfoln(msg, "MAIN");
  }
  else
  {
    msg = formatMsg("First effect running: {} | time: {} | colors: {} ", {s.initialEffect, String(s.initialDeltaT), vectorRgbColorToString(rgbColors)});
    serialService.logInfoln(msg, "MAIN");
    ((LedService *)servicesCollector.getService("LedService"))->startEffect(s.initialEffect, rgbColors, s.initialDeltaT, s.ledSettings.enableStripRgb, s.ledSettings.enableStripWs2811, s.ledSettings.enableStripWs2811Matrix);
  }

  // Loop principale del task
  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      boolean executedCorrectly = false;

      // Controllo e gestione per RGB
      if (s.ledSettings.enableStripRgb)
      {
        executedCorrectly = ((LedService *)servicesCollector.getService("LedService"))->runRgbLifeCycle();
        if (!executedCorrectly)
        {
          // Gestisci eventuali errori con un delay
          vTaskDelay(TIME_MS_FOR_ERROR_EXECUTION / portTICK_PERIOD_MS);  // Usa vTaskDelay per ridurre il carico
        }
      }

      if (s.ledSettings.enableStripWs2811)
      {
        executedCorrectly = ((LedService *)servicesCollector.getService("LedService"))->runWs2811LifeCycle();
        if (!executedCorrectly)
        {
          vTaskDelay(TIME_MS_FOR_ERROR_EXECUTION / portTICK_PERIOD_MS);  // Usa vTaskDelay per ridurre il carico
        }
      }
      if (s.ledSettings.enableStripWs2811Matrix)
      {
        executedCorrectly = ((LedService *)servicesCollector.getService("LedService"))->runWs2811MatrixLifeCycle();
        if (!executedCorrectly)
        {
          vTaskDelay(TIME_MS_FOR_ERROR_EXECUTION / portTICK_PERIOD_MS);  // Usa vTaskDelay per ridurre il carico
        }
      }
    }
    else
    {
      // Se il servizio è occupato, sospendiamo temporaneamente il task senza bloccare la CPU
      yield();  // Cede il controllo al sistema operativo per altri task
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void test()
{
  serialService.logInfoln("Test", "MAIN");
  pinMode(33, INPUT_PULLUP);
  bool printed = false;
  int xValue = 0;
  int yValue = 0;
  int swState = LOW;
  String msgLog = "";
  while (true) {
    xValue = analogRead(34);
    yValue = analogRead(35);
    swState = digitalRead(33);
  
    String code = "";
  
    const int CENTER_MIN = 1890;
    const int CENTER_MAX = 2700;

    msgLog = String("X: ") + xValue + " | Y: " + yValue;
    Serial.println(msgLog);
  
    // Asse X → UP / DOWN
    if (xValue > CENTER_MAX) {
      code += "UP";
    } else if (xValue < CENTER_MIN) {
      code += "DOWN";
    }
  
    // Asse Y → LEFT / RIGHT
    if (yValue > CENTER_MAX) {
      if(code.length() > 0){
        code += "_";
      }
      code += "RIGHT";
    } else if (yValue < CENTER_MIN) {
      if(code.length() > 0){
        code += "_";
      }
      code += "LEFT";
    }
  
    // Nessun movimento → CENTER
    if (code.length() == 0) {
      code = "CENTER";
    }
  
    // Pulsante premuto → aggiungo 'B'
    if (swState == LOW) {
      code += "_BUTTON";
    }
  
    //Serial.println(code);
  
    J_DIRECTION direction = mapStringToJdirections(code);

    // ------------------------------
    // SWITCH SU TUTTI I POSSIBILI CASI
    // ------------------------------
    
    switch (direction) {

      case J_DIRECTION::CENTER:
          Serial.println("CENTER");
          break;

      case J_DIRECTION::UP:
          Serial.println("UP");
          break;

      case J_DIRECTION::DOWN:
          Serial.println("DOWN");
          break;

      case J_DIRECTION::LEFT:
          Serial.println("LEFT");
          break;

      case J_DIRECTION::RIGHT:
          Serial.println("RIGHT");
          break;

      case J_DIRECTION::UP_LEFT:
          Serial.println("UP_LEFT");
          break;

      case J_DIRECTION::UP_RIGHT:
          Serial.println("UP_RIGHT");
          break;

      case J_DIRECTION::DOWN_LEFT:
          Serial.println("DOWN_LEFT");
          break;

      case J_DIRECTION::DOWN_RIGHT:
          Serial.println("DOWN_RIGHT");
          break;

      // Con pulsante premuto
      case J_DIRECTION::CENTER_BUTTON:
          Serial.println("CENTER_BUTTON");
          break;

      case J_DIRECTION::UP_BUTTON:
          Serial.println("UP_BUTTON");
          break;

      case J_DIRECTION::DOWN_BUTTON:
          Serial.println("DOWN_BUTTON");
          break;

      case J_DIRECTION::LEFT_BUTTON:
          Serial.println("LEFT_BUTTON");
          break;

      case J_DIRECTION::RIGHT_BUTTON:
          Serial.println("RIGHT_BUTTON");
          break;

      case J_DIRECTION::UP_LEFT_BUTTON:
          Serial.println("UP_LEFT_BUTTON");
          break;

      case J_DIRECTION::UP_RIGHT_BUTTON:
          Serial.println("UP_RIGHT_BUTTON");
          break;

      case J_DIRECTION::DOWN_LEFT_BUTTON:
          Serial.println("DOWN_LEFT_BUTTON");
          break;

      case J_DIRECTION::DOWN_RIGHT_BUTTON:
          Serial.println("DOWN_RIGHT_BUTTON");
          break;

      default:
          Serial.println("UNKNOWN");
          break;
    }
  
    delay(500);
  }

  while(true){
    xValue = analogRead(34);  // Legge il valore X
    yValue = analogRead(35);  // Legge il valore Y
    swState = digitalRead(33);
    msgLog = String("X: ") + xValue + " | Y: " + yValue;
    Serial.println(msgLog);

    // Zona morta (±300 intorno a 2048)
    int CENTER_MIN = 1890;
    int CENTER_MAX = 2700;
        
    // Direzione Y
    if (yValue > CENTER_MAX) {
      Serial.println("DES");
    } else if (yValue < CENTER_MIN) {
      Serial.println("SIN");
    }

    // Direzione X
    if (xValue > CENTER_MAX) {
      Serial.println("SU");
    } else if (xValue < CENTER_MIN) {
      Serial.println("GIU");
    }
    if (swState == LOW) {
      Serial.println("Switch");
    }
    delay(500);  // Attesa di 1 secondo
  }
      
}

void recvMsgBySerialWeb(uint8_t *data, size_t len)
{
  servicesCollector.takeExclusiveExecution();
  String dataString = "";
  for (int i = 0; i < len; i++)
  {
    dataString += char(data[i]);
  }
  if (dataString.length() > 0)
  {
    ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(dataString);
  }
  servicesCollector.freeExclusiveExecution();
}

void recvMsgBySerial(String data)
{
  servicesCollector.takeExclusiveExecution();
  ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(data);
  servicesCollector.freeExclusiveExecution();
}

//NOTE
// [env:Esp32Com]
// platform = espressif32 @ 6.8.1
// platform = espressif32
// board = esp32doit-devkit-v1
// framework = arduino
// lib_deps = 
// 	makuna/NeoPixelBus@^2.8.2
// 	makuna/NeoPixelBus
// 	esphome/ESPAsyncWebServer-esphome
// 	robtillaart/ANSI@^0.3.2
// 	bblanchon/ArduinoJson@^7.2.1
// 	robtillaart/ANSI
// 	bblanchon/ArduinoJson