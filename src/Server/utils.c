#include "utils.h"
#include "../globals.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

socket_fd accept_in_client(ServerSocket *self) {
	socklen_t in_addr_len = sizeof(self->in_addr);

	return accept(self->socket, (struct sockaddr *)&self->in_addr,
				  &in_addr_len);
}

ServerSocket *new_server_socket() {
	socket_fd server_socket;
	assert((server_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0);

	ServerSocket *res = malloc(sizeof(ServerSocket));

	res->socket = server_socket;
	res->in_addr.sin_family = AF_INET;
	res->in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	res->in_addr.sin_port = htons(SERVER_PORT);

	int opt = 1;

	assert(setsockopt(res->socket, SOL_SOCKET, SO_REUSEADDR, &opt,
					  sizeof(opt)) >= 0);

	assert(bind(res->socket, (struct sockaddr *)&res->in_addr,
				sizeof(res->in_addr)) >= 0);

	assert(listen(res->socket, 5) >= 0);

	int flags = fcntl(res->socket, F_GETFL, 0);
	fcntl(res->socket, F_SETFL, flags | O_NONBLOCK);

	res->accept_in_client = &accept_in_client;

	return res;
}
