#include "socket_utils.h"
#include "../globals.h"
#include "client_handler.h"
#include "game_handler.h"
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
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

	int flags = fcntl(res->socket, F_GETFL, 0);
	fcntl(res->socket, F_SETFL, flags | O_NONBLOCK);

	return res;
}

void add_client(socket_fd new_client) {
	int id = GAME_STATE->client_id;

	Log("[main.add_client] Adding client %d. Locking.\n", id);

	pthread_mutex_lock(&GAME_STATE->mod_lock);

	Player *new = malloc(sizeof(Player));
	new->socket = new_client;
	new->id = id;

	if (GAME_STATE->get_client_count() == 0) {
		Log("[main.add_client] First client. Setting as current turn.\n");
		GAME_STATE->curr = new;
		GAME_STATE->tail = new;
		GAME_STATE->tail->next = new;
	} else {
		Log("[main.add_client] Adding client %d to the end of the turn queue.\n", id);
		new->next = GAME_STATE->tail->next;
		GAME_STATE->tail->next = new;
		GAME_STATE->tail = new;
	}

	pthread_create(&new->thread, NULL, handle_client, new);

	GAME_STATE->client_id++;

	pthread_mutex_unlock(&GAME_STATE->mod_lock);

	Log("[main.add_client] Client %d added. Unlocking.\n", id);
}

void terminate_client(Player *player) {
	Log("[client{%d}] terminating\n", player->id);

	assert(send(player->socket, "term", 4, 0) == 4);

	close(player->socket);

	Player *player_ref = GAME_STATE->curr->next;
	Player *prev = GAME_STATE->curr;

	while (player_ref != player) {
		player_ref = player_ref->next;
		prev = prev->next;
	}

	if (GAME_STATE->get_client_count() == 1) {
		GAME_STATE->curr = NULL;
		GAME_STATE->tail = NULL;
	}

	prev->next = player_ref->next;

	if (GAME_STATE->curr == player_ref) {
		GAME_STATE->curr = GAME_STATE->curr->next;
	}

	pthread_join(player->thread, NULL);

	free(player_ref);

	Log("[client{%d}] thread terminated and removed from turn cycle\n", player->id);
}

void terminate_client_threads() {
	Log("[main.terminate_client_threads] Terminating %d client handler threads. Locking.\n",
		GAME_STATE->get_client_count());

	pthread_mutex_lock(&GAME_STATE->mod_lock);

	Player *curr = GAME_STATE->curr;
	Player *prev = NULL;

	do {
		prev = curr;
		curr = curr->next;

		Log("[main.terminate_client_threads] Terminating client. Awaiting idle.\n");
		AWAIT_VALUE(&prev->idle, true);

		Log("[main.terminate_client_threads] Client is idle. Terminating thread.\n");
		pthread_join(prev->thread, NULL);

		Log("[main.terminate_client_threads] Client %d thread terminated and removed from turn "
			"cycle.\n",
			prev->id);
	} while (curr != GAME_STATE->curr);

	GAME_STATE->curr = NULL;

	pthread_mutex_unlock(&GAME_STATE->mod_lock);

	Log("[main.terminate_client_threads] All client handler threads terminated. Unlocking.\n");
}

int get_client_count() {
	if (GAME_STATE->curr == NULL)
		return 0;

	Player *curr = GAME_STATE->curr;
	int size;

	for (size = 1; curr->next != GAME_STATE->curr; curr = curr->next) {
		size++;
	}

	return size;
}

void init_game_state() {
	GAME_STATE = malloc(sizeof(GameState));
	GAME_STATE->client_count = 0;
	GAME_STATE->get_client_count = &get_client_count;
	GAME_STATE->client_threads = malloc(sizeof(pthread_t) * MAX_CLIENT_THREAD_COUNT);
	pthread_mutex_init(&GAME_STATE->mod_lock, NULL);
	GAME_STATE->client_sockets = malloc(sizeof(socket_fd) * MAX_CLIENT_THREAD_COUNT);
	GAME_STATE->curr = NULL;
	GAME_STATE->tail = NULL;
	GAME_STATE->terminate = false;
}
