#ifndef GAME_REQUEST_HANDLER_H
#define GAME_REQUEST_HANDLER_H

#include "../globals.h"
#include "game_actions.h"
#include <pthread.h>
#include <stdbool.h>

typedef enum { REQ_SUCCESS, REQ_NOT_FOUND, REQ_BAD_FORMAT } GS_Error;

typedef struct GS_Result {
	char *res;
	GS_Error err;
} GS_Result;

GS_Result execute_cmd(char *cmd, int length, Player *player);

#endif // GAME_REQUEST_HANDLER_H
