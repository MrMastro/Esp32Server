#include "DataModelling.h"

// Function to convert a boolean to JSON
String simpleBooleanToJson(boolean paramBool) {
  // Create a JSON document
  StaticJsonDocument<50> doc;

  // Add the boolean value to the document
  doc["data"] = paramBool;

  // Serialize the JSON document to a String
  String jsonString;
  serializeJson(doc, jsonString);

  return jsonString;
}

// Function to convert JSON to a boolean
boolean jsonToSimpleBoolean(String jsonString) {
  // Parse the JSON document
  StaticJsonDocument<50> doc;
  DeserializationError error = deserializeJson(doc, jsonString);

  // Check for parsing errors
  if (error) {
    Serial.print("Error parsing JSON: ");
    Serial.println(error.c_str());
    return false;
  }

  // Extract the boolean value from the document
  bool paramBool = doc["data"];

  return paramBool;
}

StatusInfo jsonToStatusInfo(String &json)
{
    // Create a JSON document to parse the incoming JSON string
    StaticJsonDocument<200> jsonDocument;

    // Parse the JSON string
    DeserializationError error = deserializeJson(jsonDocument, json);

    // Check for parsing errors
    if (error)
    {
        // Handle parsing errors here
        Serial.print("Parsing failed: ");
        Serial.println(error.c_str());

        // Return a default-constructed StatusInfo indicating an error
        return StatusInfo();
    }

    // Retrieve data from the JSON document
    int code = jsonDocument["code"];
    String message = jsonDocument["message"].as<String>();
    String description = jsonDocument["description"].as<String>();

    // Create and return a StatusInfo object
    StatusInfo info;
    info.setMessage(message);
    info.setDescription(description);

    return info;
}

String dtoToJson(StatusInfo &info)
{
    // Create a JSON document with enough capacity to store the data
    StaticJsonDocument<200> jsonDocument;

    // Add data to the JSON document
    jsonDocument["message"] = info.getMessage();
    jsonDocument["description"] = info.getDescription();

    // Create a String to store the JSON
    String jsonString;

    // Serialize the JSON document to the String
    serializeJson(jsonDocument, jsonString);

    // Return the JSON as a String
    return jsonString;
}

BasicResponse jsonToDto(String &json){
    // Create a JSON document to parse the incoming JSON string
    StaticJsonDocument<200> jsonDocument;

    // Parse the JSON string
    DeserializationError error = deserializeJson(jsonDocument, json);

    // Check for parsing errors
    if (error)
    {
        // Handle parsing errors here
        Serial.print("Parsing failed: ");
        Serial.println(error.c_str());

        // Return a default-constructed BasicResponse indicating an error
        return BasicResponse();
    }

    // Retrieve data from the JSON document
    int code = jsonDocument["code"];
    String message = jsonDocument["message"].as<String>();
    String description = jsonDocument["description"].as<String>();
    String dataJson = jsonDocument["data"].as<String>();

    // Create and return a BasicResponse object
    return BasicResponse(message, description, dataJson);
}

String dtoToJson(BasicResponse &data){
    // Create a JSON document with enough capacity to store the data
    StaticJsonDocument<200> jsonDocument;
    StaticJsonDocument<200> jsonInnerDocument;
    // Add data to the JSON document
    // StatusInfo statusN = data.getStatus();
    // String jsonStatus = dtoToJson(statusN);
    // jsonStatus.replace("\"", "");
    // jsonDocument["status"] = jsonStatus;

    // Convert the struct to a JsonObject
    JsonObject statusObject = jsonInnerDocument.to<JsonObject>();
    statusObject["message"] =  data.getStatus().getMessage();
    statusObject["description"] = data.getStatus().getDescription();
    jsonDocument["status"] = statusObject;

    if(data.getDataJson() != ""){
        jsonDocument["data"]= data.getDataJson();
    }

    // Create a String to store the JSON
    String jsonString;

    // Serialize the JSON document to the String
    serializeJson(jsonDocument, jsonString);

    // Return the JSON as a String
    return jsonString;
}
