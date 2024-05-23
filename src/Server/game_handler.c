#include "game_handler.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

typedef enum { GET_CLIENT_COUNT } GS_Request_Type;

GS_Result execute_req(GS_Request_Type req, char *arg, int arglen) {
	GS_Result res;
	switch (req) {

	case GET_CLIENT_COUNT:
		if (arg[0] != '\0') {
			res.res = NULL;
			res.err = REQ_BAD_FORMAT;
			return res;
		}

		res.res = malloc(sizeof(char) * 64);
		sprintf(res.res, "Number of clients: %d\n", GAME_STATE->client_count);
		res.err = REQ_SUCCESS;
		return res;

	default:
		perror("Unreachable\n");
		exit(EXIT_FAILURE);
	}
}

GS_Result execute_cmd(char *cmd, int length) {
	printf("Received raw cmd: %s.\n", cmd);

	if (cmd == NULL || length < 3)
		return (GS_Result){.res = NULL, .err = REQ_BAD_FORMAT};

	if (strncmp(cmd, "get", 3) == 0)
		return execute_req(GET_CLIENT_COUNT, &cmd[3], length - 3);

	return (GS_Result){.res = NULL, .err = REQ_NOT_FOUND};
}
