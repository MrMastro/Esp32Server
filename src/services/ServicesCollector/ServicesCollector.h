#include <Arduino.h>
#include <./services/Service.h>

#ifndef ServicesCollector_H
#define ServicesCollector_H

class Service;

class ServicesCollector
{
public:
    ServicesCollector();
    std::shared_ptr<Service> getService(String name);
    void addService( std::shared_ptr<Service> service);
    void attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam);
    String executeMethod(String nameService,String nameMethod, String param);
    ~ServicesCollector();
private:
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    std::map<String, std::shared_ptr<Service>> services;
    void throwServicesCollectorError(ERROR_CODE err, const String detailMessage);
};

#endif  // ServicesCollector_H
