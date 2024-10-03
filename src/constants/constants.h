#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <map>
#include <models/response/BasicResponse.h>
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

const std::map<HTTP_CODE, String> HTTP_CODE_MAP = {
    {HTTP_CODE::OK, "OK,Success"},
    {HTTP_CODE::Created, "Created,Resource successfully created"},
    {HTTP_CODE::Accepted, "Accepted,Request accepted, processing ongoing"},
    {HTTP_CODE::BadRequest, "Bad Request,Client sent an invalid request"},
    {HTTP_CODE::Unauthorized, "Unauthorized,Client lacks valid authentication credentials"},
    {HTTP_CODE::NotFound, "Not Found,Requested resource not found"},
    {HTTP_CODE::InternalServerError, "Internal Server Error,Server encountered an unexpected condition"}
    // Add more status codes and information as needed
};

//Path

#define SETTINGS_FILE_LOCATION_PATH "/settings/settings.json"

//FRONT END CUSTOM MESSAGE
#define SUCCESS_COMMAND "Comando eseguito: {} - {}"
#define UKNOWN_COMMAND "Comando non riconosciuto"
#define ERROR_COMMAND "ERRORE: {} - {}"
#define COLOR_OUT_OF_RANGE_ERROR "I valori dei colori devono essere compresi tra 0 e 255"
#define WS2811_EFFECT_UKNOWN "Effetto sconosciuto"

//FUNCTION FOR MAPPING
StatusInfo getStatusInfoByHttpCode(HTTP_CODE code);

#endif  // CONSTANTS_H