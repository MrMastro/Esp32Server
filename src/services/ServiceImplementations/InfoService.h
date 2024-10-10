#ifndef InfoService_H
#define InfoService_H

#include <services/Service.h>
#include "SettingService.h"

class InfoService : public Service
{
public:
    InfoService();
    StatusInfo infoSuccess();
    String getIp();
    String getInfo();
    boolean loginValidate(String deviceName, String devicePwd);
    boolean isAvaible() override;
protected:
    void onInitServiceCollector() override;;
private:
    SettingService *settingService;
    boolean isOperative;
};

#endif // InfoService_H