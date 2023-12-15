#include <Arduino.h>
#include <map>

enum ERROR_CODE {
  NO_ERROR,
  SENSOR_NOT_FOUND,
  INVALID_INPUT,
  CONNECTION_FAILED,
  SERVICE_ERROR,
  // Add more error codes as needed
};

const std::map<ERROR_CODE, String> ERROR_MAP = {
    {ERROR_CODE::NO_ERROR, "NO_ERROR,no error success"},
    {ERROR_CODE::SENSOR_NOT_FOUND, "SENSOR_NOT_FOUND,sensor not found"},
    {ERROR_CODE::INVALID_INPUT, "INVALID_INPUT,invalid input"},
    {ERROR_CODE::CONNECTION_FAILED, "CONNECTION_FAILED,connection failed"},
    {ERROR_CODE::SERVICE_ERROR, "SERVICE_ERROR,service not ready or error"}
    // Add more status codes and information as needed
};

void throwError(ERROR_CODE err, const char* detailMessage);