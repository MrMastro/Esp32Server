#include "Controllers.h"
#include <models/response/VectorResponse/VectorResponse.h>

void setEffectWs2811(AsyncWebServerRequest *request)
{
    // todo introduce other param
    //todo check for parsing error String -> int
    servicesCollector.takeExclusiveExecution();
    BasicResponse response;
    String effect = request->arg("effect");
    int r = request->arg("r").toInt();
    int g = request->arg("g").toInt();
    int b = request->arg("b").toInt();
    int ms = request->arg("timing").toInt();
    boolean actionRgb = request->arg("rgbAction").equalsIgnoreCase("true");
    boolean actionWs2811 = request->arg("ws2811Action").equalsIgnoreCase("true");
    WS2811_EFFECT effectEnum = WS2811EffectStringToEnum(effect);

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(COLOR_OUT_OF_RANGE_ERROR);
    }
    else
    {
        RgbColor(r, g, b);
        ((LedService *)servicesCollector.getService("LedService"))->startEffect(effectEnum, RgbColor(r,g,b), ms, actionRgb, actionWs2811);
        response = effectEnum != WS2811_EFFECT::UKNOWN_EFFECT ? BasicResponse(HTTP_CODE::OK) : BasicResponse(HTTP_CODE::BadRequest, String(WS2811_EFFECT_UKNOWN));
    }

    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}

void stopEffectWs2811(AsyncWebServerRequest *request)
{
    servicesCollector.takeExclusiveExecution();
    BasicResponse response;
    String effect = request->arg("effect");
    int r = request->arg("r").toInt();
    int g = request->arg("g").toInt();
    int b = request->arg("b").toInt();
    int ms = request->arg("timing").toInt();
    boolean actionRgb = request->arg("rgbAction").equalsIgnoreCase("true");
    boolean actionWs2811 = request->arg("ws2811Action").equalsIgnoreCase("true");
    WS2811_EFFECT effectEnum = WS2811EffectStringToEnum(effect);

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(COLOR_OUT_OF_RANGE_ERROR);
    }
    else
    {
        RgbColor(r, g, b);
        ((LedService *)servicesCollector.getService("LedService"))->stopEffect(effectEnum, RgbColor(r,g,b), ms, actionRgb, actionWs2811);
        response = effectEnum != WS2811_EFFECT::UKNOWN_EFFECT ? BasicResponse(HTTP_CODE::OK) : BasicResponse(HTTP_CODE::BadRequest, String(WS2811_EFFECT_UKNOWN));
    }

    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}

void getAvaibleEffects(AsyncWebServerRequest *request)
{
    servicesCollector.takeExclusiveExecution();
    VectorResponse response;
    response = VectorResponse(HTTP_CODE::OK);
    std::vector<String> data = ((LedService *)servicesCollector.getService("LedService"))->getAvaibleEffects();
    response.setData(data);

    request->send(200, "application/json", response.toJson());
    servicesCollector.freeExclusiveExecution();
}