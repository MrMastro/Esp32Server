#include "ServicesCollector.h"

#ifndef SERVICES_COLLECTOR_H
#define SERVICES_COLLECTOR_H
#include "services/ServicesCollector/ServicesCollector.h"
ServicesCollector servicesCollector;

ServicesCollector::ServicesCollector()
{
    server = nullptr;
}

ServicesCollector::ServicesCollector(MastroServer *serverParam)
{
    attachServer(serverParam);
}

boolean ServicesCollector::isPresentInMap(String name)
{
    if (containerService.count(name) > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

Service* ServicesCollector::getService(String name)
{
    if (!isPresentInMap(name))
    {
        String msg = "don't found service called {name}";
        msg.replace("{name}", name);
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, msg, "getService");
        return nullptr;
    }
    return containerService.at(name);
}

String ServicesCollector::executeMethod(String nameService, String nameMethod, String param)
{
    String s = "executeMethod(nameService={nameService}, nameMethod={nameMethod}, param={param})";
    s.replace("{nameService}", nameService);
    s.replace("{nameMethod}", nameMethod);
    s.replace("{param}", param);
    logInfo(s);
    String result = "ERROR";
    if (!isPresentInMap(nameService))
    {
        String msg = "don't found service called {name}";
        msg.replace("{name}", nameService);
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, msg, "executeMethod");
        return "ERROR";
    }

    Service* it = getService(nameService);
    if(it == nullptr){
        return "ERROR";
    }
    if (it->getNameService() == "")
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_NOT_IMPLEMENTED, "please create the implementation of this class and ovverride getClassName with name of Service implemented", "executeMethod");
        return "ERROR";
    }

    if (!it->isAvaible())
    {
        String errorMsg = "the service {service} isn't avaible";
        errorMsg.replace("{service}", it->getNameService());
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, errorMsg, "executeMethod");
        return "ERROR";
    }

    result = it->executeJson(nameMethod, param);

    if (result == "Service Method not found")
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "can't find nameMethod: ", "executeMethod");
        return "ERROR";
    }
    return result;
}

MastroServer *ServicesCollector::getServer()
{
    if(server == nullptr){
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "server point is null, attach server first", "getServer");
        return nullptr;
    }

    return server;
}

void ServicesCollector::addService(Service* service, String name)
{
    String s = "Adding service: {name}";
    s.replace("{name}", name);
    logInfo(s);
    if (serialPointer == nullptr || webSerialPointer == nullptr)
    {
        logInfo("Warning: it is recommended first attach Serial or webSerial with the attachSerial method cause the services that add can required Serials");
    }

    if (name == "")
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "insert the non empty name", "addService");
        return;
    }

    service->setNameService(name);
    service->attachCollector(this);
    service->attachSerial(serialPointer, webSerialPointer);
    containerService[name] = service;
}

void ServicesCollector::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
    if (serialPointer != nullptr || webSerialPointer != nullptr)
    {
        logWarning("serial and webSerial is already attached", "attachSerial");
        return;
    }
    serialPointer = serialPointerParam;
    webSerialPointer = webSerialPointerParam;
    if (containerService.size() > 0)
    {
        for (auto &entry : containerService)
        {
            entry.second->attachSerial(&Serial, &WebSerial);
        }
    }
}

void ServicesCollector::attachServer(MastroServer *serverParam)
{
    server = serverParam;
}

void ServicesCollector::throwServicesCollectorError(ERROR_CODE err, const String detailMessage, const String context)
{
    logError(getError(err, detailMessage), context);
}

void ServicesCollector::logInfo(String msg)
{
    String result = "[ LOG - ServiceCollector ] {msg}";
    result.replace("{msg}", msg);
    differentSerialprintln(result, "\033[32m", serialPointer, webSerialPointer);
}

void ServicesCollector::logWarning(String msg, String context)
{
    String result = "[ WARNING - ServiceCollector on {context} ] {msg}";
    result.replace("{context}", context);
    result.replace("{msg}", msg);
    differentSerialprintln(result, "\033[33m", serialPointer, webSerialPointer);
}

void ServicesCollector::logError(String msg, String context)
{
    String error = "[ ERROR - ServiceCollector on {context} ] {msg}";
    error.replace("{context}", context);
    error.replace("{msg}", msg);
    differentSerialprintln(error, "\033[31m",serialPointer, webSerialPointer);
}

// Destructor to clean up dynamically allocated services
ServicesCollector::~ServicesCollector()
{
    delete serialPointer;
    delete webSerialPointer;
    delete server;
    containerService.clear();
}
#endif // SERVICES_COLLECTOR_H
