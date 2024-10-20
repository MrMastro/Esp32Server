#ifndef SettingService_H
#define SettingService_H

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


class SettingService : public Service
{
public:
    SettingService();
    SettingsModel getSettings();
    void loadSettings(String path);
    boolean saveSettings(String path, SettingsModel s);
    boolean saveInitialSettings(String path, InitialSettingSaveModel s);
    boolean changeSetting(String key, String value);
    String getJsonSettings();
protected:
    SettingsModel* settings;
    void onInitServiceCollector() override;
private:
    boolean isOperative;
    SerialService* serialService;
    boolean writeFile(String &path, String& content, bool create=false);
};

#endif // SettingService_H