#include "Main.h"
#include "models/settingModel/Settings.h"

TaskHandle_t LedTask;
boolean doTest = false;
int ledPin = 2;
boolean requestInAction = false;
// ################################################################################ //
//                              Setup and Loop Method                               //
// ################################################################################ //

void setup(void)
{
  Serial.begin(9600);
  delay(10);

  Settings mySettings = getSettings();
  //todo use settings with mySettings
  //todo remove DEBUG constants and use debug inside mySettings
  mastroServer = MastroServer(wirlessMode, ssid, password, ssidAP, passwordAP, deviceName, devicePassword, ledPin);
  if (mastroServer.isAvaible())
  {
    WebSerial.begin(mastroServer.getWebServer(), "/webConsole");
  }
  servicesCollector.attachSerial(&Serial, &WebSerial);
  servicesCollector.attachServer(&mastroServer);

  logInfoln("Service init");
  servicesCollector.addService(&commandService, "CommandService");
  servicesCollector.addService(&ledService, "LedService");
  servicesCollector.addService(&infoService, "InfoService");

  //  Attach pin
  servicesCollector.getService("LedService")->attachPin({ledPin});

  // Route handling
  initRoutes(mastroServer);

  // Other
  WebSerial.msgCallback(recvMsgBySerialWeb);
  // myRgbStript.setupLedRgb(); deprecated MastroLed

  delay(50);
  logInfoln("Init procedure completed");

  Serial.println("IP");
  Serial.println(((InfoService *)servicesCollector.getService("LedService"))->getIp());

  // Thread running
  xTaskCreate(ledTask, "LedTaskExecution", 4096, NULL, 1, &LedTask);
  // vTaskStartScheduler(); // Start the FreeRTOS scheduler, for some esp32 not working, commented!
}

void loop(void)
{
  mastroServer.handleOta();

  if (!servicesCollector.isBusyForServiceApi())
  {
    if (Serial.available())
    {
      recvMsgBySerial(Serial.readString());
    }
  }
  else
  {
    yield();
  }
}

void ledTask(void *pvParameters)
{
  logInfoln("LedTask Running");

  // Initial effect (commented for disable initial effect)
  WS2811_EFFECT firstEffect = WS2811EffectStringToEnum(initialEffect);
  ((LedService *)servicesCollector.getService("LedService"))->startEffect(firstEffect, RgbColor(0, 0, 255), 100, true, true);

  while (true)
  {
    if (!servicesCollector.isBusyForServiceApi())
    {
      // myRgbStript.loopLedRgb(); deprecated
      delay(10);
      ((LedService *)servicesCollector.getService("LedService"))->runEffectWs2811LifeCycle();
      //((LedService *)servicesCollector.getService("LedService"))->runEffectRgbLifeCycle(); //for now don't play rgb stript
    }
    else
    {
      yield();
    }
  }
}

void test()
{
  logInfoln("Test");
}

Settings getSettings()
{
  // Initialize SPIFFS
  if (!LittleFS.begin())
  {
    logInfoln("An Error has occurred while mounting SPIFFS (read file in rom)");
    logInfoln("Try go into /update elegant ota set ota mode littleFs / SPIFFS and upload FileSystem image (littlefs.bin)");
  }

  const char *filename = "settings/setting.json";

  if (!LittleFS.exists(filename)){
    logInfoln("settings.json non presente lo creo");
    
    // Crea e scrivi il contenuto predefinito nel file
    File file = LittleFS.open(filename, "w");
    if (!file) {
      logInfoln("Errore nella creazione del file");
      return;
    }

    // Scrivi il contenuto predefinito
    file.print(defaultContentSettings);

    // Chiudi il file
    file.close();

    logInfoln("File creato con il contenuto predefinito.");
  }

  // Verifica se il file esiste
  if (LittleFS.exists(filename))
  {

    // Apri il file in modalità lettura
    File file = LittleFS.open(filename, "r");
    if (!file)
    {
      Serial.println("Errore nell'aprire il file");
      return;
    }

    String fileContent;

    // Leggi il contenuto del file e immagazzinalo in una stringa
    while (file.available()) {
      fileContent += (char)file.read();
    }

    // Stampa il contenuto del file
    logInfoln("Caricate le seguenti impostazioni:\n");
    logInfoln(fileContent);

    // Chiudi il file
    file.close();

    Settings deviceSettings;
    deviceSettings.fromJson(fileContent);

    return deviceSettings;
  }
}

void recvMsgBySerialWeb(uint8_t *data, size_t len)
{
  String dataString = "";
  for (int i = 0; i < len; i++)
  {
    dataString += char(data[i]);
  }
  if (dataString.length() > 0)
  {
    ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(dataString);
  }
}

void recvMsgBySerial(String data)
{
  ((CommandService *)servicesCollector.getService("CommandService"))->recvMsgAndExecute(data);
}

void logInfoln(String msg)
{
  if (DEBUG)
  {
    String log = "[ LOG - MAIN ] {msg}";
    log.replace("{msg}", msg);
    differentSerialprintln(log, "\033[32m", &Serial, &WebSerial);
  }
}