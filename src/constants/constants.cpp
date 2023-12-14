#include "constants.h"

StatusInfo getStatusInfoByHttpCode(HTTP_CODE code)
{
    String statusStr = statusMap.at(code);
    std::vector<String> parts = splitString(statusStr,',');
    Serial.println("Analisi split: ");
    Serial.print(parts[0]);
    Serial.print(" - ");
    Serial.print(parts[1]);
    if (parts.size() == 2) {
        // Check if the conversion was successful
        return StatusInfo(parts[0], parts[1]);
    }

    // Handle incorrect format or conversion failure
    return StatusInfo{};
}