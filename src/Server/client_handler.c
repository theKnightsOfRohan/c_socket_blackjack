#include "client_handler.h"
#include "../globals.h"
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void *handle_client(void *arg) {
	int self_desc = *(int *)arg;

	ssize_t readlen;

	char buffer[1024] = {0};
	int bufferlen = sizeof(buffer) - 1;

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

		printf("Thread %d received %s\n", self_desc, buffer);

		printf("Thread %d sending confirmation\n", self_desc);
		assert(send(GAME_STATE->client_sockets[self_desc], buffer, readlen, 0) > 0);

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
