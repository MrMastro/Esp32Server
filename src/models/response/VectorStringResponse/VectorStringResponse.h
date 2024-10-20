#ifndef VectorStringResponse_H
#define VectorStringResponse_H

#include "./models/StatusInfo.h"
#include <./constants/Constants.h>
#include <vector>


class VectorStringResponse : public BasicResponse
{
public:
    // Costruttori
    VectorStringResponse();
    VectorStringResponse(StatusInfo info);
    VectorStringResponse(const int &code, const String &message, const String &description);
    VectorStringResponse(HTTP_CODE code);
    VectorStringResponse(HTTP_CODE code, String customDescription);
    VectorStringResponse(const int &code, const String &message, const String &description, const std::vector<String> &data);

    // Getter per il vettore data
    std::vector<String> getData() const;

    // Setter per il vettore data
    void setData(const std::vector<String> &newData);

    String toJson();
    bool fromJson(const String &json);
    String toString();

private:
    std::vector<String> data;
};

#endif