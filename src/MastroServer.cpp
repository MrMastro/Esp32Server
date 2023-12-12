#include "MastroServer.h"
#include "htmlPages.h"

AsyncWebServer webServer(80);
DNSServer dnsServer;

MastroServer::MastroServer()
{
    serverActive = false;
    isActiveIndicatorLed = false;
    ledIndicatorMode = false;
}

// mode:
// Ap= access point
// WIFI = wirless connect to your wifi
MastroServer::MastroServer(String mode, String ssid, String passwordWiFi, String ssidAP, String passwordAP, String deviceName, String devicePassword, int ledPin)
{
    MastroServer();
    if (ledPin != -1)
    {
        ledIndicatorMode = true;
        ledPinIndicator = ledPin;
    }
    else
    {
        Serial.println("Led indicator disabled");
    }

    // Wait Indicator
    wait5SecondsLedBlink();

    if (mode == "AP")
    {
        initAP(ssidAP, passwordAP);
    }
    else
    {
        Serial.println("init WIFI mode");
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid, passwordWiFi);
    }

    // Wait for connection
    for (int i = 0; (WiFi.status() != WL_CONNECTED && mode != "AP"); i++)
    {
        delay(1000);
        Serial.print(".");
        activeIndicatorLed(true, true);
        if (i == 10)
        {
            Serial.println("10 Seconds have elapsed, time out. Connecting in Access point mode");
            mode = "AP";
        }
    }

    if (mode != "AP")
    {
        Serial.println("");
        Serial.print("Connected to ");
        Serial.println(ssid);
        Serial.print("IP address: ");
        ip = WiFi.localIP().toString();
        Serial.println(ip);
        activeIndicatorLed(false, false);
    }
    else
    {
        initAP(ssidAP, passwordAP);
    }
    delay(200);

    initArduinoOta(deviceName, devicePassword);
    Serial.println("OTA server started");
    setRoutes();
    ElegantOTA.begin(&webServer); // Start ElegantOTA
    WebSerial.begin(&webServer);
    WebSerial.msgCallback(recvMsg);
    webServer.begin();
    Serial.println("HTTP server started");
    serverActive = true;
}

void MastroServer::initAP(String ssid, String password)
{
    Serial.println("init AP mode");
    // AsyncWiFiManager wifiManager(&webServer, &dnsServer);
    // wifiManager.startConfigPortal(ssid.c_str(), password.c_str());
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    dnsServer.start(53, "*", WiFi.softAPIP());
    // Print the AP IP address to the serial monitor
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    // Configure the captive portal behavior
    // WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
    activeIndicatorLed(true, false);
}

void MastroServer::handleOta()
{
    ArduinoOTA.handle();
}

String MastroServer::getName()
{
    return deviceName;
}

String MastroServer::getIp()
{
    return ip;
}

String MastroServer::splitIpHost(String ip)
{
    int index = ip.lastIndexOf("\\");

    // Get the length of the "My_S" string.
    int length = ip.length();

    // Use `substring()` function to extract a substring from "My_S" starting from the last newline character up to the end of the string.
    return ip.substring(index, length);
}

boolean MastroServer::activeIndicatorLed(bool active, bool toggle)
{
    if (ledIndicatorMode == false)
    {
        return isActiveIndicatorLed;
    }

    delay(50);
    if (toggle)
    {
        if (isActiveIndicatorLed)
        {
            digitalWrite(ledPinIndicator, LOW);
        }
        else
        {
            digitalWrite(ledPinIndicator, HIGH);
        }
        isActiveIndicatorLed = !isActiveIndicatorLed;
    }
    else if (active)
    {
        digitalWrite(ledPinIndicator, HIGH); // Accendi il LED
        isActiveIndicatorLed = true;
    }
    else
    {
        digitalWrite(ledPinIndicator, LOW); // Spegni il LED
        isActiveIndicatorLed = false;
    }
    delay(100);
    return isActiveIndicatorLed;
}

void MastroServer::wait5SecondsLedBlink()
{
    if (ledIndicatorMode)
    {
        for (int i = 0; i < 5; i++)
        {
            delay(1000);
            Serial.print(".");
            activeIndicatorLed(true, true);
        }
    }
}

void MastroServer::initArduinoOta(String deviceName, String devicePassword)
{
    /*
while (WiFi.waitForConnectResult() != WL_CONNECTED) {
Serial.println("Connection Failed! Rebooting...");
delay(5000);
ESP.restart();
}*/

    // Port defaults to 3232
    ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname(deviceName.c_str());
    // ArduinoOTA.setHostname("myesp32");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");
    ArduinoOTA.setPassword(devicePassword.c_str());

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA
        .onStart([]()
                 {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type); })
        .onEnd([]()
               { Serial.println("\nEnd"); })
        .onProgress([](unsigned int progress, unsigned int total)
                    { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); })
        .onError([](ota_error_t error)
                 {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed"); });

    ArduinoOTA.begin();
}

void MastroServer::setRoutes()
{
    String scopeHost = getName();
    webServer.on("/text", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->send(200, "text", "Hi! I am ESP32 :)"); });

    webServer.on("/", HTTP_GET, [scopeHost](AsyncWebServerRequest *request)
                 {
    String html = htmlPage; // Copy the HTML template from htmlCustom.h
    html.replace("%HOST_NAME%", scopeHost);
    request->send(200, "text/html", html); });
}

void MastroServer::setCustomApi(const char* uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest)
{
    delay(50);
    webServer.on(uri, method, onRequest);
}
