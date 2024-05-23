#include "../globals.h"
#include "game_handler.h"
#include "socket_utils.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
	ServerSocket *server = new_server_socket();

	init_game_state();

	// ---------------------------------------------------------

	socklen_t server_socklen = sizeof(server->in_addr);
	socket_fd new_client;

	while (true) {
		if (GAME_STATE->client_count > 3) {
			break;
		}

		new_client = accept(server->socket, (struct sockaddr *)&server->in_addr, &server_socklen);

		if (new_client < 0) {
			printf("Client accept error: %d\n", errno);
			break;
		}

		add_client(new_client);
	}

	terminate_client_threads();

	printf("Terminating server\n");

	// closing the listening socket
	close(server->socket);

	free(server);
	free(GAME_STATE);

	return 0;
}
