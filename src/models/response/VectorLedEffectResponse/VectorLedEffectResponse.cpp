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
    JsonDocument doc;
    JsonArray effectsArray = doc["effects"].to<JsonArray>();

    for (auto effect : data) {
        if (effect != nullptr) {
            JsonDocument effectJson;
            DeserializationError error = deserializeJson(effectJson, effect->toJson());

            if (!error) {
                effectsArray.add(effectJson.as<JsonObject>());
            }
        }
    }

    String output;
    serializeJson(doc, output);
    return output;
}