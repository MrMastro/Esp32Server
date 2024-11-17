#include "Controllers.h"
#include <models/response/VectorLedEffectResponse/VectorLedEffectResponse.h>
#include <models/LedEffectRequest/LedEffectRequest.h>

void setEffectWs2811(AsyncWebServerRequest *request)
{
    // todo introduce other param
    // todo check for parsing error String -> int
    servicesCollector.takeExclusiveExecution();
    BasicResponse response;
    String effect = request->arg("effect");
    int r = request->arg("r").toInt();
    int g = request->arg("g").toInt();
    int b = request->arg("b").toInt();
    int ms = request->arg("timing").toInt();
    boolean actionRgb = request->arg("rgbAction").equalsIgnoreCase("true");
    boolean actionWs2811 = request->arg("ws2811Action").equalsIgnoreCase("true");

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(COLOR_OUT_OF_RANGE_ERROR);
    }
    else if (!isPresentEffect(effect))
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(LED_EFFECT_LABEL_UKNOWN);
    }
    else
    {
        RgbColor(r, g, b);
        ((LedService *)servicesCollector.getService("LedService"))->startEffect(effect, RgbColor(r, g, b), ms, actionRgb, actionWs2811);
        response = BasicResponse(HTTP_CODE::OK);
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

    if (r < 0 || r > 255 || g < 0 || g > 255 || b < 0 || b > 255)
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(COLOR_OUT_OF_RANGE_ERROR);
    }
    else if (!isPresentEffect(effect))
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(LED_EFFECT_LABEL_UKNOWN);
    }
    else
    {
        RgbColor(r, g, b);
        ((LedService *)servicesCollector.getService("LedService"))->stopEffect(effect, RgbColor(r, g, b), ms, actionRgb, actionWs2811);
        response = BasicResponse(HTTP_CODE::OK);
    }

    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}

void setEffectWs2811_v2(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    servicesCollector.takeExclusiveExecution();
    BasicResponse response;
    String body = getBodyString(data, len);
    LedEffectRequest ledRequest;

    if (!ledRequest.fromJson(body))
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription("Errore Serializzazione");
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }

    String errorColor = validateColors(ledRequest.colors);
    if (!errorColor.isEmpty())
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription(errorColor);
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }

    if (!isPresentEffect(ledRequest.effect))
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(LED_EFFECT_LABEL_UKNOWN);
        String jsonResponse = dtoToJson(response);
        request->send(response.getStatus().getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }
    std::vector<RgbColor> rgbColors;

    for (size_t i = 0; i < ledRequest.colors.size(); i++)
    {
        LedColorRequest c = ledRequest.colors.at(i);
        rgbColors.push_back(RgbColor(c.r, c.g, c.b));
    }

    ((LedService *)servicesCollector.getService("LedService"))->startEffect(ledRequest.effect, rgbColors, ledRequest.ms, ledRequest.rgbAction, ledRequest.ws2811Action);
    response = BasicResponse(HTTP_CODE::OK);

    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}

void stopEffectWs2811_v2(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    servicesCollector.takeExclusiveExecution();
    BasicResponse response;
    String body = getBodyString(data, len);
    LedEffectRequest ledRequest;

    if (!ledRequest.fromJson(body))
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription("Errore Serializzazione");
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }

    String errorColor = validateColors(ledRequest.colors);
    if (!errorColor.isEmpty())
    {
        StatusInfo s = getStatusInfoByHttpCode(HTTP_CODE::BadRequest);
        s.setDescription(errorColor);
        response = BasicResponse(s);
        String jsonResponse = dtoToJson(response);
        request->send(s.getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }

    if (!isPresentEffect(ledRequest.effect))
    {
        response = BasicResponse(HTTP_CODE::BadRequest);
        response.getStatus().setDescription(LED_EFFECT_LABEL_UKNOWN);
        String jsonResponse = dtoToJson(response);
        request->send(response.getStatus().getCode(), "application/json", jsonResponse);
        servicesCollector.freeExclusiveExecution();
        return;
    }
    std::vector<RgbColor> rgbColors;

    for (size_t i = 0; i < ledRequest.colors.size(); i++)
    {
        LedColorRequest c = ledRequest.colors.at(i);
        rgbColors.push_back(RgbColor(c.r, c.g, c.b));
    }

    ((LedService *)servicesCollector.getService("LedService"))->stopEffect(ledRequest.effect, rgbColors, ledRequest.ms, ledRequest.rgbAction, ledRequest.ws2811Action);
    response = BasicResponse(HTTP_CODE::OK);

    String jsonResponse = dtoToJson(response);
    request->send(200, "application/json", jsonResponse);
    servicesCollector.freeExclusiveExecution();
}

void getAvaibleEffects(AsyncWebServerRequest *request)
{
    servicesCollector.takeExclusiveExecution();
    VectorLedEffectResponse response = VectorLedEffectResponse(HTTP_CODE::OK);
    std::vector<Effect*> data = ((LedService *)servicesCollector.getService("LedService"))->getAvaibleEffects();
    response.setData(data);

    request->send(200, "application/json", response.toJson());
    servicesCollector.freeExclusiveExecution();
}