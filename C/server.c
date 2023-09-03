#include "server.h"

#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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