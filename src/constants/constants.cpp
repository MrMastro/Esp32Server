#include "Constants.h"

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

StatusInfo getStatusInfoByHttpCode(HTTP_CODE code)
{
    String statusStr = HTTP_CODE_MAP.at(code);
    std::vector<String> parts = splitString(statusStr,',');
    if (parts.size() == 2) {
        // Check if the conversion was successful
        return StatusInfo(static_cast<int>(code), parts[0], parts[1]);
    }

    // Handle incorrect format or conversion failure
    return StatusInfo{};
}

// HTTP_CODE getHttpCodeByStatusInfo(StatusInfo code)
// {
//     String statusStr = HTTP_CODE_MAP.at(code);
//     std::vector<String> parts = splitString(statusStr,',');
//     if (parts.size() == 2) {
//         // Check if the conversion was successful
//         return StatusInfo(parts[0], parts[1]);
//     }

//     // Handle incorrect format or conversion failure
//     return StatusInfo{};

//     inputString.toUpperCase();
//     for (const auto &entry : LED_EFFECT_LABEL_REFERENCE_STRING)
//     {
//         if (entry.second == inputString)
//         {
//         return entry.first;
//         }
//     }
//     // Return a special value or throw an exception if the string is not found
//     return LED_EFFECT_LABEL::UKNOWN_EFFECT;
// }

