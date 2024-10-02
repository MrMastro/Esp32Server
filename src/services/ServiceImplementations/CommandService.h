#ifndef CommandService_H
#define CommandService_H

#include <services/Service.h>
#include "./services/ServiceImplementations/LedService.h"
#include "./constants/Commands.h"
#include "./constants/Constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>
#include "InfoService.cpp"
#include "SerialService.h"


class CommandService : public Service
{
public:
    CommandService();
    boolean isAvaible() override;
    StatusInfo recvMsgAndExecute(String data); // returns the msg received
protected:
    void onInitServiceCollector() override;
private:
    SerialService* serialService;
    boolean isOperative;
    StatusInfo executeCommand(CMD cmd, std::vector<String> params);
};




#endif  // CommandService_H