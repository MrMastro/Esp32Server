#include "FunctionUtils.h"
#include "SerialSimple.h"

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
        if (i != s.size()-1)
        {
            res += ",";
        }
    }
    res += "]";
    return res;
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
        if (i != s.size()-1)
        {
            res += ",";
        }
    }
    res += "]";
    return res;
}