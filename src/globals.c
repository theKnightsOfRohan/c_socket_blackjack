#include "globals.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

void safe_exit() { exit(0); }

void log_open(const char *side) {
	char *log_file = malloc(sizeof(char) * 32);
	sprintf(log_file, "log/%s.log", side);
	printf("log_file: %s\n", log_file);

	LOG_FILE_HANDLE = fopen(log_file, "w");
	assert(LOG_FILE_HANDLE != NULL);

	Log("=======> LOG_INIT\n");

	atexit(log_close);
	signal(SIGINT, safe_exit);
}

void log_close() {
	Log("LOG_END <=======\n");

	fclose(LOG_FILE_HANDLE);
}
