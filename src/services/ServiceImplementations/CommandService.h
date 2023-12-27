#ifndef CommandService_H
#define CommandService_H

#include <services/Service.h>
#include "LedService.h"

class CommandService : public Service
{
public:
    CommandService();
    boolean isAvaible() override;
    void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;
    String executeJson(String methodName, String param) override;
    String executeJson(String methodName, std::vector<String> jsonParams) override;
    String recvMsgAndExecute(String data); //returns the msg received
private:
    boolean isOperative;
    String executeCommand(CMD cmd, String cmdString);
};




#endif  // CommandService_H