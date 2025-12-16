#include <services/Service.h>
#include "Service.h"

#ifndef SERVICES_COLLECTOR_H
#define SERVICES_COLLECTOR_H

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
        logWarning("first call takeExclusiveExecution, the busy flag should have been false instead is true", "ServicesCollector", "takeExclusiveExecution");
    }
    busy = true;
}

void ServicesCollector::freeExclusiveExecution()
{
    if (!busy)
    {
        logWarning("first call freeExclusiveExecution, the busy flag should have been true instead is false", "ServicesCollector",  "freeExclusiveExecution");
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

boolean ServicesCollector::isSleepyJoystick()
{
    return sleepJoystick;
}

void ServicesCollector::sleepyJoystick()
{
    if (debug)
    {
        logInfoln("Putting joystick to sleep", "ServicesCollector");
    }
    sleepJoystick = true;
}

void ServicesCollector::wakeUpJoystick()
{
    if (debug)
    {
        logInfoln("Aweking joystick from sleep", "ServicesCollector");
    }
    sleepJoystick = false;
}

void ServicesCollector::addService(Service *service, String name)
{
    String s = "Adding service: {name}";
    s.replace("{name}", name);
    logInfoln(s, "ServicesCollector");

    if (name == "")
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "insert the non empty name", "addService");
        return;
    }

    service->setNameService(name);
    service->attachCollector(this);
    service->onInitServiceCollector();
    containerService[name] = service;
}

void ServicesCollector::addService(Service *service, String name, SettingsModel *s)
{
    String log = "Adding service: {name}";
    log.replace("{name}", name);
    logInfoln(log, "ServicesCollector");

    if (name == "")
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "insert the non empty name", "addService");
        return;
    }
    service->setNameService(name);
    service->setSettings(s);
    service->attachCollector(this);
    service->onInitServiceCollector();
    containerService[name] = service;
}

void ServicesCollector::attachServer(MastroServer *serverParam)
{
    server = serverParam;
}

void ServicesCollector::throwServicesCollectorError(ERROR_CODE err, const String detailMessage, const String context)
{
    logError(getError(err,detailMessage), "ServicesCollector", context);
}


/**
 * @brief Log an error message with context.
 *
 * @param msg The error message to be logged.
 * @param context The context in which the error occurred.
 */
void ServicesCollector::logError(String msg, String subject, String context)
{
    String error = "[ ERROR - SERVICE {nameService} on {context} ] {msg}";
    error.replace("{nameService}", subject);
    error.replace("{context}", context);
    error.replace("{msg}", msg);
    differentSerialprintln(error, "\033[31m", &Serial);
}

void ServicesCollector::logWarning(String msg, String subject, String context)
{
    String warn = "[ WARNING - SERVICE {nameService} on {context} ] {msg}";
    warn.replace("{nameService}", subject);
    warn.replace("{context}", context);
    warn.replace("{msg}", msg);
    differentSerialprintln(warn, "\033[33m", &Serial);
}

void ServicesCollector::logInfoln(String msg, String subject)
{
    if (debug)
    {
        String log = "[ LOG - {subject} ] {msg}";
        log.replace("{subject}", subject);
        log.replace("{msg}", msg);
        differentSerialprintln(log, "\033[32m", &Serial);
    }
}

/**
 * Destructor to clean up dynamically allocated services
 */
ServicesCollector::~ServicesCollector()
{
    delete server;
    containerService.clear();
}

#endif // SERVICES_COLLECTOR_H
