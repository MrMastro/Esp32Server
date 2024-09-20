#ifndef SerialService_H
#define SerialService_H

#include <services/Service.h>
#include "./constants/constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>
#include "BluetoothSerial.h"

class SerialService : public Service
{
public:
    SerialService();
    SerialService(unsigned long baud, String deviceName);
    SerialService(unsigned long baud, String deviceName, AsyncWebServer *server, const char *url = "/webserial");
    boolean isAvaible() override;
    void initSerialBegin(unsigned long baud, uint32_t config = SERIAL_8N1, int8_t rxPin = -1, int8_t txPin = -1, bool invert = false, unsigned long timeout_ms = 20000UL, uint8_t rxfifo_full_thrhd = 112);
    void initSerialBtBegin(String localName = String(), bool isMaster = false);
    void initSerialWebBegin(AsyncWebServer *server, const char *url);

    // void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;

    void initAllSerials(unsigned long baud, String deviceName, AsyncWebServer *server, const char *url = "/webserial");
    
    boolean availableSerial();
    boolean availableSerialBt();

    String getMsgbySerial();
    String getMsgbyBluetooth();

    void logInfoln(String msg, String subject);
    void logWarning(String msg, String subject, String context);
    void logError(String msg, String subject, String context);

private:
    boolean isOperative;
    BluetoothSerial SerialBT;
protected:
    HardwareSerial* serialPointer;
    WebSerialClass* webSerialPointer;
    //BluetoothSerial* btSerialPointer;
};




#endif  // SerialService_H