
#include <sys/types.h>
typedef struct
{
    char method[1024];
    char uri[1024];
    char version[1024];
    char **headers;
} HttpRequest;
int ReadHttpRequest(int client_fd, char request_string[], size_t *bytes_read);
int ParseHttpRequest(char *request_message, int request_length, HttpRequest *parsed_request);

