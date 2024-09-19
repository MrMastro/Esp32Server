#ifndef MastroServer_h
#define MastroServer_h

#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <WebSerial.h>

class MastroServer
{
public:
    MastroServer();
    MastroServer(String mode, String ssid, String passwordWiFi, String ssidAP, String passwordAP, String deviceName, String devicePassword, boolean debug, int ledPin = -1); // Costruttore
    void handleOta();
    String getName();
    String getIp();
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
    String ip;
    String deviceName;
    boolean debug;
    void initArduinoOta(String deviceName, String devicePassword);
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