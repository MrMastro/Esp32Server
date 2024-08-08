#include "Service.h"
#include "ServicesCollector/ServicesCollector.h"
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
  throwError(ERROR_CODE::SERVICE_NOT_IMPLEMENTED, "please create the implementation of isAvaible() in this service ", "isAvaible");
  return false;
}

/**
 * @brief Attach serial and web serial pointers to the service.
 * 
 * @param serialPointerParam Pointer to the HardwareSerial.
 * @param webSerialPointerParam Pointer to the WebSerialClass.
 */
void Service::attachSerial(HardwareSerial *serialPointerParam, WebSerialClass *webSerialPointerParam)
{
  serialPointer = serialPointerParam;
  webSerialPointer = webSerialPointerParam;
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
  logInfoln("prepare pin don't necessary, override this method and use pins vector");
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
    throwError(ERROR_CODE::SERVICE_ERROR, "attach serviceCollector first", "getServiceByCollector");
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
    throwError(ERROR_CODE::SERVICE_ERROR, "server point is null", "getServer");
    return "ERROR";
  }
  return pointer->getIp();
}

/**
 * @brief Set the settings for the service.
 * 
 * @param s The SettingsModel object containing the settings to be applied.
 */
void Service::setSettings(SettingsModel s)
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
    throwError(ERROR_CODE::SERVICE_ERROR, "server point is null", "getServer");
    return nullptr;
  }
  return pointer;
}

/**
 * @brief Throw an error with a specific code and message.
 * 
 * @param err The error code.
 * @param detailMessage Detailed message of the error.
 * @param context Context in which the error occurred.
 */
void Service::throwError(ERROR_CODE err, const char *detailMessage, String context)
{
  logError(getError(err, detailMessage), context);
}

/**
 * @brief Log an informational message if debug mode is enabled.
 * 
 * @param msg The informational message to be logged.
 */
void Service::logInfoln(String msg)
{
  if (settings.debug)
  {
    String log = "[ LOG - SERVICE {nameService} ] {msg}";
    log.replace("{nameService}", nameService);
    log.replace("{msg}", msg);
    differentSerialprintln(log, "\033[32m", serialPointer, webSerialPointer); // set green color
  }
}

/**
 * @brief Log a warning message with context.
 * 
 * @param msg The warning message to be logged.
 * @param context The context in which the warning occurred.
 */
void Service::logWarning(String msg, String context)
{
  String log = "[ WARNING - SERVICE {nameService} on {context} ] {msg}";
  log.replace("{nameService}", nameService);
  log.replace("{context}", context);
  log.replace("{msg}", msg);
  differentSerialprintln(log, "\033[33m", serialPointer, webSerialPointer);
}

/**
 * @brief Log an error message with context.
 * 
 * @param msg The error message to be logged.
 * @param context The context in which the error occurred.
 */
void Service::logError(String msg, String context)
{
  String error = "[ ERROR - SERVICE {nameService} on {context} ] {msg}";
  error.replace("{nameService}", nameService);
  error.replace("{context}", context);
  error.replace("{msg}", msg);
  differentSerialprintln(error, "\033[31m", serialPointer, webSerialPointer);
}
