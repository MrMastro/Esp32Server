#ifndef CommandService_H
#define CommandService_H

#include <services/Service.h>
#include "./services/ServiceImplementations/LedService.h"
#include "./constants/constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>

class CommandService : public Service
{
public:
    CommandService();
    boolean isAvaible() override;
    void attachSerial(HardwareSerial* serialPointerParam) override;
    //String executeJson(String methodName, String param) override; deprecated
    //String executeJson(String methodName, std::vector<String> jsonParams) override; deprecated
    StatusInfo recvMsgAndExecute(String data); //returns the msg received
private:
    boolean isOperative;
    StatusInfo executeCommand(CMD cmd, String cmdString);
};




#endif  // CommandService_H