#include "server.h"
#include "client.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

int InitServer(char *port) {
    int status;
    int server_fd;

    struct addrinfo hints;
    struct addrinfo *server_info;
    struct addrinfo *curr_addr_info;

    int yes = 1;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use IPV4 or IPV6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use localhost

    if ((status = getaddrinfo(NULL, port, &hints, &server_info)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    for (curr_addr_info = server_info; curr_addr_info != NULL; curr_addr_info = curr_addr_info->ai_next)
    {

        server_fd = socket(curr_addr_info->ai_family, curr_addr_info->ai_socktype, curr_addr_info->ai_protocol);

        if (server_fd == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(server_fd, curr_addr_info->ai_addr, curr_addr_info->ai_addrlen) == -1)
        {
            close(server_fd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (curr_addr_info == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(server_fd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    printf("Server Listening on %s 🚀\n\n", port);
    return server_fd;
}

int ServerHadActivity(struct pollfd poll_fds[])
{
    if (poll_fds[0].revents & POLLIN)
    {
        return 0;
    }

    return -1;
}

void *GetInAddr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void CheckServerActivity(struct pollfd poll_fds[], int server_fd, int *connected_sockets_count, int *max_connected_sockets)
{
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len;
    int new_client_fd;
    if (ServerHadActivity(poll_fds) == 0)
    {
        client_addr_len = sizeof client_addr;
        new_client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_client_fd == -1)
        {
            perror("accept");
        }
        AddClient(poll_fds, new_client_fd, connected_sockets_count, max_connected_sockets);
        printf("New connection accepted, socket fd is %d\n", new_client_fd);
    }
}