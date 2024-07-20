#include "game_actions.h"
#include "../globals.h"
#include <stdbool.h>
#include <stdlib.h>

bool next_turn() {
	Log("Progressing turn from %d to %d\n", GAME_STATE->curr->id, GAME_STATE->curr->next->id);

	Player *curr = GAME_STATE->curr;
	GAME_STATE->curr = GAME_STATE->curr->next;
	return curr != GAME_STATE->curr;
}

bool is_current_player(Player *p) { return p == GAME_STATE->curr; }

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
	GAME_STATE->get_client_count = &get_client_count;
	GAME_STATE->client_threads = malloc(sizeof(pthread_t) * MAX_CLIENT_THREAD_COUNT);
	pthread_mutex_init(&GAME_STATE->mod_lock, NULL);
	GAME_STATE->client_sockets = malloc(sizeof(socket_fd) * MAX_CLIENT_THREAD_COUNT);
	GAME_STATE->curr = NULL;
	GAME_STATE->tail = NULL;
	GAME_STATE->terminate = false;
}
