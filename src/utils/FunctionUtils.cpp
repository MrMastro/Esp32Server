#include "FunctionUtils.h"

// Function to split a string based on a delimiter
std::vector<String> splitString(const String &str, char delimiter)
{
    std::vector<String> tokens;
    int start = 0;
    int end = str.indexOf(delimiter);
    
    while (end != -1) {
        tokens.push_back(str.substring(start, end));
        start = end + 1;
        end = str.indexOf(delimiter, start);
    }

    tokens.push_back(str.substring(start));
    return tokens;
}

uint32_t getColor(int r, int g, int b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}
