#include "ServicesCollector.h"

ServicesCollector::ServicesCollector()
{
    
}

Service* ServicesCollector::getService(String name)
{
    return services.at(name);
    
}

String ServicesCollector::executeMethod(String nameService,String nameMethod, String param){
    Service* service = services.at(nameService);
    if(service == nullptr){
        return "ERROR";
    }
    return service->executeJson(nameMethod, param);
}

void ServicesCollector::addService(Service* service) 
{
    if(service->getClassName() == String()){
        String error = "";
        throwError(ERROR_CODE::SERVICE_ERROR,"The method getClassName of service can't have a empty name, modify geClassName Method");
        return;
    }
    services.insert(std::make_pair(service->getClassName(),service));
}

//Destructor to clean up dynamically allocated services
ServicesCollector::~ServicesCollector()
{
    // Destructor implementation
    // Remember to clean up any dynamically allocated resources
    for (const auto& pair : services) {
        delete pair.second;
    }
    services.clear();
}
