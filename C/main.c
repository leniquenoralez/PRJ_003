#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 5

char *DIRECTORY = NULL;
char *ADDRESS = NULL;
char *LOG_FILE = NULL;
int PORT = 8080;


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
            PORT = atoi(optarg);
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
int main(int argc, char **argv)
{
    decodeFlags(argc, argv);

    int sock;
    socklen_t length;
    struct sockaddr_in server;

    memset(&server, 0, sizeof(server));

    if ((sock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("opening stream socket");
        exit(EXIT_FAILURE);
    }
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) != 0)
    {
        perror("binding stream socket");
        exit(EXIT_FAILURE);
    }

    length = sizeof(server);
    if (getsockname(sock, (struct sockaddr *)&server, &length) != 0)
    {
        perror("getting socket name");
        exit(EXIT_FAILURE);
    }

    char ip_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server.sin_addr), ip_address, INET_ADDRSTRLEN);

    printf("Socket has port %d\n", server.sin_port);
    printf("Socket has address %s\n", ip_address);

    close(sock);

    if (LOG_FILE != NULL)
    {
        printf("LOG_FILE: %s\n", LOG_FILE);
    }
    return 0;
}

