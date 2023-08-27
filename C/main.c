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

char *DIRECTORY = NULL;
char *ADDRESS = NULL;
char *LOG_FILE = NULL;
char *PORT = "9000";
int nextAvailableFd(int *client_fds, int maxClients){
    int i = 0;
    while (client_fds[i] != 0 && i < maxClients)
    {
        i++;
    }
    return i;
}
int InitServer() {
    int status;
    int server_fd;

    struct addrinfo  hints;
    struct addrinfo *server_info;
    struct addrinfo  *curr_addr_info;

    int yes = 1;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use IPV4 or IPV6    
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE; // use localhost

    if ((status = getaddrinfo(NULL, PORT, &hints, &server_info)) != 0)
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
    printf("Server Listening on %s 🚀\n\n", PORT);
    return server_fd;
}

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
    int current_client_fd;
    struct sockaddr_storage client_addr;
    socklen_t client_addr_len;
    char data_buffer[256];
    char client_ip[INET6_ADDRSTRLEN];

    int server_fd = InitServer();
    if (server_fd == -1)
    {
        fprintf(stderr, "error getting listening socket\n");
        exit(1);
    }

    int connected_sockets_count = 0;
    int max_connected_sockets = 5;
    struct pollfd *poll_fds = malloc(sizeof *poll_fds * max_connected_sockets);

    poll_fds[0].fd = server_fd;
    poll_fds[0].events = POLL_IN;

    connected_sockets_count = 1;


    return 0;
}

