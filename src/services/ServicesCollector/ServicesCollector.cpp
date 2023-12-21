#include "ServicesCollector.h"

#ifndef SERVICES_COLLECTOR_H
#define SERVICES_COLLECTOR_H

ServicesCollector::ServicesCollector()
{
}

std::shared_ptr<Service> ServicesCollector::getService(String name)
{
    auto it = services.find(name);
    return (it != services.end()) ? it->second : nullptr;
}

String ServicesCollector::executeMethod(String nameService, String nameMethod, String param)
{
    String result = "ERROR";
    auto it = services.find(nameService);
    if (it != services.end())
    {
        if(!it->second.get()->isAvaible())
        {
            String errorMsg = "the service {service} isn't avaible";
            errorMsg.replace("{service}",it->second.get()->getClassName());
            throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, errorMsg);
            return "ERROR";
        }
        result = it->second.get()->executeJson(nameMethod, param);
        if (result == "Service Method not found")
        {
            throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "can't find nameMethod: ");
            return "ERROR";
        }
    }
    else
    {
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "can't find nameService");
        return "ERROR";
    }
    return result;
}

void ServicesCollector::addService(std::shared_ptr<Service> service)
{
    if (service->getClassName() == String())
    {
        String error = "";
        throwServicesCollectorError(ERROR_CODE::SERVICE_ERROR, "The method getClassName of service can't have a empty name, modify geClassName Method");
        return;
    }
    service.get()->attachCollector(this);
    services.insert(std::make_pair(service->getClassName(), service));
}

void ServicesCollector::attachSerial(HardwareSerial * serialPointerParam, WebSerialClass * webSerialPointerParam)
{
    serialPointer = serialPointerParam;
    webSerialPointer = webSerialPointerParam;
    for (const auto &pair : services)
    {
        pair.second.get()->attachSerial(&Serial,&WebSerial);
    }
}

void ServicesCollector::throwServicesCollectorError(ERROR_CODE err, const String detailMessage) {
  String result = "Error: ";
  String statusStr = ERROR_MAP.at(err);
  std::vector<String> parts = splitString(statusStr, ',');
  for (const auto& el : parts) {
    result+= (" - " + el);
  }
  result += " - " + detailMessage;
  differentSerialprintln(result, serialPointer, webSerialPointer);
}

// Destructor to clean up dynamically allocated services
ServicesCollector::~ServicesCollector()
{
    // Destructor implementation
    // Remember to clean up any dynamically allocated resources
    for (const auto &pair : services)
    {
        delete pair.second.get();
    }
    services.clear();
}
#endif // SERVICES_COLLECTOR_H
