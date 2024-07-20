#include "../globals.h"
#include "game_actions.h"
#include "socket_utils.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	if (argv[1] == NULL) {
		printf("Usage: %s <log_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	log_open(argv[1]);

	Log("Starting server\n");
	ServerSocket *server = new_server_socket();

	Log("Binding server\n");
	init_game_state();

	// ---------------------------------------------------------

	socklen_t server_socklen = sizeof(server->in_addr);
	socket_fd new_client;

	while (true) {
		if (GAME_STATE->terminate) {
			Log("Terminating server\n");
			break;
		}

		new_client = accept(server->socket, (struct sockaddr *)&server->in_addr, &server_socklen);

		if (new_client < 0) {
			if (errno == EAGAIN)
				continue;
			else {
				Log("Client accept error: %d\n", errno);
				break;
			}
		}

		Log("Client connected at socket %d\n", new_client);
		add_client(new_client);
	}

	Log("Terminating client threads\n");
	terminate_client_threads();

	Log("Closing server\n");

	// closing the listening socket
	close(server->socket);

	free(server);
	free(GAME_STATE);

	return 0;
}
