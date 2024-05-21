#include "../globals.h"
#include "client_handler.h"
#include "utils.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main() {
	ServerSocket *server = new_server_socket();

	GAME_STATE = malloc(sizeof(GameState));
	GAME_STATE->client_count = 0;
	GAME_STATE->client_threads = malloc(sizeof(pthread_t) * MAX_CLIENT_THREAD_COUNT);
	pthread_mutex_init(&GAME_STATE->mod_lock, NULL);
	GAME_STATE->client_sockets = malloc(sizeof(socket_fd) * MAX_CLIENT_THREAD_COUNT);

	// ---------------------------------------------------------

	socklen_t server_size = sizeof(server->in_addr);
	socket_fd new_client;

	while (true) {
		if (GAME_STATE->client_count > 3) {
			break;
		}

		new_client = accept(server->socket, (struct sockaddr *)&server->in_addr, &server_size);

		if (new_client < 0) {
			printf("Client accept error: %d\n", errno);
			exit(EXIT_FAILURE);
		}

		int idx = GAME_STATE->client_count;

		pthread_mutex_lock(&GAME_STATE->mod_lock);

		GAME_STATE->client_sockets[idx] = new_client;
		pthread_create(&GAME_STATE->client_threads[idx], NULL, handle_client, &idx);

		GAME_STATE->client_count++;

		pthread_mutex_unlock(&GAME_STATE->mod_lock);
	}

	// Terminate all client handler threads
	for (int i = 0; i < GAME_STATE->client_count; i++) {
		pthread_cancel(GAME_STATE->client_threads[i]);
		send(GAME_STATE->client_sockets[i], "term", 4, 0);
		close(GAME_STATE->client_sockets[i]);
	}

	printf("Terminating server\n");

	// closing the listening socket
	close(server->socket);

	free(server);
	free(GAME_STATE);

	return 0;
}
