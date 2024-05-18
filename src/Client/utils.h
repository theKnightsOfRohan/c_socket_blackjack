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

typedef struct input_buffer {
	pthread_mutex_t lock;
	char buffer[1024];
} input_buffer;

ssize_t read_blocking(socket_fd fd, char *buffer, int bufsize);

#endif // CLIENT_UTILS_H
