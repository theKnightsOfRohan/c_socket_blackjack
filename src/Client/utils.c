#include "utils.h"
#include "../globals.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

void find_server(ClientSocket *self, const char *ip) {
	self->server_addr.sin_family = AF_INET;
	self->server_addr.sin_port = htons(SERVER_PORT);

	// Convert IPv4 and IPv6 addresses from text to binary form
	assert(inet_pton(AF_INET, ip, &self->server_addr.sin_addr) >= 0);
}

void connect_to_server(ClientSocket *self) {
	if (connect(self->socket, (struct sockaddr *)&self->server_addr,
				sizeof(self->server_addr)) < 0) {

		printf("error: %d\n", errno);
		exit(EXIT_FAILURE);
	}
}

ClientSocket *new_client_socket() {
	socket_fd client_socket;
	assert((client_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0);

	ClientSocket *res = malloc(sizeof(ClientSocket));

	res->socket = client_socket;

	res->find_server = &find_server;
	res->connect_to_server = &connect_to_server;

	return res;
}
