#include <Arduino.h>
#include <constants/constants.h>
#ifndef CommandService_H
#define CommandService_H


class CommandService
{
public:
    CommandService();
    CommandService(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam);
    void attach(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam);
    String executeCommand(CMD cmd, String cmdString);
    String recvMsgAndExecute(String data); //returns the msg received
private:
    boolean isOperative;
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
};




#endif  // CommandService_H