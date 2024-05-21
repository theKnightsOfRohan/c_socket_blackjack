#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <pthread.h>

// (input_buffer *) -> void
void *handle_input(void *arg);

typedef struct input_buffer {
	pthread_mutex_t lock;
	pthread_mutex_t setup_lock;
	char buffer[1024];
} input_buffer;

#endif // INPUT_HANDLER_H
