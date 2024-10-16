#ifndef SettingsResponse_H
#define SettingsResponse_H

#define DEPRECATED __attribute__((deprecated))

#include "./models/StatusInfo.h"
#include <./constants/Constants.h>
#include <models/settingModel/SettingsModel.h>

enum class HTTP_CODE;

class StatusInfo;

class SettingsResponse
{
public:
    SettingsResponse();
    SettingsResponse(StatusInfo info);
    SettingsResponse(StatusInfo info, const SettingsModel &dataJson);
    SettingsResponse(const int &code, const String &message, const String &description, const SettingsModel &dataJsonInput);
    SettingsResponse(HTTP_CODE code);
    SettingsResponse(HTTP_CODE code, String customDescription);
    StatusInfo getStatus();
    void setStatus(StatusInfo &status);
    SettingsModel getDataJson();
    void setDataJson(const SettingsModel &json);
    String toString();
    String toJson();
    bool fromJson(const String &json);

private:
    StatusInfo status;
    SettingsModel dataJson;
};

#endif //SettingsResponse