#include "globals.h"
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void safe_exit() { exit(0); }

void log_open(char *log_file) {
	if (log_file == NULL) {
		log_file = "log.txt";
	}

	int res = snprintf(LOG_FILE, sizeof(LOG_FILE), "%s%s", "log/", log_file);
	assert(res > 0);

	assert(pthread_mutex_init(&LOG_LOCK, NULL) == 0);

	LOG_FILE_HANDLE = fopen(LOG_FILE, "a");
	eassert(LOG_FILE_HANDLE != NULL);

	Log("=======> LOG_INIT\n");

	atexit(log_close);

	signal(SIGINT, safe_exit);
	signal(SIGSEGV, safe_exit);
}

void log_close() {
	Log("LOG_END <=======\n");
	pthread_mutex_lock(&LOG_LOCK);

	fclose(LOG_FILE_HANDLE);
}

void *smalloc(size_t size) {
	void *ret = malloc(size);
	eassert(ret != NULL);
	return ret;
}
