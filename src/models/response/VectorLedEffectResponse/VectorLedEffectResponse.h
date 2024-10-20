#ifndef VectorLedEffectResponse_H
#define VectorLedEffectResponse_H

#include "./models/StatusInfo.h"
#include <./constants/Constants.h>
#include <ledEffects/Effect.h>
#include <vector>


class VectorLedEffectResponse : public BasicResponse
{
public:
    VectorLedEffectResponse(HTTP_CODE code);
    std::vector<Effect*> getData() const;
    void setData(const std::vector<Effect*> &newData);
    String toJson();

private:
    std::vector<Effect*> data;
};

#endif