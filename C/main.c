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
int initServer(struct addrinfo hints, struct addrinfo *servinfo, char *port)
{
    int status;
    int sockfd;
    struct addrinfo  *p;
    int yes = 1;

    hints.ai_family = AF_UNSPEC;     
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;     

    if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }
    return sockfd;
}

int decodeFlags(int argc, char **argv)
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
    decodeFlags(argc, argv);
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof hints);
    memset(&servinfo, 0, sizeof servinfo);
    int server_fd = initServer(hints, servinfo, PORT);
    printf("Sever socket descriptor is %d", server_fd);
    // TODO:
    // TODO: Speaks only HTTP/1.0
    return 0;
}

