#include "SerialSimple.h"
#include <vector>

void differentSerialprintln(const String &msg, String colorMsg, HardwareSerial *serial, WebSerialClass *webSerial)
{
    String msgColored = colorMsg + msg + "\033[0m";
    if (serial != nullptr)
    {
        colorMsg == "" ? serial->println(msg.c_str()) : serial->println(msgColored.c_str());
    }

    if (webSerial != nullptr)
    {
        if(webSerial->availableForWrite())
        {
            webSerial->println(msg.c_str());
        } 
    }
}

void differentSerialprint(const String &msg, String colorMsg, HardwareSerial *serial, WebSerialClass *webSerial)
{
    String msgColored = colorMsg + msg + "\033[0m";
    if (serial != nullptr)
    {
        colorMsg == "" ? webSerial->print(msg.c_str()) : serial->print(msgColored.c_str());
    }

    if (webSerial != nullptr)
    {
        webSerial->print(msg.c_str());
    }
}

String formatMsg(String msg, std::vector<String> args){
    std::string stdMsg = msg.c_str();
    //String result = String(stdMsg.c_str());
    String result = msg;
    for (size_t i = 0; i < args.size(); i++)
    {
        int foundIndex = result.indexOf("{}");
        if(foundIndex != -1)
        {
            String s1 = result.substring(0, foundIndex);
            String s2 = result.substring(foundIndex + 2);
            result = s1 + args.at(i) + s2;
        }
    }
    return result;
}

int countSubString(String msg, String subString)
{
   int occurrences = 0;
   std::string::size_type pos = 0;
   std::string s = std::string(msg.c_str());
   std::string target = std::string(subString.c_str());;
   while ((pos = s.find(target, pos )) != std::string::npos) {
          ++ occurrences;
          pos += target.length();
   }
   return occurrences;

}