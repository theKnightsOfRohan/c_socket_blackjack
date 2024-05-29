#include "game_handler.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

GS_Result execute_req(char *req, char *arg, int arglen) {
	Log("[execute_req] Request type: %s Arg: %s\n", req, arg);

	GS_Result res;

	if (strcmp(req, "get") == 0) {
		if (arg[0] != '\0') {
			res.res = NULL;
			res.err = REQ_BAD_FORMAT;
			return res;
		}

		Log("[execute_req] Getting client count\n");

		res.res = malloc(sizeof(char) * 64);
		sprintf(res.res, "Number of clients: %d", GAME_STATE->get_client_count());
		res.err = REQ_SUCCESS;
		return res;

	} else if (strcmp(req, "curr") == 0) {
		if (arg[0] != '\0') {
			res.res = NULL;
			res.err = REQ_BAD_FORMAT;
			return res;
		}

		Log("[execute_req] Getting current player\n");

		res.res = malloc(sizeof(char) * 64);
		sprintf(res.res, "Current turn player: %d", GAME_STATE->curr->id);
		res.err = REQ_SUCCESS;
		return res;

	} else if (strcmp(req, "TERM") == 0) {
		if (arg[0] != '\0') {
			res.res = NULL;
			res.err = REQ_BAD_FORMAT;
			return res;
		}

		Log("[execute_req] Terminating server\n");

		assert(pthread_mutex_lock(&GAME_STATE->mod_lock) == 0);

		GAME_STATE->terminate = true;

		res.res = malloc(sizeof(char) * 64);

		sprintf(res.res, "Terminating server");
		res.err = REQ_SUCCESS;

		pthread_mutex_unlock(&GAME_STATE->mod_lock);

		return res;

	} else {
		Log("[execute_req] UNREACHABLE\n");

		exit(EXIT_FAILURE);
	}
}

GS_Result execute_cmd(char *cmd, int length) {
	Log("[execute_req] Received raw cmd: %s.\n", cmd);

	if (cmd == NULL || length < 3)
		return (GS_Result){.res = NULL, .err = REQ_BAD_FORMAT};

	if (strncmp(cmd, "get", 3) == 0)
		return execute_req("get", &cmd[3], length - 3);
	else if (strncmp(cmd, "curr", 4) == 0)
		return execute_req("curr", &cmd[4], length - 4);
	else if (strncmp(cmd, "TERM", 4) == 0)
		return execute_req("TERM", &cmd[4], length - 4);
	else
		return (GS_Result){.res = NULL, .err = REQ_NOT_FOUND};
}
