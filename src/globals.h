#ifndef GLOBALS_H
#define GLOBALS_H

#define SERVER_PORT 42069

#define MAX_CLIENT_THREAD_COUNT 4

typedef int socket_fd;

#include <netinet/in.h>
#include <pthread.h>

typedef struct GameState {
	int client_count;
	pthread_mutex_t mod_lock;
	pthread_t *client_threads;
	socket_fd *client_sockets;
} GameState;

// ODR DEEZ NUTZ
GameState *GAME_STATE;

#endif // GLOBALS_H
