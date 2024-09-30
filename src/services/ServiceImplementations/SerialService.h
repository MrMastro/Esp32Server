#ifndef SerialService_H
#define SerialService_H

#define DEPRECATED __attribute__((deprecated))

#include <services/Service.h>
#include "./constants/Constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>
#include "BluetoothSerial.h"

class SerialService : public Service
{
public:
    SerialService();
    SerialService(unsigned long baud, String deviceName, BluetoothSerial* btlPointer, bool isMaster = false);   //Serial and SerialBluetooth
    DEPRECATED SerialService(unsigned long baud, BluetoothSerial* btlPointer, String deviceName, AsyncWebServer *server, const char *url = "/webserial"); //All Serials
    boolean isAvaible() override;
    String getLastSentMsg();
    void initSerialBegin(unsigned long baud, uint32_t config = SERIAL_8N1, int8_t rxPin = -1, int8_t txPin = -1, bool invert = false, unsigned long timeout_ms = 20000UL, uint8_t rxfifo_full_thrhd = 112);
    void initSerialBtBegin(String localName, BluetoothSerial* bluetoothSerialPointer, bool isMaster = false);
    void initSerialWebBegin(AsyncWebServer *server, const char *url);

    void attachSerial(HardwareSerial* serial);

    // void attachSerial(HardwareSerial* serialPointerParam, WebSerialClass* webSerialPointerParam) override;

    void initAllSerials(unsigned long baud, BluetoothSerial* bluetoothPointer, String deviceName,  AsyncWebServer *server, const char *url = "/webserial");
    
    boolean availableSerial();
    boolean availableSerialBt();

    String getMsgbySerial();
    String getMsgbyBluetooth();

    void logInfoln(String msg, String subject);
    void logWarning(String msg, String subject, String context);
    void logError(String msg, String subject, String context);
protected:
    HardwareSerial* serialPointer;
    BluetoothSerial* btSerialPointer;
    void onInitServiceCollector() override;
    
private:
    boolean isOperative;
    String lastSentMsg;

};




#endif  // SerialService_H