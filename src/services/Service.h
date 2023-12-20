#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <vector>
#ifndef Services_H
#define Services_H

String pong();
String pongSuccess();

// Service interface
class Service {
public:
    // Pure virtual function makes this class abstract
    virtual boolean isAvaible() = 0;
    virtual void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) = 0;
    virtual boolean attachPins(std::vector<int> pins) = 0;
    virtual String executeJson(String methodName, String param) = 0;
    virtual String executeJson(String methodName, std::vector<String> jsonParams) = 0;
    virtual String getClassName() const = 0;
    virtual ~Service() {} // Virtual destructor for proper polymorphic destruction
};

#endif  // Services_H