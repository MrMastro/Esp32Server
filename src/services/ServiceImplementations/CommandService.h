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
#include "InfoService.h"
#include "SerialService.h"
#include "SettingService.h"
#include "./validators/validators.h"
#include <models/LedPresetModel/LedPresetModel.h>


class CommandService : public Service
{
public:
    CommandService();
    boolean isAvaible() override;
    StatusInfo recvMsgAndExecute(String data); // returns the msg received
    StatusInfo insertDelayedCommand(CMD cmd, std::vector<String> params, unsigned long millsDelayed);
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
    // Method for command
    StatusInfo changeCommunicationMode(std::vector<String> params);
    void reboot(std::vector<String> params);
};




#endif  // CommandService_H