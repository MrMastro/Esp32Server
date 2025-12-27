#include "StatusInfo.h"

StatusInfo::StatusInfo() : code(0), message(""), description() {}

StatusInfo::StatusInfo(const int &code, const String &message, const String &description)
    : code(code), message(message), description(description) {}

String StatusInfo::getMessage()
{
    return message;
}

String StatusInfo::getDescription()
{
    return description;
}

int StatusInfo::getCode()
{
    return code;
}

void StatusInfo::setMessage(const String &newMessage)
{
    message = newMessage;
}

void StatusInfo::setDescription(const String &newDescription)
{
    description = newDescription;
}

String StatusInfo::toString()
{
    String strCode = "" + code;
    return "{ " + strCode + "," + message + ", " + description + " }";
}

String StatusInfo::toJson() const
{
    JsonDocument doc;

    doc["code"] = code;
    doc["message"] = message;
    doc["description"] = description;

    String output;
    serializeJson(doc, output);
    return output;
}

bool StatusInfo::fromJson(const String &json)
{
    JsonDocument doc;  // Documento JSON con un buffer massimo di 256 byte
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.println("Errore di deserializzazione JSON: ");
        Serial.println(error.c_str());
        return false;
    }

    // Assicuriamoci che i campi siano presenti
    if (doc["code"].is<int>() && doc["message"].is<String>() && doc["description"].is<String>()) {
        code = doc["code"];
        message = doc["message"].as<String>();
        description = doc["description"].as<String>();
        return true;
    }

    return false;  // Ritorna false se mancano dei campi
}
