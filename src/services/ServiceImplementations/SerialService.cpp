#include "SerialService.h"

/**
 * Create an empty SerialService with isOperative false
 */
SerialService::SerialService()
{
    lastSentMsg = "";
    btSerialPointer = new BluetoothSerial();
    isOperative = false;
}

/**
 * Create a SerialService with Serial and bluetooth Serial
 */
SerialService::SerialService(unsigned long baud, String deviceName)
{
    lastSentMsg = "";
    initSerialBegin(baud);
    initSerialBtBegin(deviceName);
    isOperative = true;
}

/**
 * Create a SerialService with all 3 Serial avaible (Serial, bluetooth Serial and web Serial)
 */
SerialService::SerialService(unsigned long baud, String deviceName, AsyncWebServer *server, const char *url)
{
    initAllSerials(baud, deviceName, server, url);
}

boolean SerialService::isAvaible()
{
    return isOperative;
}

String SerialService::getLastSentMsg(){
    return lastSentMsg;
}

void SerialService::initSerialBegin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert, unsigned long timeout_ms, uint8_t rxfifo_full_thrhd)
{
    if (serialPointer == nullptr)
    {
        Serial.begin(baud, config, rxPin, txPin, invert, timeout_ms, rxfifo_full_thrhd);
        serialPointer = &Serial;
    }
    isOperative = true;
}

void SerialService::initSerialBtBegin(String localName, bool isMaster)
{
    if (btSerialPointer != nullptr)
    {
        logInfoln("Init Bluetooth with name: "+localName, "SerialService");
        btSerialPointer = new BluetoothSerial();
        btSerialPointer->begin(localName, isMaster);
        logInfoln("Bluetooth initializated", "SerialService");
        isOperative = true;
    }
   
}

void SerialService::initSerialWebBegin(AsyncWebServer *server, const char *url)
{
    if (webSerialPointer == nullptr)
    {
        WebSerial.begin(server, url);
        webSerialPointer = &WebSerial;
    }
    isOperative = true;
}

void SerialService::initAllSerials(unsigned long baud, String deviceName, AsyncWebServer *server, const char *url)
{
    initSerialBegin(baud);
    initSerialBtBegin(deviceName);
    initSerialWebBegin(server, url);
    isOperative = true;
}

boolean SerialService::availableSerial()
{
    if (serialPointer == nullptr)
    {
        return false;
    }
    return serialPointer->available();
}

boolean SerialService::availableSerialBt()
{
    if (btSerialPointer == nullptr)
    {
        return false;
    }
    return btSerialPointer->available();
}

String SerialService::getMsgbySerial()
{
    if (availableSerial())
    {
        String msg = serialPointer->readString();
        return msg;
    }
    return String();
}

String SerialService::getMsgbyBluetooth()
{
    if (availableSerialBt())
    {
        String msg = btSerialPointer->readString();
        return msg;
    }
    return String();
}

void SerialService::logInfoln(String msg, String subject)
{
    // todo use s.debug
    if (settings != nullptr)
    {
        if (settings->debug)
        {
            String log = "[ LOG - {subject} ] {msg}";
            log.replace("{subject}", subject);
            log.replace("{msg}", msg);
            lastSentMsg = log; //todo integrate in differentSerialPrintln of service (to create)
            differentSerialprintln(log, "\033[32m", serialPointer, webSerialPointer);
        }
    }
}

void SerialService::logWarning(String msg, String subject, String context)
{
    String warn = "[ WARNING - SERVICE {nameService} on {context} ] {msg}";
    warn.replace("{nameService}", subject);
    warn.replace("{context}", context);
    warn.replace("{msg}", msg);
    lastSentMsg = warn; //todo integrate in differentSerialPrintln of service (to create)
    differentSerialprintln(warn, "\033[33m", serialPointer, webSerialPointer);
}

/**
 * @brief Log an error message with context.
 *
 * @param msg The error message to be logged.
 * @param context The context in which the error occurred.
 */
void SerialService::logError(String msg, String subject, String context)
{
    String error = "[ ERROR - SERVICE {nameService} on {context} ] {msg}";
    error.replace("{nameService}", subject);
    error.replace("{context}", context);
    error.replace("{msg}", msg);
    lastSentMsg = error; //todo integrate in differentSerialPrintln of service (to create)
    differentSerialprintln(error, "\033[31m", serialPointer, webSerialPointer);
}
