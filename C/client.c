#include <sys/socket.h>
#include <poll.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "response.h"
#include "request.h"
#include <errno.h>
void swap(struct pollfd *a, struct pollfd *b)
{
    struct pollfd temp = *a;
    *a = *b;
    *b = temp;
}

void AddClient(struct pollfd poll_fds[], int new_client_fd, int *connected_sockets_count, int *max_connected_sockets)
{
    if (*connected_sockets_count == *max_connected_sockets)
    {
        *max_connected_sockets *= 2;

        poll_fds = realloc(poll_fds, sizeof(*poll_fds) * (*max_connected_sockets));
    }

    poll_fds[*connected_sockets_count].fd = new_client_fd;
    poll_fds[*connected_sockets_count].events = POLLIN;
    (*connected_sockets_count)++;
}

void DeleteClientFd(struct pollfd poll_fds[], int index, int *connected_sockets_count)
{
    if (*connected_sockets_count == 2)
    {
        poll_fds[index].fd = -1;
        poll_fds[index].revents = 0;
    }
    else
    {
        swap(&poll_fds[index], &poll_fds[*connected_sockets_count - 1]);
        poll_fds[*connected_sockets_count - 1].fd = -1;
        poll_fds[*connected_sockets_count - 1].revents = 0;
    }

    (*connected_sockets_count)--;
}
void CloseConnection(struct pollfd poll_fds[], int index, int *connected_sockets_count){
    int fd = poll_fds[index].fd;
    close(fd);
    DeleteClientFd(poll_fds, index, connected_sockets_count);
}

void CheckClientActivity(struct pollfd poll_fds[], int server_fd, int *connected_sockets_count, int *max_connected_sockets){
    for (int i = 1; i < *connected_sockets_count; ++i)
    {
        struct pollfd curr_poll_fd = poll_fds[i];
        if (curr_poll_fd.revents & POLLHUP)
        {
            printf("Hang-up event occurred closing socket!!\n");
            close(curr_poll_fd.fd);
            DeleteClientFd(poll_fds, i, connected_sockets_count);
            break;
        }
        if (curr_poll_fd.fd != -1 && poll_fds[i].revents & POLLIN)
        {
            // Initialize all fields to NULL, NULL indicates no error after  reading request
            httpResponse response;
            httpRequestLine request_line;
            httpRequestHeaders request_headers;

            ReadRequest(poll_fds, i, connected_sockets_count, &request_line, &request_headers, &response);
            WriteResponse(&request_line, &request_headers, & response);
            if (send(poll_fds[i].fd, response.message, response.message_length, 0) == -1)
            {
                printf("ERRNO: %d\n", response.message_length);
                perror("Send failed");
                CloseConnection(poll_fds, i, connected_sockets_count);
                break;
            }
            
            break;
        }
    }
}