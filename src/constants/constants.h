#include <Arduino.h>
#include <map>
#include <model/response/BasicResponse.h>
#include "./utils/FunctionUtils.h"

// Enum to represent response status with additional information
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

const std::map<HTTP_CODE, String> statusMap = {
    {HTTP_CODE::OK, "OK,Success"},
    {HTTP_CODE::Created, "Created,Resource successfully created"},
    {HTTP_CODE::Accepted, "Accepted,Request accepted, processing ongoing"},
    {HTTP_CODE::BadRequest, "Bad Request,Client sent an invalid request"},
    {HTTP_CODE::Unauthorized, "Unauthorized,Client lacks valid authentication credentials"},
    {HTTP_CODE::NotFound, "Not Found,Requested resource not found"},
    {HTTP_CODE::InternalServerError, "Internal Server Error,Server encountered an unexpected condition"},
    // Add more status codes and information as needed
};

StatusInfo getStatusInfoByHttpCode(HTTP_CODE code);