#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdarg.h>
#include <stdio.h>

#define SERVER_PORT 42069

// #define SERVER_IP "10.17.81.90"
#define SERVER_IP "127.0.0.1"

#define MAX_CLIENT_THREAD_COUNT 4

typedef int socket_fd;

// ODR DEEZ NUTZ
FILE *LOG_FILE_HANDLE;

// Should be the basename of the executable
void log_open(const char *log_file);

void log_close();

#define Log(fmt, ...)                                                                              \
	fprintf(LOG_FILE_HANDLE, "[%s.%s:%d] " fmt, __FILE_NAME__, __FUNCTION__, __LINE__,             \
			##__VA_ARGS__);                                                                        \
	printf("[%s.%s:%d] " fmt, __FILE_NAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);

#endif // GLOBALS_H
