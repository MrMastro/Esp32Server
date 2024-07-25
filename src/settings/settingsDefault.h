#include <Arduino.h>

//---- DEVICE SETTINGS -------
const char* deviceName = "MastroDevice";
const char* devicePassword = "Password";
const boolean debug = false;

//---- Initial Operation -------
const String initialEffect = "PROGRESSIVE_BAR_UNIQUE_COLOR";

//---- WIRELESS SETTINGS -------
const String wirlessMode = "AP"; //AP or WIFI

//-- SETTINGS FOR AP MODE
const char* ssidAP = "Esp32Server";
const char* passwordAP = "8GvsfMEC";

//-- SETTINGS FOR WIFI MODE
const char* ssid = "MySsid";
const char* password = "myPass";

//todo update per compilare il bin.