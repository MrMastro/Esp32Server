#include <Arduino.h>
#include <MastroServer.h>
#include <./services/Service.h>

#ifndef ServicesCollector_H
#define ServicesCollector_H

class Service;

class ServicesCollector
{
public:
    ServicesCollector();
    ServicesCollector(MastroServer* serverParam);
    std::shared_ptr<Service> getService(String name);
    void addService( std::shared_ptr<Service> service);
    void attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam);
    void attachServer(MastroServer* serverParam);
    String executeMethod(String nameService,String nameMethod, String param);
    ~ServicesCollector();
private:
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    MastroServer* server;
    std::map<String, std::shared_ptr<Service>> services;
    void throwServicesCollectorError(ERROR_CODE err, const String detailMessage);
    void logInfo(String msg);
    void logError(String msg);
};

#endif  // ServicesCollector_H
