#include "input_handler.h"
#include "utils.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
	ClientSocket *client = new_client_socket();

	client->find_server(client, "127.0.0.1");

	client->connect_to_server(client);

	// ---------------------------------------------------------

	int status, readlen, count = 0;

	char socket_buffer[1024] = {0};
	int socket_buflen = sizeof(socket_buffer) - 1;

	input_buffer input;

	pthread_t input_thread;

	assert(pthread_mutex_init(&input.lock, NULL) == 0);
	pthread_mutex_lock(&input.lock);

	assert(pthread_create(&input_thread, NULL, handle_input, &input) == 0);

	while (true) {
		if ((status = pthread_mutex_trylock(&input.lock)) == 0) {
			printf("Main thread received user input %s. Sending...\n", input.buffer);

			if (send(client->socket, input.buffer, strlen(input.buffer), 0) < 0) {
				printf("Main thread send error: %d\n", errno);
				break;
			}

			if (strncmp(input.buffer, "quit", 4) == 0) {
				printf("User quitting...");
				break;
			}

			input.buffer[0] = '\0';
			printf("Awaiting response...\n");

			readlen = read_blocking(client->socket, socket_buffer, socket_buflen);

			printf("Received confirmation message %s from server\n", socket_buffer);

			pthread_mutex_unlock(&input.lock);
			sleep(1);
			socket_buffer[0] = '\0';
		} else {
			assert(status == EBUSY);

			if ((readlen = read(client->socket, socket_buffer, socket_buflen)) < 0) {
				assert(errno == EAGAIN);
				continue;
			}

			if (strncmp(socket_buffer, "term", 4) == 0) {
				printf("Received termination request from server...\n");
				break;
			}
		}

		/*
				readlen = read(client->socket, buffer, 1023);

				printf("Received: %s.\n", buffer);

				if (strncmp(buffer, "ping", 4) == 0) {
					count++;
					printf("Received ping %d, sending pong\n", count);
					send(client->socket, pong, sizeof(pong), 0);
				} else if (strncmp(buffer, "term", 4) == 0) {
					printf("Terminating client\n");
					break;
				} else {
					printf("Received unknown message: %s\n", buffer);
				}
		*/
	}

	pthread_cancel(input_thread);
	pthread_mutex_destroy(&input.lock);

	// closing the connected socket
	close(client->socket);

	free(client);

	return 0;
}
