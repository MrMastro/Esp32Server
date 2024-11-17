#include "InfoService.h"

// This Service works to manage information about status of system and various information (like ip, authorization, information like communication, etc.)

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

boolean InfoService::checkAuthorization(String ip){
    return true; //todo authorization with array of logged ip
}

boolean InfoService::isAvaible()
{
  return isOperative;
}

InfoEsp32Model InfoService::getHelloInfo()
{
    InfoEsp32Model info = InfoEsp32Model();
    info.deviceName = getServerByCollector()->getName();
    info.ip = getIp();
    return info;
}
