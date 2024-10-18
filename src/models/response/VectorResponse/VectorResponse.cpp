#include "VectorResponse.h"

// Costruttori
VectorResponse::VectorResponse() : BasicResponse() {}

VectorResponse::VectorResponse(StatusInfo info) 
    : BasicResponse(info) {}

VectorResponse::VectorResponse(const int &code, const String &message, const String &description)
    : BasicResponse(code, message, description) {}

VectorResponse::VectorResponse(HTTP_CODE code)
    : BasicResponse(code) {}

VectorResponse::VectorResponse(HTTP_CODE code, String customDescription)
    : BasicResponse(code, customDescription) {}

VectorResponse::VectorResponse(const int &code, const String &message, const String &description, const std::vector<String> &data)
    : BasicResponse(code, message, description), data(data) {}

// Getter per il vettore data
std::vector<String> VectorResponse::getData() const {
    return data;
}

// Setter per il vettore data
void VectorResponse::setData(const std::vector<String> &newData) {
    data = newData;
}

String VectorResponse::toJson()
{
    // Richiama il metodo toJson di BasicResponse per serializzare i campi base
    String basicResponseJson = BasicResponse::toJson();

    // Crea un documento JSON che contiene il risultato di BasicResponse
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, basicResponseJson);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return "";  // Ritorna stringa vuota in caso di errore
    }

    // Aggiungi il vettore data al documento JSON
    JsonArray dataJsonArray = doc.createNestedArray("data");
    for (const auto &item : data) {
        dataJsonArray.add(item);  // Aggiungi ciascun elemento del vettore al JSON
    }

    // Converte il documento in una stringa JSON
    String output;
    serializeJson(doc, output);
    return output;
}

bool VectorResponse::fromJson(const String &json)
{
    // Deserializza l'intero JSON nel documento
    StaticJsonDocument<1024> doc;
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
String VectorResponse::toString() {
    String baseResponseString = BasicResponse::toString();
    
    // Costruisce la rappresentazione del vettore data
    String dataString = "[";
    for (const auto& item : data) {
        dataString += item + ", ";
    }
    dataString += "]";
    
    return baseResponseString + " Data: " + dataString;
}