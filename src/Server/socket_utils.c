#include "socket_utils.h"
#include "../globals.h"
#include "client_handler.h"
#include "game_handler.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

ServerSocket *new_server_socket() {
	socket_fd server_socket;
	assert((server_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0);

	ServerSocket *res = malloc(sizeof(ServerSocket));

	res->socket = server_socket;
	res->in_addr.sin_family = AF_INET;
	res->in_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	res->in_addr.sin_port = htons(SERVER_PORT);

	int opt = 1;

	assert(setsockopt(res->socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) >= 0);

	assert(bind(res->socket, (struct sockaddr *)&res->in_addr, sizeof(res->in_addr)) >= 0);

	assert(listen(res->socket, 5) >= 0);

	return res;
}

void add_client(socket_fd new_client) {
	int idx = GAME_STATE->client_count;

	printf("Accepted client %d\n", idx);

	pthread_mutex_lock(&GAME_STATE->mod_lock);

	GAME_STATE->client_sockets[idx] = new_client;
	pthread_create(&GAME_STATE->client_threads[idx], NULL, handle_client, &idx);

	GAME_STATE->client_count++;

	pthread_mutex_unlock(&GAME_STATE->mod_lock);
}

void init_game_state() {
	GAME_STATE = malloc(sizeof(GameState));
	GAME_STATE->client_count = 0;
	GAME_STATE->client_threads = malloc(sizeof(pthread_t) * MAX_CLIENT_THREAD_COUNT);
	pthread_mutex_init(&GAME_STATE->mod_lock, NULL);
	GAME_STATE->client_sockets = malloc(sizeof(socket_fd) * MAX_CLIENT_THREAD_COUNT);
}

void terminate_client_threads() {
	printf("Terminating %d client handler threads...", GAME_STATE->client_count);

	// Terminate all client handler threads
	for (int i = 0; i < GAME_STATE->client_count; i++) {
		pthread_cancel(GAME_STATE->client_threads[i]);
		send(GAME_STATE->client_sockets[i], "term", 4, 0);
		close(GAME_STATE->client_sockets[i]);
	}
}
