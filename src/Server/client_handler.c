#include "client_handler.h"
#include "game_handler.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *handle_response(GS_Result *res) {
	char *response;

	switch (res->err) {

	case REQ_SUCCESS:
		response = res->res;
		printf("Query was a success\n");
		break;

	case REQ_BAD_FORMAT:
		response = "Request format was incorrect\n";
		printf("%s", response);
		break;

	case REQ_NOT_FOUND:
		response = "Request type was not found\n";
		printf("%s", response);
		break;

	default:
		perror("UNREACHABLE\n");
		printf("ERR: %d; RESPONSE: %s\n", res->err, res->res);
		exit(EXIT_FAILURE);
	}

	return response;
}

void *handle_client(void *arg) {
	int self_desc = *(int *)arg;

	ssize_t readlen;

	char buffer[32] = {0};
	ssize_t bufferlen = sizeof(buffer) - 1;
	GS_Result res;

	int count = 0;

	while (true) {
		readlen = read(GAME_STATE->client_sockets[self_desc], buffer, bufferlen);
		if (readlen < 0) {
			printf("Thread %d read error: %d\n", self_desc, errno);
			break;
		}

		if (strncmp(buffer, "quit", 4) == 0) {
			printf("Thread %d user quit\n", self_desc);
			break;
		}

		printf("Thread %d received %s.\n", self_desc, buffer);

		res = execute_cmd(buffer, readlen);
		char *response = handle_response(&res);

		printf("Thread %d sending result\n", self_desc);
		assert(send(GAME_STATE->client_sockets[self_desc], response, strlen(response), 0) > 0);

		free(res.res);
		res.res = NULL;

		count++;
		memset(buffer, '\0', sizeof(buffer));

		if (count == 5) {
			break;
		}
	}

	printf("Thread %d terminating\n", self_desc);

	assert(send(GAME_STATE->client_sockets[self_desc], "term", 4, 0) > 0);

	return NULL;
}
