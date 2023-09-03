#include "request.h"
#include "server.h"
#include "client.h"

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <poll.h>

#define BACKLOG 5
#define BUFFER_SIZE 1024

char *DIRECTORY = NULL;
char *ADDRESS = NULL;
char *LOG_FILE = NULL;
char *PORT = "9000";

int DecodeFlags(int argc, char **argv)
{
    int opt;

    opterr = 0;
    while ((opt = getopt(argc, argv, ":dhc:i:l:p:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            DIRECTORY = optarg;
            break;
        case 'i':
            ADDRESS = optarg;
            break;
        case 'l':
            LOG_FILE = optarg;
            break;
        case 'p':
            PORT = optarg;
            break;
        case 'd':
        case 'h':
        case '?':
            fprintf(stderr, "Unknown option: -%c\n", optopt);
            return 1;
        case ':':
            fprintf(stderr, "Option -%c requires an argument.\n", optopt);
            return 1;
        default:
            printf("==> %c\n", opt);
            break;
        }
    }
    return 0;
}
#define MAX_CLIENTS 10
int main(int argc, char **argv)
{
    DecodeFlags(argc, argv);

    int server_fd = InitServer(PORT);
    if (server_fd == -1)
    {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    int connected_sockets_count = 0;
    int max_connected_sockets = 10;
    struct pollfd *poll_fds = malloc(sizeof *poll_fds * (max_connected_sockets));
    for (int i = 0; i < MAX_CLIENTS; ++i)
    {
        poll_fds[i].fd = -1;
        poll_fds[i].events = POLLIN;
    }
    poll_fds[0].fd = server_fd;
    poll_fds[0].events = POLL_IN;

    connected_sockets_count = 1;
    for (;;)
    {
        int events_count = poll(poll_fds, connected_sockets_count, -1);

        if (events_count == -1)
        {
            perror("poll");
            exit(1);
        }
        CheckServerActivity(poll_fds, server_fd, &connected_sockets_count, &max_connected_sockets);
        CheckClientActivity(poll_fds, server_fd, &connected_sockets_count, &max_connected_sockets);
    }

    return 0;
}

