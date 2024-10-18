#ifndef VectorResponse_H
#define VectorResponse_H

#include "./models/StatusInfo.h"
#include <./constants/Constants.h>

#include <vector>


class VectorResponse : public BasicResponse
{
public:
    // Costruttori
    VectorResponse();
    VectorResponse(StatusInfo info);
    VectorResponse(const int &code, const String &message, const String &description);
    VectorResponse(HTTP_CODE code);
    VectorResponse(HTTP_CODE code, String customDescription);
    VectorResponse(const int &code, const String &message, const String &description, const std::vector<String> &data);

    // Getter per il vettore data
    std::vector<String> getData() const;

    // Setter per il vettore data
    void setData(const std::vector<String> &newData);

    String toJson(); // Sovrascrivi il metodo toJson
    bool fromJson(const String &json); // Sovrascrivi il metodo fromJson
    String toString();

private:
    std::vector<String> data; // Nuovo membro variabile per contenere i dati
};

#endif