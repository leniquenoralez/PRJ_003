#include <poll.h>

#define BACKLOG 5
int InitServer(char *PORT);
void CheckServerActivity(struct pollfd poll_fds[], int server_fd, int *connected_sockets_count, int *max_connected_sockets);