#include "SerialSimple.h"
#include <vector>

void differentSerialprintln(const String &msg, String colorMsg, HardwareSerial *serial, WebSerialClass *webSerial)
{
    String msgColored = colorMsg + msg + "\033[0m";
    delay(15);
    if (serial != nullptr)
    {
        colorMsg == "" ? serial->println(msg.c_str()) : serial->println(msgColored.c_str());
    }

    if (webSerial != nullptr)
    {
        webSerial->println(msg.c_str());
    }
    delay(15);
}

void differentSerialprint(const String &msg, String colorMsg, HardwareSerial *serial, WebSerialClass *webSerial)
{
    String msgColored = colorMsg + msg + "\033[0m";
    delay(25);
    if (serial != nullptr)
    {
        colorMsg == "" ? webSerial->println(msg.c_str()) : serial->println(msgColored.c_str());
    }

    if (webSerial != nullptr)
    {
        webSerial->print(msg.c_str());
    }
    delay(25);
}

String formatMsg(String msg, std::vector<String> args){
    String result = msg;
    size_t pos = 0;

    int count = countSubString(msg,"{}");
    if(count<args.size())
    {
        //ERROR
        return msg;
    }
    for(int i=0; i<count; i++)
    {
        msg.replace("{}",args.at(i));
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