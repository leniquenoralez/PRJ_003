
#include <sys/types.h>
#include <poll.h>
#include "http_common.h"
#define REQUEST_BUFFER_SIZE 1024

int ReadHttpRequest(int client_fd, char request_string[], size_t *bytes_read);
int ParseHttpRequest(char *request_message, int request_length, char *request, char *headers);
int ParseHttpRequestLine(char *request, httpRequestLine *request_line);
int ParseHttpRequestHeaders(char *headers, httpRequestHeaders *request_headers);
int ReadRequest(struct pollfd poll_fds[], int index, int *connected_sockets_count, httpRequestLine *request_line, httpRequestHeaders *request_headers, httpResponse *response);