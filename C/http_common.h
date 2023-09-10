#ifndef http_common_h
#define http_common_h

#define BUFFER_SIZE 1024

typedef struct HttpGeneralHeaders
{
    char date[BUFFER_SIZE];
    char pragma[BUFFER_SIZE];
} httpGeneralHeaders;

typedef struct HttpEntityHeaders
{
    char allow[BUFFER_SIZE];
    char content_encoding[BUFFER_SIZE];
    char content_length[BUFFER_SIZE];
    char content_type[BUFFER_SIZE];
    char expires[BUFFER_SIZE];
    char last_modified[BUFFER_SIZE];
    char extension_header[BUFFER_SIZE];
} httpEntityHeaders;

typedef struct HttpBaseHeaders
{
    httpGeneralHeaders general;
    httpEntityHeaders entity;
} httpBaseHeaders;

typedef struct HttpRequestLine
{
    char method[BUFFER_SIZE];
    char uri[BUFFER_SIZE];
    char version[BUFFER_SIZE];
} httpRequestLine;

typedef struct HttpRequestHeaders
{
    httpBaseHeaders base;
    char authorization[BUFFER_SIZE];
    char from[BUFFER_SIZE];
    char if_modified_since[BUFFER_SIZE];
    char referer[BUFFER_SIZE];
    char user_agent[BUFFER_SIZE];

} httpRequestHeaders;





typedef struct HttpResponseHeaders
{
    httpBaseHeaders base;
} httpResponseHeaders;

typedef struct HttpResponse
{
    int status_code;
    char message[BUFFER_SIZE];
    int message_length;
    httpResponseHeaders headers;
} httpResponse;
#endif