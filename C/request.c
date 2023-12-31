#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>
#include "request.h"
#include <stdlib.h>
#define BUFFER_SIZE 1024

const char *GENERAL_HEADERS[] = {
    "Date",
    "Pragma",
};

const char *REQUEST_HEADERS[] = {
    "Authorization",
    "From",
    "If-Modified-Since",
    "Referer",
    "User-Agent",
};

const char *ENTITY_HEADERS[] = {
    "Allow",
    "Content-Encoding",
    "Content-Length",
    "Content-Type ",
    "Expires",
    "Last-Modified",
    "extension-header",
};

const char *RESPONSE_HEADERS[] = {
    "Location",
    "Server",
    "WWW-Authenticate",
};

const char *RESPONSE_METHODS[] = {
    "GET",
    "HEAD",
};

int ReadHttpRequest(int client_fd, char request_string[], size_t *message_length)
{
    char data_buffer[BUFFER_SIZE];
    ssize_t received_bytes;
    size_t bytes_read = 0;
    memset(request_string, '\0', BUFFER_SIZE * 2);

    while (1)
    {
        received_bytes = recv(client_fd, data_buffer, sizeof(data_buffer), 0);
        if (received_bytes < 0)
        {
            perror("recv");
            return -1;
        }

        if (received_bytes == 0)
        {
            printf("Connection closed by peer.\n");
            return -1;
        }

        if (bytes_read <= 2 && data_buffer[0] == '\r' && data_buffer[1] == '\n')
        {
            continue;
        }

        memcpy(request_string + bytes_read, data_buffer, received_bytes);
        bytes_read += received_bytes;
        if (data_buffer[0] == '\r' && data_buffer[1] == '\n')
        {
            request_string[bytes_read] = '\0';
            (*message_length) = bytes_read;
            bytes_read = 0;
            break;
        }
    }

    return 0;
}
int ReadRequestLine(char *request_message, char *request_line, size_t request_length)
{
    size_t length = 0;
    size_t i = 0;
    while ((request_message[i] != '\r' && request_message[i + 1] != '\n') && i < request_length)
    {
        i++;
        length++;
    }
    strncpy(request_line, request_message, length);
    request_line[length] = '\0';

    return length;
}

int ReadHttpHeaders(char *request_message, size_t request_length, char *request_headers, size_t request_line_length)
{
    size_t headers_start_pos = request_line_length + strlen("\r\n");
    size_t headers_length = 0;
    size_t i = headers_start_pos;
    size_t end = request_length - strlen("\r\n\r\n");
    while (i < end)
    {
        i++;
        headers_length++;
    }
    if (strncpy(request_headers, request_message + headers_start_pos, headers_length + strlen("\r\n")) == NULL)
    {
        return -1;
    }
    request_headers[headers_length + 2] = '\0';
    return headers_length;
}
int ParseHttpRequest(char *request_message, int request_length, char *request, char *headers)
{
    size_t request_line_length = ReadRequestLine(request_message, request, request_length);
    if (request_line_length <= 0)
    {
        return -1;
    }

    size_t headers_length = ReadHttpHeaders(request_message, request_length, headers, request_line_length);
    if (headers_length < 0)
    {
        return -1;
    }

    return 0;
}

int Includes(const char *array[], int size, const char *str)
{
    for (int i = 0; i < size; i++)
    {
        if (strcmp(str, array[i]) == 0)
        {
            return 0;
        }
    }
    return -1;
}

void Trim(char *string)
{
    char *end = string + strlen(string) - 1;
    while (end > string && isspace((unsigned char)*end))
    {
        *end-- = '\0';
    }
}
int ParseHttpRequestLine(char *request, httpRequestLine *request_line)
{
    if (sscanf(request, "%s %s %s", request_line->method, request_line->uri, request_line->version) != 3)
    {
        return -1;
    }
    return 0;
}
int IsGeneralHeader(const char *header_key)
{
    int num_general_header = sizeof(GENERAL_HEADERS) / sizeof(GENERAL_HEADERS[0]);

    return Includes(GENERAL_HEADERS, num_general_header, header_key);
}
int IsEntityHeader(const char *header_key)
{
    int num_entity_header = sizeof(ENTITY_HEADERS) / sizeof(ENTITY_HEADERS[0]);

    return Includes(ENTITY_HEADERS, num_entity_header, header_key);
}
int IsRequestHeader(const char *header_key)
{
    int num_request_headers = sizeof(REQUEST_HEADERS) / sizeof(REQUEST_HEADERS[0]);

    return Includes(REQUEST_HEADERS, num_request_headers, header_key);
}
int IsResponseHeader(const char *header_key)
{
    int num_response_headers = sizeof(RESPONSE_HEADERS) / sizeof(RESPONSE_HEADERS[0]);

    return Includes(RESPONSE_HEADERS, num_response_headers, header_key);
}

int IsRequestMethodValid(const char *request_method)
{
    int num_response_methods = sizeof(RESPONSE_METHODS) / sizeof(RESPONSE_METHODS[0]);

    return Includes(RESPONSE_METHODS, num_response_methods, request_method);
}
int SetRequestHeader(char *key, char *value, httpRequestHeaders *request_headers)
{

    size_t length = strlen(value);
    if (strcmp(key, "Authorization") == 0)
    {
        strncpy(request_headers->authorization, value, length);
        request_headers->authorization[length] = '\0';
        return 0;
    }
    if (strcmp(key, "From") == 0)
    {
        strncpy(request_headers->from, value, length);
        request_headers->from[length] = '\0';
        return 0;
    }
    if (strcmp(key, "If-Modified-Since") == 0)
    {
        strncpy(request_headers->if_modified_since, value, length);
        request_headers->if_modified_since[length] = '\0';
        return 0;
    }
    if (strcmp(key, "Referer") == 0)
    {
        strncpy(request_headers->referer, value, length);
        request_headers->referer[length] = '\0';
        return 0;
    }
    if (strcmp(key, "User-Agent") == 0)
    {
        strncpy(request_headers->user_agent, value, length);
        request_headers->user_agent[length] = '\0';
        return 0;
    }
    char error_message[100];
    sprintf(error_message, "Unknown Request Header %s:", key);
    perror(error_message);
    return -1;
}

int SetHttpRequestHeader(char *header, httpRequestHeaders *request_headers)
{
    char header_key[64];
    char header_val[256];

    if (sscanf(header, " %49[^:]: %49[^\n]", header_key, header_val) != 2)
    {
        perror("SetHttpRequestHeader: Key");
    }
    Trim(header_key);
    Trim(header_val);
    if (IsGeneralHeader(header_key) == 0)
    {
        printf("is general header!!!\n");
        return 0;
    }
    if (IsEntityHeader(header_key) == 0)
    {
        printf("is entity header!!!\n");
        return 0;
    }
    if (IsRequestHeader(header_key) == 0)
    {
        return SetRequestHeader(header_key, header_val, request_headers);
    }
    if (IsResponseHeader(header_key) == 0)
    {
        printf("is response header!!!\n");
        return 0;
    }
    char error_message[100];
    sprintf(error_message, "Unknown Header %s:", header_key);
    perror(error_message);
    return -1;
}
int ParseHttpRequestHeaders(char *headers, httpRequestHeaders *request_headers)
{
    char *start_pos = headers;
    char current_header[BUFFER_SIZE];

    while (*start_pos != '\0')
    {
        memset(current_header, '\0', BUFFER_SIZE);

        char *end_pos = strstr(start_pos, "\r\n");
        int length = end_pos - start_pos;
        strncpy(current_header, start_pos, length);
        current_header[length] = '\0';
        SetHttpRequestHeader(current_header, request_headers);
        start_pos = end_pos;
        start_pos += strlen("\r\n");
    }
    return 0;
}
int ValidateHttpRequestHeaders(httpRequestHeaders *request_headers)
{
    return 0;
}

char *combinePaths(const char *relativePath)
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("getcwd() error");
        return NULL;
    }

    char *combinedPath = (char *)malloc(strlen(cwd) + strlen(relativePath) + 2); // +2 for the slash and null terminator
    if (combinedPath == NULL)
    {
        perror("malloc() error");
        return NULL;
    }

    strcpy(combinedPath, cwd);
    strcat(combinedPath, relativePath);

    return combinedPath;
}
int isValidPath(const char *path)
{
    // TODO: properly handle errors, [EACCES, ELOOP, ENAMETOOLONG, ENOENT, ENOTDIR, EROFS, EFAULT, EINVAL, EIO, ENOMEM, ETXTBSY]
    // TODO: check if path starts with ~ and resolve to /home/<user>/sws/
    // TODO: check if path starts with /cgi-bin then and the -c dir is provided then the path is resolve to dir provided with the flag. eg. uri: /cgi-bin/test -c some-folder then uri resolves to /some-folder/test
    char *fullPath = combinePaths(path);
    return access(fullPath, F_OK);
}
int IsHttpVersionValid(const char *http_version)
{
    if (strcmp(http_version, "HTTP/1.0") != 0)
    {
        return -1;
    }
    return 0;
};

int ValidateHttpRequestLine(httpRequestLine *request_line, httpResponse *response)
{
    if (IsRequestMethodValid(request_line->method) == -1)
    {
        response->message_length = 29;
        strncpy(response->message, "Bad Request - Unknown Method", response->message_length);
        response->status_code = 400;
        return -1;
    }
    if (isValidPath(request_line->uri) == -1)
    {
        response->message_length = 11;
        strncpy(response->message, "Not Found", response->message_length);
        response->status_code = 404;
        return -1;
    }
    char *fullPath = combinePaths(request_line->uri);
    memset(request_line->uri, '\0', BUFFER_SIZE);
    strncpy(request_line->uri, fullPath, strlen(fullPath) + 1);

    if (IsHttpVersionValid(request_line->version) == -1)
    {
        response->message_length = 35;
        strncpy(response->message, "Bad Request - Unknown HTTP version", response->message_length);
        response->status_code = 400;
        return -1;
    }
    return 0;
}

int ReadRequest(struct pollfd poll_fds[], int index, int *connected_sockets_count, httpRequestLine *request_line, httpRequestHeaders *request_headers, httpResponse *response)
{

    int client_fd = poll_fds[index].fd;

    char read_request[BUFFER_SIZE * 2];
    size_t request_length = 0;


    char request[BUFFER_SIZE];
    char headers[BUFFER_SIZE];

    if (ReadHttpRequest(client_fd, read_request, &request_length) == -1)
    {
        response->message_length = 11;
        strncpy(response->message, "Bad Request", response->message_length);
        response->status_code = 400;
        return -1;
    }
    if (ParseHttpRequest(read_request, request_length, request, headers) == -1)
    {
        response->message_length = 11;
        strncpy(response->message, "Bad Request", response->message_length);
        response->status_code = 400;
        return -1;
    }
    if (ParseHttpRequestLine(request, request_line) == -1)
    {
        response->message_length = 11;
        strncpy(response->message, "Bad Request", response->message_length);
        response->status_code = 400;
        return -1;
    }
    if (ValidateHttpRequestLine(request_line, response) == -1)
    {
        return -1;
    }

    if (ParseHttpRequestHeaders(headers, request_headers) == -1)
    {
        response->message_length = 29;
        strncpy(response->message, "Bad Request - Unknown Header", response->message_length);
        response->status_code = 400;
        return -1;
    }

    if (ValidateHttpRequestHeaders(request_headers) == -1)
    {
        response->message_length = 29;
        strncpy(response->message, "Bad Request - Invalid Header", response->message_length);
        response->status_code = 400;
        return -1;
    }
    response->message_length = 3;
    strncpy(response->message, "OK", response->message_length);
    response->status_code = 200;
    return 0;
}