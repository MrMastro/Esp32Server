#ifndef ServicesCollector_H
#define ServicesCollector_H

#include <./services/Service.h>

#include <MastroServer.h>

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
    void attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam);
    void attachServer(MastroServer* serverParam);
    //String executeMethod(String nameService,String nameMethod, String param);
    MastroServer* getServer();
    ~ServicesCollector();
private:
    boolean busy;
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    MastroServer* server;
    std::map<String,Service*> containerService;
    boolean debug;
    void throwServicesCollectorError(ERROR_CODE err, const String detailMessage, const String context);
    void logInfoln(String msg);
    void logWarning(String msg, String context);
    void logError(String msg, String context);
};


extern ServicesCollector servicesCollector;

#endif  // ServicesCollector_H
