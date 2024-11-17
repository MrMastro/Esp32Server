#ifndef SettingsResponse_H
#define SettingsResponse_H

#define DEPRECATED __attribute__((deprecated))

#include "./models/StatusInfo.h"
#include <./constants/Constants.h>
#include <models/response/BasicResponse/BasicResponse.h>
#include <models/settingModel/SettingsModel.h>

enum class HTTP_CODE;

class StatusInfo;

class SettingsResponse : public BasicResponse 
{
public:
    SettingsResponse();
    SettingsResponse(StatusInfo info);
    // SettingsResponse(StatusInfo info, const SettingsModel &data);
    // SettingsResponse(const int &code, const String &message, const String &description, const SettingsModel &dataInput);
    StatusInfo getStatus();
    void setStatus(StatusInfo &status);
    SettingsModel getDataJson();
    void setDataJson(const SettingsModel &json);
    String toString();
    String toJson();
    bool fromJson(const String &json);

private:
    StatusInfo status;
    SettingsModel data;
};

#endif //SettingsResponse