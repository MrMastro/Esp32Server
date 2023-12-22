#include "SerialSimple.h"

void differentSerialprintln(const String &msg, String colorMsg, HardwareSerial *serial, WebSerialClass *webSerial)
{
    String msgColored = colorMsg + msg + "\033[0m";
    delay(25);
    if (serial != nullptr)
    {
        colorMsg == "" ? serial->println(msg.c_str()) : serial->println(msgColored.c_str());
    }

    if (webSerial != nullptr)
    {
        webSerial->println(msg.c_str());
    }
    delay(25);
}

void differentSerialprint(const String &msg, String colorMsg, HardwareSerial *serial, WebSerialClass *webSerial)
{
    String msgColored = colorMsg + msg + "\033[0m";
    delay(25);
    if (serial != nullptr)
    {
        colorMsg == "" ? webSerial->println(msg.c_str()) : serial->println(msgColored.c_str());
    }

    if (webSerial != nullptr)
    {
        webSerial->print(msg.c_str());
    }
    delay(25);
}