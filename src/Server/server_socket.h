#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include <netinet/in.h>
#include <sys/socket.h>

typedef int socket_fd;

#define SERVER_PORT 42069

typedef struct {
	socket_fd fd;
	socklen_t fd_len;
	struct sockaddr_in address;
	socklen_t addr_len;
} Server;

Server *new_server_socket();

socket_fd accept_client(socket_fd server_socket);

void close_socket(socket_fd socket);

#endif // SERVER_SOCKET_H
