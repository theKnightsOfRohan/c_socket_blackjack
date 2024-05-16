#ifndef CLIENT_UTILS_H

#define CLIENT_UTILS_H

#include "../globals.h"
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct ClientSocket {
	socket_fd socket;
	struct sockaddr_in server_addr;
	void (*find_server)(struct ClientSocket *self, const char *ip);
	void (*connect_to_server)(struct ClientSocket *self);
} ClientSocket;

ClientSocket *new_client_socket();

#endif // CLIENT_UTILS_H
