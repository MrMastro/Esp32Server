#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <map>
#include <models/response/BasicResponse/BasicResponse.h>
#include "./utils/FunctionUtils.h"

//Enum to represent http_code with additional information
enum class HTTP_CODE
{
    OK = 200,
    Created = 201,
    Accepted = 202,
    BadRequest = 400,
    Unauthorized = 401,
    NotFound = 404,
    InternalServerError = 500
    // Add more status codes as needed
};

#define WAITING_CONNECTION_TIME_MS 100

//Led constants
#define TIME_MS_FOR_ERROR_EXECUTION 50

//Path
#define SETTINGS_FILE_LOCATION_PATH "/settings/settings.json"
#define SETTINGS_FOLDER_LOCATION_PATH "/settings"

//FRONT END CUSTOM MESSAGE
#define SUCCESS_COMMAND "Comando eseguito: {} - {}"
#define SUCCESS_DELAYED_COMMAND "Comando delayed eseguito: {} - {}"
#define UKNOWN_COMMAND "Comando non riconosciuto"
#define ERROR_COMMAND "ERRORE: {} - {}"
#define COLOR_OUT_OF_RANGE_ERROR "I valori dei colori devono essere compresi tra 0 e 255"
#define LED_EFFECT_LABEL_UKNOWN "Effetto sconosciuto"

//FUNCTION FOR MAPPING
StatusInfo getStatusInfoByHttpCode(HTTP_CODE code);

#endif  // CONSTANTS_H