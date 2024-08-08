#ifndef SettingService_H
#define SettingService_H

#include <settings/settingsDefault.h>
#include <services/Service.h>
#include "./services/ServiceImplementations/LedService.h"
#include "./constants/constants.h"
#include <models/DataModelling.h>
#include <utils/SerialSimple.h>
#include <exceptions/exceptions.h>
#include <models/settingModel/SettingsModel.h>
#include <Arduino.h>
#include <LittleFS.h>


class SettingService : public Service
{
public:
    SettingService();
    SettingsModel getSettings();
    void loadSettings(String path);
private:
    boolean isOperative;
    SettingsModel settings;
};




#endif  // SettingService_H