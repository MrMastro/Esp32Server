#ifndef SerialService_H
#define SerialService_H

#define DEPRECATED __attribute__((deprecated))

#include <services/Service.h>
#include "./constants/Constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>
#ifndef EXCLUDE_BLUETOOTH
#ifndef EXCLUDE_BLUETOOTH
#include "BluetoothSerial.h"
#endif
#endif

class SerialService : public Service
{
public:
    SerialService();
    boolean isAvaible() override;
#ifndef EXCLUDE_BLUETOOTH
    void initSerialBtBegin(String localName, BluetoothSerial* bluetoothSerialPointer, bool isMaster = false);
#endif

    void attachSerial(HardwareSerial* serial);
    
    boolean availableSerial();
    boolean availableSerialBt();

    String getMsgbySerial();
    String getMsgbyBluetooth();

    void print(String msg);
    void println(String msg);
    void printColored(const String &msg, String colorMsg);
    void printlnColored(const String &msg, String colorMsg);
    void logInfoFixed(String msg, String subject);
    void logInfoln(String msg, String subject);
    void logInfoln(String msg, String subject, boolean debug);
    void logWarning(String msg, String subject, String context);
    void logError(String msg, String subject, String context);
protected:
    HardwareSerial* serialPointer;
    // Stream (BluetoothSerial ne deriva): print/read non dipendono dalla libreria Bluetooth.
    // Resta nullptr se il Bluetooth non e' attivo o e' escluso dalla build (EXCLUDE_BLUETOOTH)
    Stream* btSerialPointer;
    void onInitServiceCollector() override;
    
private:
    boolean isOperative;
    std::vector<String> lastLineMsg;
    boolean lastMsgIsFixed;
    const std::vector<String> getLastSentMsg();
    void setLastMessage(const String &msg, bool isLn);

};




#endif  // SerialService_H