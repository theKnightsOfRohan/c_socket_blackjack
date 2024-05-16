#include "utils.h"
#include <arpa/inet.h>
#include <assert.h>
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

	int status, readlen;

	char *hello = "Hello from client";
	char buffer[1024] = {0};

	status = send(client->socket, hello, strlen(hello), 0);
	printf("Hello message sent\n");

	// subtract 1 for the null terminator at the end
	readlen = read(client->socket, buffer, sizeof(buffer) - 1);

	printf("%s\n", buffer);

	// closing the connected socket
	close(client->socket);

	free(client);

	return 0;
}
