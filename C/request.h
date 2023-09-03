
#include <sys/types.h>
#include <poll.h>
#define REQUEST_BUFFER_SIZE 1024

typedef struct HttpRequestLine
{
    char method[REQUEST_BUFFER_SIZE];
    char uri[REQUEST_BUFFER_SIZE];
    char version[REQUEST_BUFFER_SIZE];
} httpRequestLine;
typedef struct HttpGeneralHeaders
{
    char date[REQUEST_BUFFER_SIZE];
    char pragma[REQUEST_BUFFER_SIZE];
} httpGeneralHeaders;

typedef struct HttpEntityHeaders
{
    char allow[REQUEST_BUFFER_SIZE];
    char content_encoding[REQUEST_BUFFER_SIZE];
    char content_length[REQUEST_BUFFER_SIZE];
    char content_type[REQUEST_BUFFER_SIZE];
    char expires[REQUEST_BUFFER_SIZE];
    char last_modified[REQUEST_BUFFER_SIZE];
    char extension_header[REQUEST_BUFFER_SIZE];
} httpEntityHeaders;
typedef struct HttpBaseHeaders
{
    httpGeneralHeaders general;
    httpEntityHeaders entity;
} httpBaseHeaders;
typedef struct HttpRequestHeaders
{
    httpBaseHeaders base;
    char authorization[REQUEST_BUFFER_SIZE];
    char from[REQUEST_BUFFER_SIZE];
    char if_modified_since[REQUEST_BUFFER_SIZE];
    char referer[REQUEST_BUFFER_SIZE];
    char user_agent[REQUEST_BUFFER_SIZE];

} httpRequestHeaders;
typedef struct HttpResponseHeaders
{
    httpBaseHeaders base;
} httpResponseHeaders;

int ReadHttpRequest(int client_fd, char request_string[], size_t *bytes_read);
int ParseHttpRequest(char *request_message, int request_length, char *request, char *headers);
int ParseHttpRequestLine(char *request, httpRequestLine *request_line);
int ParseHttpRequestHeaders(char *headers, httpRequestHeaders *request_headers);
void HandleRequest(struct pollfd poll_fds[], int index, int *connected_sockets_count);