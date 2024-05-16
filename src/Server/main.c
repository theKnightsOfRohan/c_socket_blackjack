#include "../globals.h"
#include "utils.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
	ServerSocket *server = new_server_socket();

	// ---------------------------------------------------------

	socklen_t in_addr_len = sizeof(server->in_addr);
	socket_fd new_socket;

	int count = 0;

	char buffer[1024] = {0};
	char *hello = "Hello from server";
	ssize_t valread;

	while (true) {
		if (count == 3)
			break;

		if ((new_socket = server->accept_in_client(server)) < 0) {
			assert(errno == EWOULDBLOCK);
			continue;
		}

		// subtract 1 for the null terminator at the end
		valread = read(new_socket, buffer, sizeof(buffer) - 1);

		printf("%s\n", buffer);
		send(new_socket, hello, strlen(hello), 0);
		printf("Hello message sent\n");

		// closing the connected socket
		close(new_socket);

		count++;
	}

	// closing the listening socket
	close(server->socket);

	free(server);

	return 0;
}
