#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include <vector>
#include "./models/DataModelling.h"
#include <constants/constants.h>
#include <exceptions/exceptions.h>
#include <utils/FunctionUtils.h>
#include <utils/SerialSimple.h>

#ifndef Services_H
#define Services_H

class ServicesCollector;

String pong();
String pongSuccess();

// Service interface
class Service {
public:
    // Pure virtual function makes this class abstract
    virtual boolean isAvaible() = 0;
    void attachCollector(ServicesCollector* collectorParam);
    virtual void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam);
    virtual boolean attachPin(int pin);
    virtual String executeJson(String methodName, String param) = 0;
    virtual String executeJson(String methodName, std::vector<String> jsonParams) = 0;
    String executeMethodByCollector(String nameService,String nameMethod, String param);
    virtual String getClassName() const = 0;
    virtual ~Service() {} // Virtual destructor for proper polymorphic destruction
protected:
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    virtual void throwError(ERROR_CODE err, const char* detailMessage);
    virtual void logInfo(String msg);
    virtual void logError(String msg);
private:
    ServicesCollector* collector;
};

#endif  // Services_H