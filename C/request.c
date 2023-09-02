#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include "request.h"

#define BUFFER_SIZE 1024

typedef struct
{
    char method[1024];
    char uri[1024];
    char version[1024];
    char **headers;
} HttpRequest;

int ReadHttpRequest(int client_fd, char request_string[], size_t *message_length)
{
    char data_buffer[BUFFER_SIZE];
    ssize_t received_bytes;
    char *end_of_line_pointer;
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
    if (strncpy(request_headers, request_message + headers_start_pos, headers_length) == NULL)
    {
        return -1;
    }
    request_headers[headers_length] = '\0';
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

int ParseHttpRequestLine(char *request, httpRequestLine *request_line){
    if (sscanf(request, "%s %s %s", request_line->method, request_line->uri, request_line->version) != 3)
    {
        return -1;
    }
    return 0;
}

int ParseHttpRequestHeaders(char *headers, httpRequestHeaders request_headers){

}