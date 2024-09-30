#include "Constants.h"

StatusInfo getStatusInfoByHttpCode(HTTP_CODE code)
{
    String statusStr = HTTP_CODE_MAP.at(code);
    std::vector<String> parts = splitString(statusStr,',');
    if (parts.size() == 2) {
        // Check if the conversion was successful
        return StatusInfo(parts[0], parts[1]);
    }

    // Handle incorrect format or conversion failure
    return StatusInfo{};
}