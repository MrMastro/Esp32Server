#include "FunctionUtils.h"
#include "SerialSimple.h"
#include <models/LedEffectRequest/LedColorRequest.h>
#include <models/LedPresetModel/LedColor.h>

// Function to split a string based on a delimiter
std::vector<String> splitString(const String &str, char delimiter)
{
    std::vector<String> tokens;
    int start = 0;
    int end = str.indexOf(delimiter);

    while (end != -1)
    {
        tokens.push_back(str.substring(start, end));
        start = end + 1;
        end = str.indexOf(delimiter, start);
    }

    tokens.push_back(str.substring(start));
    return tokens;
}

/**
 * @brief Removes and returns the first element of a vector.
 *
 * Removes the first element from the vector `s` and returns it.
 * If the vector is empty, an empty `String` is returned.
 *
 * @param s A reference to a `std::vector<String>`.
 * @return The first element of the vector, or an empty `String` if the vector is empty.
 */
String stringPop(std::vector<String> &s)
{
    if (s.size() < 1)
    {
        return String();
    }
    String result = s.at(0);
    std::vector<String> copy;
    s.erase(s.begin());
    return result;
}

String vectorStringtoString(std::vector<String> s)
{
    String res = "[";
    for (int i = 0; i < s.size(); i++)
    {
        res += (s[i]);
        if (i != s.size() - 1)
        {
            res += ",";
        }
    }
    res += "]";
    return res;
}

const std::vector<RgbColor> getRgbColorsByRequest(const std::vector<LedColorRequest> ledColorRequests)
{
    std::vector<RgbColor> rgbColors;

    for (size_t i = 0; i < ledColorRequests.size(); i++)
    {
        LedColorRequest c = ledColorRequests.at(i);
        rgbColors.push_back(RgbColor(c.r, c.g, c.b));
    }

    return rgbColors;
}

const std::vector<RgbColor> getRgbColorsByLedColor(const std::vector<LedColor> ledColor)
{
    std::vector<RgbColor> rgbColors;

    for (size_t i = 0; i < ledColor.size(); i++)
    {
        LedColor c = ledColor.at(i);
        rgbColors.push_back(RgbColor(c.r, c.g, c.b));
    }

    return rgbColors;
}

void normalizeStringVectors(std::vector<String>& vector1, std::vector<String>& vector2) {
    // Determine the lengths of the vectors
    size_t len_v1 = vector1.size();
    size_t len_v2 = vector2.size();
    
    // If the first vector is shorter, add empty strings
    if (len_v1 < len_v2) {
        vector1.resize(len_v2, "");  // Resize vector1 to match vector2 and fill with empty strings
    }
    // If the second vector is shorter, add empty strings
    else if (len_v2 < len_v1) {
        vector2.resize(len_v1, "");  // Resize vector2 to match vector1 and fill with empty strings
    }
}

uint32_t getColor(int r, int g, int b)
{
    return ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

String rgbColorToString(RgbColor color)
{
    return formatMsg("[{},{},{}]", {String(color.R), String(color.G), String(color.B)});
}

String vectorRgbColorToString(std::vector<RgbColor> s)
{
    String res = "[";
    for (int i = 0; i < s.size(); i++)
    {
        res += (rgbColorToString(s[i]));
        if (i != s.size() - 1)
        {
            res += ",";
        }
    }
    res += "]";
    return res;
}