#ifndef GAME_HANDLER_H
#define GAME_HANDLER_H

#include "../globals.h"
#include <pthread.h>

typedef struct GameState {
	int client_count;
	pthread_mutex_t mod_lock;
	pthread_t *client_threads;
	socket_fd *client_sockets;
} GameState;

// ODR DEEZ NUTZ
GameState *GAME_STATE;

typedef enum { REQ_SUCCESS, REQ_NOT_FOUND, REQ_BAD_FORMAT } GS_Error;

typedef struct GS_Result {
	char *res;
	GS_Error err;
} GS_Result;

GS_Result execute_cmd(char *cmd, int length);

#endif // GAME_HANDLER_H
