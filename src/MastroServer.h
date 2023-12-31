#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include <ElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
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
private:
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
    void wait5SecondsLedBlink();
};

void recvMsg(uint8_t *data, size_t len);

#endif