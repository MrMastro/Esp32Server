#include "Service.h"
#include <cstdarg>

/**
 * @brief Set the name of the service.
 * 
 * @param name The name to be set for the service.
 */
void Service::setNameService(String name)
{
  nameService = name;
}

/**
 * @brief Get the name of the service.
 * 
 * @return String The name of the service.
 */
String Service::getNameService()
{
  return nameService;
}

/**
 * @brief Attach a ServicesCollector to the service.
 * 
 * @param collectorParam The ServicesCollector to be attached.
 */
void Service::attachCollector(ServicesCollector *collectorParam)
{
  collector = collectorParam;
}

/**
 * @brief Check if the service is available.
 * 
 * @throws Throws an error if the implementation of isAvaible() is not provided.
 * @return boolean Always returns false.
 */
boolean Service::isAvaible()
{
  return false;
}

/**
 * @brief Attach a vector of pin values to the service and prepare the pins.
 * 
 * @param values Vector of pin values to be attached.
 * @return boolean Always returns true.
 */
boolean Service::attachPin(std::vector<int> values)
{
  pins = values;
  preparePin();
  return true;
}

/**
 * @brief Prepare the pins for the service.
 * 
 * @note Override this method if specific pin preparation is needed.
 * @return boolean Always returns false.
 */
boolean Service::preparePin()
{
  return false;
}

/**
 * @brief Get a service by its name using the collector.
 * 
 * @param nameService The name of the service to be retrieved.
 * @return Service* Pointer to the requested service or nullptr if not found.
 * @throws Throws an error if the ServicesCollector is not attached.
 */
Service *Service::getServiceByCollector(String nameService)
{
  if (collector == nullptr)
  {
    return nullptr;
  }
  return collector->getService(nameService);
}

/**
 * @brief Get the server IP using the collector.
 * 
 * @return String The IP address of the server or "ERROR" if the server pointer is null.
 * @throws Throws an error if the server pointer is null.
 */
String Service::getServerIpByCollector()
{
  MastroServer *pointer = collector->getServer();
  if (pointer == nullptr)
  {
    return "ERROR";
  }
  return pointer->getIp();
}

/**
 * @brief Set the reference settings for the service.
 * 
 * @param s The SettingsModel object containing the settings to be applied.
 */
void Service::setSettings(SettingsModel* s)
{
  settings = s;
}

/**
 * @brief Get the server pointer using the collector.
 * 
 * @return MastroServer* Pointer to the MastroServer or nullptr if not found.
 * @throws Throws an error if the server pointer is null.
 */
MastroServer *Service::getServerByCollector()
{
  MastroServer *pointer = collector->getServer();
  if (pointer == nullptr)
  {
    return nullptr;
  }
  return pointer;
}

void Service::onInitServiceCollector()
{
}
