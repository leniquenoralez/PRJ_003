#include "http_common.h"

int WriteResponse(httpRequestLine *request_line, httpRequestHeaders *request_headers, httpResponse *response)
{
    // TODO: check if cgi is enabled and execute cgi. write cgi results if uri starts with  /cgi-bin
    // TODO: generate response header
    // TODO: write index.html if it exists
    // TODO: if index.html does not exist the create on with the directory contents and the html contents
    // TODO: execute CGIs
    return 0;
}