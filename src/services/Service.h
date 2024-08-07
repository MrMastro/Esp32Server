#ifndef Services_H
#define Services_H

#include <MastroServer.h>
#include "./models/DataModelling.h"
#include "./models/settingModel/SettingsModel.h"
#include <constants/constants.h>
#include <exceptions/exceptions.h>
#include <utils/FunctionUtils.h>
#include <utils/SerialSimple.h>

class ServicesCollector;

// Service interface
class Service {
public:
    virtual boolean isAvaible();
    virtual void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam);
    boolean attachPin(std::vector<int> values);
    virtual boolean preparePin();
    //virtual String executeJson(String methodName, String param); deprecated
    //virtual String executeJson(String methodName, std::vector<String> jsonParams); deprecated
    void setNameService(String name);
    String getNameService();
    void attachCollector(ServicesCollector* collectorParam);
    Service* getServiceByCollector(String nameService);
    String getServerIpByCollector();
    virtual ~Service() {}
protected:
    String nameService = "";
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    std::vector<int> pins;
    MastroServer* getServerByCollector();
    SettingsModel settings;
    virtual void throwError(ERROR_CODE err, const char* detailMessage, String context);
    virtual void logInfoln(String msg);
    virtual void logWarning(String msg, String context);
    virtual void logError(String msg, String context);
private:
    ServicesCollector* collector;
};

#endif  // Services_H