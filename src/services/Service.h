#ifndef Services_H
#define Services_H

#include <MastroServer.h>
#include "./models/settingModel/SettingsModel.h"
#include <constants/Constants.h>
#include <exceptions/exceptions.h>
#include <utils/FunctionUtils.h>
#include <utils/SerialSimple.h>

class Service;
class ServicesCollector;

typedef std::map<String,Service*> MapService;

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
    virtual ~Service() {}
protected:
    String nameService = "";
    std::vector<int> pins;
    SettingsModel* settings;
    MastroServer* getServerByCollector();
    virtual void onInitServiceCollector() = 0; //method pure, this is cause this must be declared into child class
    friend class ServicesCollector;
private:
    ServicesCollector* collector;
};

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

#endif  // Services_H