#include <Arduino.h>
#include <constants/constants.h>
#ifndef CommandService_H
#define CommandService_H


class CommandService
{
public:
    CommandService();
    void executeCommand(CMD cmd, String cmdString);
    void recvMsgAndExecute(uint8_t *data, size_t len); //returns the msg received
private:
    boolean isOperative;
};




#endif  // CommandService_H