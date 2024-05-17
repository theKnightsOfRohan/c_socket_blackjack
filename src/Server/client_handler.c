#include "client_handler.h"
#include "../globals.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void *handle_client(void *arg) {
	int self_desc = *(int *)arg;

	ssize_t readlen;

	char buffer[1024] = {0};
	int bufferlen = sizeof(buffer) - 1;

	clock_t start = clock(), curr;

	int count = 0;

	while (true) {
		curr = clock();

		if (((double)curr - start) / CLOCKS_PER_SEC > 5) {
			count++;

			start = curr;

			printf("Thread %d ping #%d\n", self_desc, count);
			assert(send(GAME_STATE->client_sockets[self_desc], "ping", 4, 0) > 0);

			readlen = read(GAME_STATE->client_sockets[self_desc], buffer, bufferlen);

			assert(readlen >= 4);

			printf("Thread %d received %s, should be pong\n", self_desc, buffer);

			if (count == 5)
				break;
		}
	}

	printf("Thread %d terminating\n", self_desc);

	assert(send(GAME_STATE->client_sockets[self_desc], "term", 4, 0) > 0);

	return NULL;
}
