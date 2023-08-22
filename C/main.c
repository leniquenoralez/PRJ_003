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
int PORT = 9000;
int nextAvailableFd(int *client_fds, int maxClients){
    int i = 0;
    while (client_fds[i] != 0 && i < maxClients)
    {
        i++;
    }
    return i;
} 
int initServer(int port)
{
    int serverSock;
    socklen_t length;
    struct sockaddr_in server;

    memset(&server, 0, sizeof(server));

    if ((serverSock = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("opening stream socket");
        exit(EXIT_FAILURE);
    }
    server.sin_family = PF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(serverSock, (struct sockaddr *)&server, sizeof(server)) != 0)
    {
        perror("binding stream socket");
        exit(EXIT_FAILURE);
    }

    length = sizeof(server);
    if (getsockname(serverSock, (struct sockaddr *)&server, &length) != 0)
    {
        perror("getting socket name");
        exit(EXIT_FAILURE);
    }
    if (listen(serverSock, BACKLOG) == -1)
    {
        perror("listen");
        close(serverSock);
        exit(1);
    }

    char ip_address[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(server.sin_addr), ip_address, INET_ADDRSTRLEN);

    printf("Server listening on %s:%d...\n", ip_address, ntohs(server.sin_port));
    return serverSock;
}
void initClientFds(int *client_fds, int maxClients)
{
    for (int i = 0; i < maxClients; ++i)
    {
        client_fds[i] = 0;
    }
}
fd_set initReadFds(int server_fd)
{
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    return read_fds;
}
void addClientReadFds(int *client_fds, fd_set read_fds, int *max_fd, int maxClients)
{
    for (int i = 0; i < maxClients; ++i)
    {
        int fd = client_fds[i];
        if (fd > 0)
        {
            FD_SET(fd, &read_fds);
            if (fd > *max_fd)
            {
                *max_fd = fd;
            }
        }
    }
}
char *getClientAddress(struct sockaddr_in client_addr)
{
    char client_ip_address[INET_ADDRSTRLEN];
    const char *rip = inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip_address, INET_ADDRSTRLEN);
    if (rip == NULL)
    {
        return "unknown";
    }

    return client_ip_address;
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
#define MAX_CLIENTS 10
int main(int argc, char **argv)
{
    decodeFlags(argc, argv);

    int server_fd = initServer(PORT);
    int client_fds[MAX_CLIENTS];
    int max_fd;
    int activity;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[1024] = {0};

    initClientFds(client_fds, MAX_CLIENTS);

    while (1)
    {
        fd_set read_fds = initReadFds(server_fd);
        max_fd = server_fd;

        addClientReadFds(client_fds, read_fds, &max_fd, MAX_CLIENTS);
        activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity == -1)
        {
            perror("select");
        }

        if (FD_ISSET(server_fd, &read_fds))
        {
            int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (new_socket == -1)
            {
                perror("accept");
            }
            else
            {
                printf("New connection accepted, socket fd is %d\n", new_socket);
                int nextFd = nextAvailableFd(client_fds, MAX_CLIENTS);
                client_fds[nextFd] = new_socket;
            }
        }

        for (int i = 0; i < MAX_CLIENTS; ++i)
        {
            int fd = client_fds[i];
            if (FD_ISSET(fd, &read_fds))
            {
                ssize_t read_size = recv(fd, buffer, sizeof(buffer), 0);
                if (read_size <= 0)
                {
                    // Connection closed or error
                    printf("Client disconnected, socket fd is %d\n", fd);
                    close(fd);
                    client_fds[i] = 0;
                }
                else
                {
                    // Handle received data, for example, send a response
                    char *client_ip_address = getClientAddress(client_addr);
                    
                    (void)printf("Client (%s) sent: \"%s\"", client_ip_address, buffer);

                    send(fd, buffer, read_size, 0);
                }
            }
        }
    }
    

    // TODO: Speaks only HTTP/1.0
    return 0;
}

