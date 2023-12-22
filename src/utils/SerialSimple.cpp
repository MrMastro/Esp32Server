#include "SerialSimple.h"

void differentSerialprintln(const String& msg, HardwareSerial* serial, WebSerialClass* webSerial)
{
    delay(25);
    if (serial != nullptr)
    {
        serial->println(msg.c_str());
    }

    if (webSerial != nullptr)
    {
        webSerial->println(msg.c_str());
    }
    delay(25);
}