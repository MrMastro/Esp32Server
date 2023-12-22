#include <Arduino.h>
#include <MastroServer.h>
#include <./services/Service.h>

#ifndef ServicesCollector_H
#define ServicesCollector_H

class Service;

typedef std::map<String,Service*> MapService;
class ServicesCollector
{
public:
    ServicesCollector();
    ServicesCollector(MastroServer* serverParam);
    boolean isPresentInMap(String name);
    Service* getService(String name);
    void addService(Service* service, String name);
    void attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam);
    void attachServer(MastroServer* serverParam);
    String executeMethod(String nameService,String nameMethod, String param);
    ~ServicesCollector();
    void doSome() const {}
private:
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    MastroServer* server;
    std::map<String,Service*> containerService;
    void throwServicesCollectorError(ERROR_CODE err, const String detailMessage, const String context);
    void logInfo(String msg);
    void logWarning(String msg, String context);
    void logError(String msg, String context);
};

#endif  // ServicesCollector_H
