#include <Arduino.h>
#include <./services/Service.h>
#include <constants/constants.h>
#include <exceptions/exceptions.h>
#ifndef ServicesCollector_H
#define ServicesCollector_H

class ServicesCollector
{
public:
    ServicesCollector();
    Service* getService(String name);
    void addService(Service* service);
    String executeMethod(String nameService,String nameMethod, String param);
    ~ServicesCollector();
private:
    std::map<String, Service*> services;
};

#endif  // ServicesCollector_H
