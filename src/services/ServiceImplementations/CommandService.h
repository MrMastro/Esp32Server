#ifndef CommandService_H
#define CommandService_H

#include <services/Service.h>
#include "./services/ServiceImplementations/LedService.h"
#include "./constants/Commands.h"
#include "./constants/Constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <constants/CommunicationMode.h>
#include <exceptions/exceptions.h>
#include "InfoService.cpp"
#include "SerialService.h"
#include "SettingService.h"


class CommandService : public Service
{
public:
    CommandService();
    boolean isAvaible() override;
    StatusInfo recvMsgAndExecute(String data); // returns the msg received
    StatusInfo checkDelyedCmdAndExecute();
protected:
    void onInitServiceCollector() override;
private:
    SerialService* serialService;
    SettingService* settingService;
    boolean isOperative;
    unsigned long triggerTimeCmd;
    CMD delayedCmdToDo;
    std::vector<String> delayedParams;
    StatusInfo executeCommand(CMD cmd, std::vector<String> params);
    StatusInfo insertDelayedCommand(CMD cmd, std::vector<String> params, unsigned long millsDelayed);
    //Validator for command (if string is empty is valid also is specified the error in the return value)
    String validateCmd(CMD cmd, std::vector<String> params);
    String validateChangeCommunicationMode(std::vector<String> params);
    // Method for command
    StatusInfo changeCommunicationMode(std::vector<String> params);
    void sendRebootRequest();
};




#endif  // CommandService_H