#include <Arduino.h>
#include <services/Service.h>
#include <constants/constants.h>
#ifndef CommandService_H
#define CommandService_H


class CommandService : public Service
{
public:
    CommandService();
    CommandService(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam);
    boolean isAvaible() override;
    void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;
    boolean attachPins(std::vector<int> pins) override;
    String executeJson(String methodName, String param) override;
    String executeJson(String methodName, std::vector<String> jsonParams) override;
    //String executeStringMethod(String nameMethod, String param) override; deprecated
    String getClassName() const override;
    String recvMsgAndExecute(String data); //returns the msg received
private:
    boolean isOperative;
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    String executeCommand(CMD cmd, String cmdString);
};




#endif  // CommandService_H