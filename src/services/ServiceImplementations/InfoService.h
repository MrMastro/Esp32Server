#ifndef InfoService_H
#define InfoService_H

#include <services/Service.h>
#include "SettingService.h"
#include <models/InfoEsp32Model/InfoEsp32Model.h>

class InfoService : public Service
{
public:
    InfoService();
    StatusInfo infoSuccess();
    String getIp();
    String getInfo();
    boolean loginValidate(String deviceName, String devicePwd);
    boolean checkAuthorization(String ip);
    boolean isAvaible() override;
    InfoEsp32Model getHelloInfo();
protected:
    void onInitServiceCollector() override;
private:
    SettingService *settingService;
    boolean isOperative;
};

#endif // InfoService_H