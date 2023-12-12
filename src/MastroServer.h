#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#ifndef MastroServer_h
#define MastroServer_h

class MastroServer
{
public:
    MastroServer();
    MastroServer(String mode, String ssid, String passwordWiFi, String ssidAP, String passwordAP, String deviceName, String devicePassword); // Costruttore
    void handleOta();
    String getName();
    String getIp();
private:
    bool active;
    String ip;
    String deviceName;
    void initArduinoOta(String deviceName, String devicePassword);
    void setRoutes();
    String splitIpHost(String ip);
    void initAP(String ssid, String password);
};

void recvMsg(uint8_t *data, size_t len);

#endif