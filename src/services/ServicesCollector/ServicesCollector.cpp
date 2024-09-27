#include "ServicesCollector.h"

#ifndef SERVICES_COLLECTOR_H
#define SERVICES_COLLECTOR_H
#include "services/ServicesCollector/ServicesCollector.h"
ServicesCollector servicesCollector;

ServicesCollector::ServicesCollector()
{
    server = nullptr;
    busy = false;
    debug = false;
}

ServicesCollector::ServicesCollector(MastroServer *serverParam, boolean debugMode)
{
    ServicesCollector();
    attachServer(serverParam);
    debug = debugMode;
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

boolean ServicesCollector::isBusyForServiceApi()
{
    return busy;
}

void ServicesCollector::takeExclusiveExecution()
{
    if (busy)
    {
        logWarning("first call takeExclusiveExecution, the busy flag should have been false instead is true", "takeExclusiveExecution");
    }
    busy = true;
}

void ServicesCollector::freeExclusiveExecution()
{
    if (!busy)
    {
        logWarning("first call freeExclusiveExecution, the busy flag should have been true instead is false", "freeExclusiveExecution");
    }
    busy = false;
}

Service *ServicesCollector::getService(String name)
{
    if (!isPresentInMap(name))
    {
        String msg = "don't found service called {name}, you should add service usign addService(...) before calling getService method";
        msg.replace("{name}", name);
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, msg, "getService");
        return nullptr;
    }
    return containerService.at(name);
}

MastroServer *ServicesCollector::getServer()
{
    if (server == nullptr)
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "server point is null, attach server first", "getServer");
        return nullptr;
    }

    return server;
}

void ServicesCollector::addService(Service *service, String name)
{
    String s = "Adding service: {name}";
    s.replace("{name}", name);
    logInfoln(s);
    if (serialPointer == nullptr)
    {
        logInfoln("Warning: it is recommended first attach Serial or webSerial with the attachSerial method cause the services that add can required Serials");
    }

    if (name == "")
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "insert the non empty name", "addService");
        return;
    }

    service->setNameService(name);
    service->attachCollector(this);
    service->attachSerial(serialPointer);
    containerService[name] = service;
}

void ServicesCollector::addService(Service *service, String name, SettingsModel* s)
{
    String log = "Adding service: {name}";
    log.replace("{name}", name);
    logInfoln(log);
    if (serialPointer == nullptr)
    {
        logInfoln("Warning: it is recommended first attach Serial or webSerial with the attachSerial method cause the services that add can required Serials");
    }

    if (name == "")
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "insert the non empty name", "addService");
        return;
    }
    service->setNameService(name);
    service->setSettings(s);
    service->attachCollector(this);
    service->attachSerial(serialPointer);
    containerService[name] = service;
}


void ServicesCollector::attachSerial(HardwareSerial *serialPointerParam)
{
    if (serialPointer != nullptr)
    {
        logWarning("serial and webSerial is already attached", "attachSerial");
        return;
    }
    serialPointer = serialPointerParam;

    if (containerService.size() > 0)
    {
        for (auto &entry : containerService)
        {
            entry.second->attachSerial(&Serial);
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

void ServicesCollector::logInfoln(String msg)
{
    if (debug)
    {
        String result = "[ LOG - ServiceCollector ] {msg}";
        result.replace("{msg}", msg);
        differentSerialprintln(result, "\033[32m", serialPointer);
    }
}

void ServicesCollector::logWarning(String msg, String context)
{
    String result = "[ WARNING - ServiceCollector on {context} ] {msg}";
    result.replace("{context}", context);
    result.replace("{msg}", msg);
    differentSerialprintln(result, "\033[33m", serialPointer);
}

void ServicesCollector::logError(String msg, String context)
{
    String error = "[ ERROR - ServiceCollector on {context} ] {msg}";
    error.replace("{context}", context);
    error.replace("{msg}", msg);
    differentSerialprintln(error, "\033[31m", serialPointer);
}


/**
 * Destructor to clean up dynamically allocated services
 */
ServicesCollector::~ServicesCollector()
{
    delete serialPointer;
    delete server;
    containerService.clear();
}
#endif // SERVICES_COLLECTOR_H
