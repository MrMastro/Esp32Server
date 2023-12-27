#include <./services/Service.h>

template <typename T, typename... Args>
bool Service::attachPin(T first, Args... rest)
{
    logInfo("Attaching pins into vector pins");
    pins.push_back(first);
    ((pins.push_back(rest)), ...);
    preparePin();
    return true;
}