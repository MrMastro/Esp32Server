#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ArduinoJson.h>
#include <WebSerial.h>

#ifndef MastroServer_h
#define MastroServer_h

class MastroServer
{
public:
    MastroServer();
    MastroServer(String mode, String ssid, String passwordWiFi, String ssidAP, String passwordAP, String deviceName, String devicePassword, int ledPin=-1); // Costruttore
    void handleOta();
    String getName();
    String getIp();
    void setCustomApi(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest);
    String getOneElementJsonString(String key, String value);
    boolean isAvaible();
    AsyncWebServer* getWebServer();
private:
    AsyncWebServer* pointWebServer;
    bool ledIndicatorMode;
    int ledPinIndicator;
    bool serverActive;
    bool isActiveIndicatorLed;
    String ip;
    String deviceName;
    void initArduinoOta(String deviceName, String devicePassword);
    void setRoutes();
    String splitIpHost(String ip);
    void initAP(String ssid, String password);
    bool activeIndicatorLed(bool active, bool toggle);
    void welcomeWaitLedBlink();
};

#endif