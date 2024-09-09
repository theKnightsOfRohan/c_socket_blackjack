#include "../globals.h"
#include "server_socket.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	log_open(argv[1]);

	Server *server = new_server_socket();

	char buffer[1024];
	memset(&buffer, '\0', sizeof(buffer));

	printf("Server awaiting ping...\n");

	socket_fd new_client = accept(
		server->fd, (struct sockaddr *)&server->address, &server->addr_len);

	ssize_t readlen = read(server->fd, &buffer, sizeof(buffer));

	printf("Readlen: %zd\n", readlen);

	printf("Errno: %d\n", errno);

	close_socket(server->fd);

	return EXIT_SUCCESS;
}
