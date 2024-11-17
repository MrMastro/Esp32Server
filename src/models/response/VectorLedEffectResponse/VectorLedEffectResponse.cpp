#include "VectorLedEffectResponse.h"

VectorLedEffectResponse::VectorLedEffectResponse(HTTP_CODE code)
    : BasicResponse(code) {}

std::vector<Effect*> VectorLedEffectResponse::getData() const {
    return data;
}


void VectorLedEffectResponse::setData(const std::vector<Effect*> &newData) {
    data = newData;
}

String VectorLedEffectResponse::toJson() {
    StaticJsonDocument<1024> doc;
    JsonArray effectsArray = doc.createNestedArray("effects");
    for (auto effect : data) {
        if (effect != nullptr) {
            StaticJsonDocument<256> effectJson;
            DeserializationError error = deserializeJson(effectJson, effect->toJson());

            if (!error) {
                effectsArray.add(effectJson);
            }
        }
    }
    String output;
    serializeJson(doc, output);
    return output;
}