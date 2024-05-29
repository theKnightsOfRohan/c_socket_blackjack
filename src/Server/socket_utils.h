#ifndef SERVER_SOCKET_UTILS_H
#define SERVER_SOCKET_UTILS_H

#include "../globals.h"
#include <netinet/in.h>
#include <sys/socket.h>

typedef struct ServerSocket {
	socket_fd socket;
	struct sockaddr_in in_addr;
} ServerSocket;

ServerSocket *new_server_socket();

void init_game_state();

void terminate_client_threads();

void add_client(socket_fd new_client);

#define AWAIT_VALUE(ptr, val)                                                                      \
	do {                                                                                           \
		while (*(ptr) != (val)) {                                                                  \
		}                                                                                          \
	} while (0)

#endif // SERVER_SOCKET_UTILS_H
