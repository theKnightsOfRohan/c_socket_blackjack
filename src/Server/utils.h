#ifndef SERVER_UTILS_H
#define SERVER_UTILS_H

#include "../globals.h"
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct ServerSocket {
	socket_fd socket;
	struct sockaddr_in in_addr;
	socket_fd (*accept_in_client)(struct ServerSocket *self);
	ssize_t (*read_from_client)(socket_fd client_socket, char *buffer);
	void (*send_to_client)(socket_fd client_socket, const char *message,
						   int flags);
} ServerSocket;

ServerSocket *new_server_socket();

#endif // SERVER_UTILS_H
