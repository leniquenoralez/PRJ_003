#include <poll.h>
void AddClient(struct pollfd poll_fds[], int new_client_fd, int *connected_sockets_count, int *max_connected_sockets);
void DeleteClientFd(struct pollfd poll_fds[], int index, int *connected_sockets_count);
void CheckClientActivity(struct pollfd poll_fds[], int server_fd, int *connected_sockets_count, int *max_connected_sockets);