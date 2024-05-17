#include "utils.h"
#include <arpa/inet.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
	ClientSocket *client = new_client_socket();

	client->find_server(client, "127.0.0.1");

	client->connect_to_server(client);

	// ---------------------------------------------------------

	int status, readlen, count = 0;

	char pong[] = "pong";

	char buffer[1024] = {0};

	while (true) {
		readlen = read(client->socket, buffer, 1023);

		printf("Received: %s.\n", buffer);

		if (strncmp(buffer, "ping", 4) == 0) {
			count++;
			printf("Received ping %d, sending pong\n", count);
			send(client->socket, pong, sizeof(pong), 0);
		} else if (strncmp(buffer, "term", 4) == 0) {
			printf("Terminating client");
			break;
		}
	}

	// closing the connected socket
	close(client->socket);

	free(client);

	return 0;
}
