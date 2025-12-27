#include "SerialSimple.h"

void differentSerialprintln(const String &msg, String colorMsg, HardwareSerial *serial//, WebSerialClass *webSerial
)
{
    String msgColored = colorMsg + msg + "\033[0m";
    if (serial != nullptr)
    {
        colorMsg == "" ? serial->println(msg.c_str()) : serial->println(msgColored.c_str());
    }

    // if (webSerial != nullptr)
    // {
    //     if(webSerial->availableForWrite())
    //     {
    //         webSerial->println(msg.c_str());
    //     } 
    // }
}

void differentSerialprint(const String &msg, String colorMsg, HardwareSerial *serial//,WebSerialClass *webSerial)
)
{
    String msgColored = colorMsg + msg + "\033[0m";
    if (serial != nullptr)
    {
        colorMsg == "" ? serial->print(msg.c_str()) : serial->print(msgColored.c_str());
    }

    // if (webSerial != nullptr)
    // {
    //     webSerial->print(msg.c_str());
    // }
}

String formatMsg(const String& msg, const std::vector<String>& args) {
    String result = msg;  // iniziale copy
    size_t argIndex = 0;
    int foundIndex = result.indexOf("{}");

    while (foundIndex != -1 && argIndex < args.size()) {
        result = result.substring(0, foundIndex) + args[argIndex] + result.substring(foundIndex + 2);
        argIndex++;
        foundIndex = result.indexOf("{}", foundIndex + args[argIndex-1].length());
    }
    return result;
}

int countSubString(const String& msg, const String& subString) {
    int occurrences = 0;
    int pos = 0;
    int subLen = subString.length();

    while ((pos = msg.indexOf(subString, pos)) != -1) {
        occurrences++;
        pos += subLen;
    }
    return occurrences;
}

// String formatMsg(String msg, std::vector<String> args){
//     std::string stdMsg = msg.c_str();
//     //String result = String(stdMsg.c_str());
//     String result = msg;
//     for (size_t i = 0; i < args.size(); i++)
//     {
//         int foundIndex = result.indexOf("{}");
//         if(foundIndex != -1)
//         {
//             String s1 = result.substring(0, foundIndex);
//             String s2 = result.substring(foundIndex + 2);
//             result = s1 + args.at(i) + s2;
//         }
//     }
//     return result;
// }

// int countSubString(String msg, String subString)
// {
//    int occurrences = 0;
//    std::string::size_type pos = 0;
//    std::string s = std::string(msg.c_str());
//    std::string target = std::string(subString.c_str());;
//    while ((pos = s.find(target, pos )) != std::string::npos) {
//           ++ occurrences;
//           pos += target.length();
//    }
//    return occurrences;

// }