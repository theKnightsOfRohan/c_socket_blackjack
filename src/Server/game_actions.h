#ifndef GAME_ACTIONS_H
#define GAME_ACTIONS_H

#include "../globals.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct Player {
	int id;
	pthread_t thread;
	socket_fd socket;
	struct Player *next;
	bool idle;
} Player;

typedef struct GameState {
	int client_id;
	pthread_mutex_t mod_lock;
	pthread_t *client_threads;
	socket_fd *client_sockets;
	Player *curr;
	// Tail is figurative, as list is circular for turns
	Player *tail;
	bool terminate;
	int (*get_client_count)(void);
	bool (*next_turn)(void);
	bool (*is_current_player)(Player *p);
} GameState;

// ODR DEEZ NUTZ
GameState *GAME_STATE;

void init_game_state(void);

#endif // GAME_ACTIONS_H
