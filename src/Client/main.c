#include "input_handler.h"
#include "socket_utils.h"
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

int main(int argc, char *argv[]) {
	if (argv[1] == NULL) {
		printf("Usage: %s <log_file>\n", argv[0]);
		return EXIT_FAILURE;
	}

	log_open(argv[1]);

	Log("Starting client\n");

	ClientSocket *client = new_client_socket();

	Log("Finding server at %s\n", SERVER_IP);
	client->find_server(client, SERVER_IP);

	Log("Connecting to server at %s\n", SERVER_IP);
	client->connect_to_server(client);

	// ---------------------------------------------------------

	int status, readlen, count = 0;

	char socket_buffer[1024] = {0};
	int socket_buflen = sizeof(socket_buffer) - 1;

	input_buffer input;

	pthread_t input_thread;

	assert(pthread_mutex_init(&input.lock, NULL) == 0);
	pthread_mutex_lock(&input.lock);

	assert(pthread_mutex_init(&input.setup_lock, NULL) == 0);
	pthread_mutex_lock(&input.setup_lock);

	assert(pthread_create(&input_thread, NULL, handle_input, &input) == 0);

	while (true) {
		if ((status = pthread_mutex_trylock(&input.lock)) == 0) {
			printf("Main thread received user input %s. Sending...\n", input.buffer);

			if (send(client->socket, input.buffer, strlen(input.buffer), 0) < 0) {
				printf("Main thread send error: %d\n", errno);
				break;
			}

			if (strncmp(input.buffer, "quit", 4) == 0) {
				printf("User quitting...\n");
			}

			memset(input.buffer, '\0', sizeof(input.buffer));

			printf("Awaiting response...\n");

			readlen = read_blocking(client->socket, socket_buffer, socket_buflen);

			printf("Received message from server:\n%s\n", socket_buffer);

			if (strncmp(socket_buffer, "term", 4) == 0) {
				printf("Received termination request from server...\n");
				break;
			}

			memset(socket_buffer, '\0', sizeof(socket_buffer));

			pthread_mutex_unlock(&input.lock);
			pthread_mutex_lock(&input.setup_lock);
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
	}

	pthread_cancel(input_thread);
	pthread_mutex_destroy(&input.lock);

	// closing the connected socket
	close(client->socket);

	free(client);

	return 0;
}
