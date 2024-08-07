#include "MastroServer.h"
#include "constants/htmlPages.h"
#include "LITTLEFS.h"
#include "utils/SerialSimple.h"
#include "constants/constants.h"

MastroServer mastroServer;
AsyncWebServer webServer(80);
DNSServer dnsServer;

MastroServer::MastroServer()
{
    serverActive = false;
    isActiveIndicatorLed = false;
    ledIndicatorMode = false;
    littleFSAvaible = false;
    debug = false;
}

// mode:
// Ap= access point
// WIFI = wirless connect to your wifi
MastroServer::MastroServer(String mode, String ssid, String passwordWiFi, String ssidAP, String passwordAP, String deviceName, String devicePassword, boolean debugMode, int ledPin)
{
    MastroServer();
    debug = debugMode;
    logInfo("\n");
    if (ledPin != -1)
    {
        ledIndicatorMode = true;
        ledPinIndicator = ledPin;
    }
    else
    {
        logInfoln("Led indicator disabled");
    }

    // Wait Indicator
    // welcomeWaitLedBlink();

    // Initialize SPIFFS
    if (!LittleFS.begin())
    {
        littleFSAvaible = false;
        logInfoln("An Error has occurred while mounting SPIFFS (read file in rom)");
        logInfoln("Try go into /update elegant ota set ota mode littleFs / SPIFFS and upload FileSystem image (littlefs.bin)");
    }
    else
    {
        littleFSAvaible = true;
    }

    logInfo("\n");

    if (mode != "AP")
    {
        boolean successWifi = initWIFI(ssid, passwordWiFi);
        if(!successWifi)
        {
            logInfo("\n");
            logInfoln("10 Seconds have elapsed, time out. Connecting in Access point mode");
            initAP(ssidAP, passwordAP);
        }
    }
    else
    {
        initAP(ssidAP, passwordAP);
    }
    delay(200);

    initArduinoOta(deviceName, devicePassword);
    logInfoln("OTA server started");
    beginListFiles("/www");
    setRoutes();
    ElegantOTA.begin(&webServer, deviceName.c_str(), devicePassword.c_str());
    // ElegantOTA.begin(&webServer); // Start ElegantOTA
    webServer.begin();
    logInfoln("HTTP server started");
    pointWebServer = &webServer;
    serverActive = true;
}

boolean MastroServer::initWIFI(String &ssid, String &passwordWiFi)
{
    logInfoln("init WIFI mode");
    logInfoln("Try to connected to ");
    logInfoln(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, passwordWiFi);

    // Wait for connection
    for (int i = 0; (WiFi.status() != WL_CONNECTED); i++)
    {
        delay(1000);
        logInfo(".");
        activeIndicatorLed(true, true);
        if (i == 10)
        {
            // Fail
            return false;
        }
    }

    // Success
    logInfoln("IP address: ");
    ip = WiFi.localIP().toString();
    logInfoln(ip);
    activeIndicatorLed(false, false);
    return true;
}

void MastroServer::initAP(String ssid, String password)
{
    logInfoln("init AP mode");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    dnsServer.start(53, "*", WiFi.softAPIP());
    // Print the AP IP address to the serial monitor
    logInfoln("AP IP address: ");
    ip = WiFi.softAPIP().toString();
    logInfoln(ip);
    // Configure the captive portal behavior
    // WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
    activeIndicatorLed(true, false);
}

boolean MastroServer::isAvaible()
{
    return serverActive;
}

AsyncWebServer *MastroServer::getWebServer()
{
    return pointWebServer;
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

void MastroServer::welcomeWaitLedBlink()
{
    if (ledIndicatorMode)
    {
        for (int i = 0; i < 10; i++)
        {
            delay(100);
            Serial.print(".");
            activeIndicatorLed(true, true);
        }
    }
}

void MastroServer::beginListFiles(String path)
{
    if (littleFSAvaible)
    {
        logInfoln("Listing absolute file");
        logInfoln("Add api /favicon.ico - getting: /www/assets/img/favicon.png");
        webServer.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
                     {
                         //
                         request->send(LittleFS, "/www/assets/img/favicon.ico", String(), false);
                         //
                     });
        logInfoln("Listing files in: " + path);
        fs::File root = LittleFS.open(path);
        listFiles(root, path);
    }
    else
    {
        logInfoln("Can't preparated www api for get pages (littelFs don't ready)");
    }
}

void MastroServer::listFiles(fs::File root, String path)
{

    for (fs::File file = root.openNextFile(); file != File(); file = root.openNextFile())
    {

        if (!file.isDirectory())
        {
            logInfoln("is a file: ");
            logInfoln(file.name());
            logInfoln(" - Add api ");
            setRouteSystem(path, String(file.name()));
        }
        else
        {
            // Serial.print("is a directory: ");
            // Serial.print(file.name());
            // Serial.println(" - Analizing directory");
            listFiles(file, path + "/" + String(file.name()));
        }
    }
}

void MastroServer::setRouteSystem(String path, String resource)
{
    boolean apiRedirected = false;
    String resourcePath = path + "/" + resource;
    String apiPath = path + "/" + resource;
    if (resource.endsWith(".html"))
    {
        String pathNotModified = resourcePath;
        int index = resource.lastIndexOf(".html");
        int length = resource.length();
        resource = resource.substring(0, index);
        apiPath = path + "/" + resource;
        webServer.on(pathNotModified.c_str(), HTTP_GET, [apiPath](AsyncWebServerRequest *request)
                     { request->redirect(apiPath); });
    }

    // if for redirect on web commented for now
    // if (resource.endsWith("bootstrap.min.css"))
    // {
    //     apiRedirected = true;
    //     String bootStraptWeb = "https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css";
    //     Serial.println(String("" + apiPath + " - getting: redirected on web - " + bootStraptWeb));
    //     webServer.on(apiPath.c_str(), HTTP_GET, [bootStraptWeb](AsyncWebServerRequest *request)
    //             {
    //                 //
    //                 request->redirect(bootStraptWeb);
    //                 //
    //             });
    // }

    if (!apiRedirected)
    {
        logInfoln(String("" + apiPath + " - getting: " + resourcePath));
        webServer.on(apiPath.c_str(), HTTP_GET, [resourcePath](AsyncWebServerRequest *request)
                     {
                         //
                         request->send(LittleFS, resourcePath, String(), false);
                         //
                     });
    }
}

void MastroServer::initArduinoOta(String deviceName, String devicePassword)
{

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

    // old root
    webServer.on("/indexOld", HTTP_GET, [scopeHost](AsyncWebServerRequest *request)
                 {
     String html = htmlPage; // Copy the HTML template from htmlCustom.h
     html.replace("%HOST_NAME%", scopeHost);
     request->send(200, "text/html", html); });

    // New root
    webServer.on("/", HTTP_GET, [scopeHost](AsyncWebServerRequest *request)
                 { request->redirect("/www/control"); });

    // Api to get control web page
    webServer.on("/control", HTTP_GET, [](AsyncWebServerRequest *request)
                 { request->redirect("/www/control"); });
}

void MastroServer::setCustomApi(const char *uri, WebRequestMethodComposite method, ArRequestHandlerFunction onRequest)
{
    delay(50);
    webServer.on(uri, method, onRequest);
}

String processor(const String &var)
{
    Serial.println(var);
    if (var == "HOST_NAME")
    {
        return ""; // mastroServer.getName();
    }
    return String();
}

void MastroServer::logInfoln(String msg)
{
    if (debug)
    {
        String log = "[ LOG - MastroServer ] {msg}";
        log.replace("{msg}", msg);
        differentSerialprintln(log, "\033[32m", &Serial, nullptr);
    }
}

void MastroServer::logInfo(String msg)
{
    if (debug)
    {
        String log = "{msg}";
        log.replace("{msg}", msg);
        differentSerialprint(log, "\033[32m", &Serial, nullptr);
    }
}
