#ifndef LedService_H
#define LedService_H

#include <./services/Service.h>

class LedService: public Service
{
public:
    LedService();
    LedService(int ledPin);
    boolean isAvaible() override;
    //void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;
    boolean attachPin(int pin) override;
    String executeJson(String methodName, String param) override;
    String executeJson(String methodName, std::vector<String> jsonParams) override;
    boolean changeLed(boolean active, boolean toggle);
private:
    boolean isAttachedLed;
    int ledPin;
    bool isLedOn;
};

#endif  // LedService_H
