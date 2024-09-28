#ifndef MastroServer_h
#define MastroServer_h
#define DEPRECATED __attribute__((deprecated))

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "utils/SerialSimple.h"
#include "constants/Constants.h"



class MastroServer
{
public:
    MastroServer();
    MastroServer(AsyncWebServer* webServer, String mode, String ssid, String passwordWiFi, String ssidAP, String passwordAP, String deviceName, String devicePassword, boolean debug, int ledPin = -1); // Costruttore
    void handleOta();
    String getName();
    String getIp();
    String getWifiCommunicationMode();
    void setCustomApi(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest);
    boolean isAvaible();
    AsyncWebServer* getWebServer();
private:
    AsyncWebServer* pointWebServer;
    bool ledIndicatorMode;
    int ledPinIndicator;
    bool serverActive;
    bool isActiveIndicatorLed;
    bool littleFSAvaible;
    String wifiCommunicationMode;
    String ip;
    String deviceName;
    boolean debug;
    DEPRECATED void initOta(String deviceName, String devicePassword); 
    void setRoutes();
    String splitIpHost(String ip);
    boolean initWIFI(String &ssid, String &passwordWiFi);
    void initAP(String ssid, String password);
    bool activeIndicatorLed(bool active, bool toggle);
    void welcomeWaitLedBlink();
    void beginListFiles(String path);
    void listFiles(fs::File file, String path);
    void setRouteSystem(String path, String resource);
    void logInfoln(String msg);
    void logInfo(String msg);
};

String processor(const String& var);

extern MastroServer mastroServer;

#endif