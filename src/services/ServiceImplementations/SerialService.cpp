#include "SerialService.h"
#include <GeneralUtils.h>

/**
 * Create an empty SerialService with isOperative false
 */
SerialService::SerialService()
{
    lastLineMsg = std::vector<String>();
    lastMsgIsFixed = false;
    isOperative = false;
    serialPointer = nullptr;
    btSerialPointer = nullptr;
}

/**
 * Create a SerialService with Serial and bluetooth Serial
 */
SerialService::SerialService(unsigned long baud, String deviceName, BluetoothSerial *btlPointer, bool isMaster)
{
    SerialService();
    initSerialBegin(baud);
    initSerialBtBegin(deviceName, btlPointer, isMaster);
    isOperative = true;
}

/**
 * Create a SerialService with all 3 Serial avaible (Serial, bluetooth Serial and web Serial)
 */
SerialService::SerialService(unsigned long baud, BluetoothSerial *btlPointer, String deviceName, AsyncWebServer *server, const char *url)
{
    SerialService();
    initAllSerials(baud, btlPointer, deviceName, server, url);
}

boolean SerialService::isAvaible()
{
    return isOperative;
}

const std::vector<String> SerialService::getLastSentMsg()
{
    return lastLineMsg;
}

void SerialService::attachSerial(HardwareSerial *serial)
{
    if (serialPointer != nullptr)
    {
        logError("Serial already previously initializated, cannot reinizilizer", "SerialService", "attachSerial");
        return;
    }

    if (serialPointer == nullptr)
    {
        serialPointer = serial;
        isOperative = true;
    }
}

// WARNING Deprercated
void SerialService::initSerialBegin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t txPin, bool invert, unsigned long timeout_ms, uint8_t rxfifo_full_thrhd)
{
    if (serialPointer != nullptr)
    {
        logError("Serial already previously initializated, cannot reinizilizer", "SerialService", "initSerialBegin");
        return;
    }

    if (serialPointer == nullptr)
    {
        Serial.begin(baud, config, rxPin, txPin, invert, timeout_ms, rxfifo_full_thrhd);
        logInfoln("Init Serial with baud: " + baud, "SerialService");
        serialPointer = &Serial;
        isOperative = true;
    }
}

void SerialService::initSerialBtBegin(String localName, BluetoothSerial *btlPointer, bool isMaster)
{
    if (btSerialPointer != nullptr)
    {
        logError("Bluotooth already previously initializated, cannot reinizilizer", "SerialService", "initSerialBtBegin");
        return;
    }

    logInfoln("Init Bluetooth with name: " + localName, "SerialService");
    // Avvia Bluetooth seriale
    if (!btlPointer->begin(localName, isMaster))
    { // Imposta il nome del dispositivo Bluetooth
        logError("An error occurred initializing Bluetooth", "SerialService", "initSerialBtBegin");
    }
    else
    {
        logInfoln("Bluetooth initializated", "SerialService");
        btSerialPointer = btlPointer;
        isOperative = true;
    }
}

// todo
void SerialService::initSerialWebBegin(AsyncWebServer *server, const char *url)
{
    // if (webSerialPointer == nullptr)
    // {
    //     WebSerial.begin(server, url);
    //     webSerialPointer = &WebSerial;
    // }
    // isOperative = true;
}

void SerialService::initAllSerials(unsigned long baud, BluetoothSerial *bluetoothPointer, String deviceName, AsyncWebServer *server, const char *url)
{
    initSerialBegin(baud);
    initSerialBtBegin(deviceName, bluetoothPointer);
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
        lastMsgIsFixed = false;
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

void SerialService::print(String msg)
{
    if (serialPointer != nullptr)
    {
        serialPointer->print(msg);
    }

    if (btSerialPointer != nullptr)
    {
        btSerialPointer->print(msg);
    }
}

void SerialService::println(String msg)
{
    if (serialPointer != nullptr)
    {
        serialPointer->println(msg);
    }

    if (btSerialPointer != nullptr)
    {
        btSerialPointer->println(msg);
    }
}

void SerialService::printColored(const String &msg, String colorMsg)
{
    String msgColored = colorMsg + msg + "\033[0m";
    if (serialPointer != nullptr)
    {
        colorMsg == "" ? serialPointer->print(msg.c_str()) : serialPointer->print(msgColored.c_str());
    }

    if (btSerialPointer != nullptr)
    {
        colorMsg == "" ? btSerialPointer->print(msg.c_str()) : btSerialPointer->print(msgColored.c_str());
    }
}

void SerialService::printlnColored(const String &msg, String colorMsg)
{
    String msgColored = colorMsg + msg + "\033[0m";
    if (serialPointer != nullptr)
    {
        colorMsg == "" ? serialPointer->println(msg.c_str()) : serialPointer->println(msgColored.c_str());
    }

    if (btSerialPointer != nullptr)
    {
        colorMsg == "" ? btSerialPointer->println(msg.c_str()) : btSerialPointer->println(msgColored.c_str());
    }
}

void SerialService::logInfoFixed(String msg, String subject)
{
    if (settings != nullptr)
    {   //todo confront newMessageVector with lastMessageVector, print only if there is a change. Fix thread problem
        if (settings->debug) //settings->debug
        {
            String log = "[ INFO - {subject} ] {msg}";
            log.replace("{subject}", subject);
            log.replace("{msg}", msg);

            // Conta il numero di \n nel nuovo messaggio
            int newLinesInLog = 0;
            for (int i = 0; i < log.length(); i++)
            {
                if (log[i] == '\n')
                {
                    newLinesInLog++;
                }
            }

            int newLinesInLastMsg = lastLineMsg.size();

            if (lastLineMsg.size() > 0 && lastMsgIsFixed)
            {
                std::vector<String> lineLog = splitString(log,'\n');
                normalizeStringVectors(lineLog,lastLineMsg);
                int delta = lineLog.size() - lastLineMsg.size();
                if(delta>0){
                    for (size_t i = 0; i < delta; i++)
                    {
                        serialPointer->println();
                    }
                }
                serialPointer->print("\033[s"); //save position of cursor
                for (int i = lineLog.size()-1; i >= 0; i--)
                {
                    serialPointer->print("\033[F"); 
                    if(lineLog[i] != lastLineMsg[i]){
                        serialPointer->print("\033[2K"); // clear actual line
                        serialPointer->print("\033[32m" + lineLog[i] + "\033[0m"); //sovrascrivi
                    }
                }
                serialPointer->print("\033[u"); //load position of cursor
                setLastMessage(log, false); // Imposta il messaggio e gestisci le righe
            }else{
                // Stampa il nuovo messaggio
                if (serialPointer != nullptr)
                {
                    serialPointer->print("\n\n\n\n\n");
                    serialPointer->println("\033[32m" + log + "\033[0m");
                }

                if (btSerialPointer != nullptr)
                {
                    serialPointer->print("\n\n\n\n\n");
                    serialPointer->println("\033[32m" + log + "\033[0m");
                }
                setLastMessage(log, true); // Imposta il messaggio e gestisci le righe                
            }
            lastMsgIsFixed = true;     // Imposta che l'ultimo messaggio era fisso
        }
    }
}

void SerialService::logInfoln(String msg, String subject)
{
    if (settings != nullptr)
    {
        if (settings->debug)
        {
            String log = "[ LOG - {subject} ] {msg}";
            log.replace("{subject}", subject);
            log.replace("{msg}", msg);
            setLastMessage(log, true);
            printlnColored(log, "\033[32m");
            lastMsgIsFixed = false;
        }
    }
}

void SerialService::logWarning(String msg, String subject, String context)
{
    String warn = "[ WARNING - SERVICE {nameService} on {context} ] {msg}";
    warn.replace("{nameService}", subject);
    warn.replace("{context}", context);
    warn.replace("{msg}", msg);
    setLastMessage(msg, true);
    printlnColored(warn, "\033[33m");
    lastMsgIsFixed = false;
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
    setLastMessage(error, true);
    printlnColored(error, "\033[31m");
    lastMsgIsFixed = false;
}

void SerialService::setLastMessage(const String &msg, bool isLn)
{
    lastLineMsg.clear(); // Pulisci il vettore esistente

    int startIndex = 0;
    int endIndex = msg.indexOf('\n');

    while (endIndex != -1)
    {
        lastLineMsg.push_back(msg.substring(startIndex, endIndex));
        startIndex = endIndex + 1;
        endIndex = msg.indexOf('\n', startIndex);
    }

    // Aggiungi l'ultima riga
    if (startIndex < msg.length())
    {
        lastLineMsg.push_back(msg.substring(startIndex));
    }

    // Se isLn è true, aggiungi una riga vuota
    if (isLn)//isLn
    {
        lastLineMsg.push_back(""); // Aggiungi una riga vuota
    }
}

void SerialService::onInitServiceCollector()
{
}