#include "Constants.h"

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
//     for (const auto &entry : EFFECT_WS2811_PAIR)
//     {
//         if (entry.second == inputString)
//         {
//         return entry.first;
//         }
//     }
//     // Return a special value or throw an exception if the string is not found
//     return WS2811_EFFECT::UKNOWN_EFFECT;
// }

