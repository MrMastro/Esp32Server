#include <Arduino.h>
#include <./services/Service.h>

#ifndef ServicesCollector_H
#define ServicesCollector_H

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
    std::map<String, std::shared_ptr<Service>> services;
    void throwServicesCollectorError(ERROR_CODE err, const char* detailMessage);
};

#endif  // ServicesCollector_H
