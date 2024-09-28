#ifndef Services_H
#define Services_H

#include <MastroServer.h>
#include "./models/settingModel/SettingsModel.h"
#include <constants/Constants.h>
#include <exceptions/exceptions.h>
#include <utils/FunctionUtils.h>
#include <utils/SerialSimple.h>

class Service;

typedef std::map<String,Service*> MapService;

class ServicesCollector
{
public:
    ServicesCollector();
    ServicesCollector(MastroServer* serverParam, boolean debug);
    boolean isPresentInMap(String name);
    boolean isBusyForServiceApi();
    void takeExclusiveExecution();
    void freeExclusiveExecution();
    Service* getService(String name);
    void addService(Service* service, String name);
    void addService(Service* service, String name, SettingsModel* s);
    void attachServer(MastroServer* serverParam);
    MastroServer* getServer();
    ~ServicesCollector();
private:
    boolean busy;
    MastroServer* server;
    std::map<String,Service*> containerService;
    boolean debug;
    void throwServicesCollectorError(ERROR_CODE err, const String detailMessage, const String context);
    void logError(String msg, String subject, String context);
    void logInfoln(String msg, String subject);
    void logWarning(String msg, String subject, String context);
};

extern ServicesCollector servicesCollector;

// Service interface
class Service {
public:
    virtual boolean isAvaible();
    boolean attachPin(std::vector<int> values);
    virtual boolean preparePin();
    void setNameService(String name);
    String getNameService();
    void attachCollector(ServicesCollector* collectorParam);
    Service* getServiceByCollector(String nameService);
    String getServerIpByCollector();
    void setSettings(SettingsModel* s);
    virtual void onInitServiceCollector();
    virtual ~Service() {}
protected:
    String nameService = "";
    std::vector<int> pins;
    SettingsModel* settings;
    MastroServer* getServerByCollector();
private:
    ServicesCollector* collector;
};

#endif  // Services_H