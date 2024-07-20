#include "client_handler.h"
#include "game_actions.h"
#include "game_request_handler.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *handle_response(GS_Result *res, int thread_id) {
	char *response;

	switch (res->err) {

	case REQ_SUCCESS:
		response = res->res;
		Log("client{%d}: Query was a success. Received \"%s\"\n", thread_id, res->res);
		break;

	case REQ_BAD_FORMAT:
		response = "Request format was incorrect\n";
		Log("client{%d}: %s", thread_id, response);
		break;

	case REQ_NOT_FOUND:
		response = "Request type was not found\n";
		Log("client{%d}: %s", thread_id, response);
		break;

	default:
		Log("client{%d}: UNREACHABLE. ERR: %d; RESPONSE: %s\n", thread_id, res->err, res->res);
		exit(EXIT_FAILURE);
	}

	return response;
}

void *handle_client(void *arg) {
	Player *self = (Player *)arg;
	self->idle = true;

	ssize_t readlen;

	char buffer[32] = {'\0'};
	ssize_t bufferlen = sizeof(buffer) - 1;
	GS_Result res;

	while (true) {
		readlen = read(self->socket, buffer, bufferlen);

		if (readlen < 0) {
			if (errno == EAGAIN)
				continue;
			else {
				Log("client{%d}: read error: %d\n", self->id, errno);
				break;
			}
		}

		self->idle = false;

		if (strncmp(buffer, "quit", 4) == 0) {
			Log("client{%d}: user quit\n", self->id);
			break;
		}

		Log("client{%d}: received %s.\n", self->id, buffer);

		res = execute_cmd(buffer, readlen, self);
		char *response = handle_response(&res, self->id);

		Log("client{%d}: sending result\n", self->id);
		assert(send(self->socket, response, strlen(response), 0) > 0);

		free(res.res);
		res.res = NULL;

		memset(buffer, '\0', sizeof(buffer));

		self->idle = true;

		if (GAME_STATE->terminate) {
			break;
		}
	}

	Log("client{%d}: terminating\n", self->id);

	assert(send(self->socket, "term", 4, 0) == 4);

	close(self->socket);

	Player *self_ref = GAME_STATE->curr->next;
	Player *prev = GAME_STATE->curr;

	while (self_ref != self) {
		self_ref = self_ref->next;
		prev = prev->next;
	}

	if (GAME_STATE->get_client_count() == 1) {
		GAME_STATE->curr = NULL;
		GAME_STATE->tail = NULL;
	}

	prev->next = self_ref->next;

	if (GAME_STATE->curr == self_ref) {
		GAME_STATE->next_turn();
	}

	free(self_ref);

	Log("client{%d}: thread terminated and removed from turn cycle\n", self->id);

	self->idle = true;

	pthread_exit(NULL);

	return NULL;
}
