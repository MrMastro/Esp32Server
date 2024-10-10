#include "InfoService.h"

InfoService::InfoService()
{
  isOperative = false;
}

StatusInfo InfoService::infoSuccess()
{
    return getStatusInfoByHttpCode(HTTP_CODE::OK);
}

String InfoService::getIp()
{
    return getServerByCollector()->getIp();
}

String InfoService::getInfo()
{
    String info = "WIP, info visualizated here";
    return info;
}

boolean InfoService::loginValidate(String deviceName, String devicePwd)
{
    SettingsModel s = settingService->getSettings();
    if (s.deviceName == deviceName && s.devicePassword == devicePwd)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void InfoService::onInitServiceCollector()
{
    settingService = ((SettingService *)servicesCollector.getService("SettingService"));
    isOperative = true;
}

boolean InfoService::isAvaible()
{
  return isOperative;
}
