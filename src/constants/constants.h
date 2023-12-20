#include <Arduino.h>
#include <map>
#include <models/response/BasicResponse.h>
#include "./utils/FunctionUtils.h"

#ifndef CONSTANTS_H
#define CONSTANTS_H

// Enum to represent http_code with additional information
enum class HTTP_CODE
{
    OK = 200,
    Created = 201,
    Accepted = 202,
    BadRequest = 400,
    Unauthorized = 401,
    NotFound = 404,
    InternalServerError = 500,
    // Add more status codes as needed
};

const std::map<HTTP_CODE, String> HTTP_CODE_MAP = {
    {HTTP_CODE::OK, "OK,Success"},
    {HTTP_CODE::Created, "Created,Resource successfully created"},
    {HTTP_CODE::Accepted, "Accepted,Request accepted, processing ongoing"},
    {HTTP_CODE::BadRequest, "Bad Request,Client sent an invalid request"},
    {HTTP_CODE::Unauthorized, "Unauthorized,Client lacks valid authentication credentials"},
    {HTTP_CODE::NotFound, "Not Found,Requested resource not found"},
    {HTTP_CODE::InternalServerError, "Internal Server Error,Server encountered an unexpected condition"},
    // Add more status codes and information as needed
};

enum class CMD
{
  LED_ON,
  LED_OFF,
  LED_TOGGLE,
  INFO,
  OTHER
};

const std::map<CMD, String> CMD_MAP = {
    {CMD::LED_ON, "led on"},
    {CMD::LED_OFF, "led off"},
    {CMD::LED_TOGGLE, "led toggle"},
    {CMD::INFO, "info"},
    {CMD::OTHER, "other"}
    // Add more status codes and information as needed
};

CMD mapStringToEnum(String inputString);
StatusInfo getStatusInfoByHttpCode(HTTP_CODE code);

#endif  // CONSTANTS_H