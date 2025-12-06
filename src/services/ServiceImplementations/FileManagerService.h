#ifndef FileManagerService_H
#define FileManagerService_H

#include <services/Service.h>
#include "./services/ServiceImplementations/LedService.h"
#include "./constants/Constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>
#include <models/settingModel/SettingsModel.h>
#include <models/InitialSettingSaveModel/InitialSettingSaveModel.h>
#include <Arduino.h>
#include <LittleFS.h>
#include "SerialService.h"


class FileManagerService : public Service
{
public:
    FileManagerService();
    boolean writeFile(String path, String& content, bool create=false);
    String getFileData(String path);
    boolean fileExists(String path);
protected:
    void onInitServiceCollector() override;
private:
    boolean isOperative;
    SerialService* serialService;
};

#endif // FileManagerService_H