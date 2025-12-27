#include "VectorStringResponse.h"

// Costruttori
VectorStringResponse::VectorStringResponse() : BasicResponse() {}

VectorStringResponse::VectorStringResponse(StatusInfo info) 
    : BasicResponse(info) {}

VectorStringResponse::VectorStringResponse(const int &code, const String &message, const String &description)
    : BasicResponse(code, message, description) {}

VectorStringResponse::VectorStringResponse(HTTP_CODE code)
    : BasicResponse(code) {}

VectorStringResponse::VectorStringResponse(HTTP_CODE code, String customDescription)
    : BasicResponse(code, customDescription) {}

VectorStringResponse::VectorStringResponse(const int &code, const String &message, const String &description, const std::vector<String> &data)
    : BasicResponse(code, message, description), data(data) {}

// Getter per il vettore data
std::vector<String> VectorStringResponse::getData() const {
    return data;
}

// Setter per il vettore data
void VectorStringResponse::setData(const std::vector<String> &newData) {
    data = newData;
}

String VectorStringResponse::toJson()
{
    // Richiama il metodo toJson di BasicResponse per serializzare i campi base
    String basicResponseJson = BasicResponse::toJson();

    // Crea un documento JSON che contiene il risultato di BasicResponse
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, basicResponseJson);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return "";  // Ritorna stringa vuota in caso di errore
    }

    // Aggiungi il vettore data al documento JSON
    JsonArray dataJsonArray = doc["data"].to<JsonArray>();
    for (const auto &item : data) {
        dataJsonArray.add(item);  // Aggiungi ciascun elemento del vettore al JSON
    }

    // Converte il documento in una stringa JSON
    String output;
    serializeJson(doc, output);
    return output;
}

bool VectorStringResponse::fromJson(const String &json)
{
    // Deserializza l'intero JSON nel documento
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return false;
    }

    // Deserializza i campi della classe base usando BasicResponse::fromJson
    String basicResponseJson;
    serializeJson(doc, basicResponseJson);  // Serializza il documento in una stringa
    if (!BasicResponse::fromJson(basicResponseJson)) {
        return false;  // Se la deserializzazione della classe base fallisce
    }

    // Deserializza il vettore data
    JsonArray dataJsonArray = doc["data"].as<JsonArray>();
    if (!dataJsonArray.isNull()) {
        data.clear();  // Pulisce il vettore prima di inserire nuovi dati
        for (JsonVariant v : dataJsonArray) {
            data.push_back(v.as<String>());  // Aggiunge ogni elemento del JSON nel vettore
        }
    } else {
        Serial.println(F("data array is null"));
        return false;
    }

    return true;
}

// Sovrascrive il metodo toString per includere i dati del vettore
String VectorStringResponse::toString() {
    String baseResponseString = BasicResponse::toString();
    
    // Costruisce la rappresentazione del vettore data
    String dataString = "[";
    for (const auto& item : data) {
        dataString += item + ", ";
    }
    dataString += "]";
    
    return baseResponseString + " Data: " + dataString;
}