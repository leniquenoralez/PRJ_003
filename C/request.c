#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>

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
    int line_read = 0;
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

        if (data_buffer[0] == '\r' && data_buffer[1] == '\n')
        {
            request_string[bytes_read] = '\0';
            (*message_length) = bytes_read;
            bytes_read = 0;
            break;
        }

        memcpy(request_string + bytes_read, data_buffer, received_bytes);
        bytes_read += received_bytes;
    }

    return 0;
}
int ReadRequestLine(char *request_message, char *request_line)
{
    printf("ReadRequestLine ==>\n");
    char *pos = strstr(request_message, "\r\n");
    char output[BUFFER_SIZE];

    if (pos == NULL)
    {
        return -1;
    }

    size_t length = pos - request_message;
    strncpy(request_line, request_message, length);
    request_line[length] = '\0';

    return length;
}
void ParseHttpRequest(char *request_message, int request_length, HttpRequest *parsed_request)
{
    int line_read = 0;
    int characters_read = 0;
    char request_line[BUFFER_SIZE];
    while (characters_read < request_length)
    {
        if (line_read == 0)
        {
            int request_line_length = 0;
            if ((request_line_length = ReadRequestLine(request_message, request_line)) == -1)
            {
                perror("ReadRequestLine");
            }
            printf("Request Line: %s %d\n", request_line, request_line_length);
            line_read++;
            characters_read += request_line_length;
            continue;
        }

        characters_read++;
    }
}