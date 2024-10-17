#include "Controllers.h"

String getBodyString(uint8_t *data, size_t len)
{
    // Crea una stringa per accumulare il body
    String body = "";

    // Accumula i dati del body
    for (size_t i = 0; i < len; i++) {
        body += (char)data[i];
    }

    return body;
}