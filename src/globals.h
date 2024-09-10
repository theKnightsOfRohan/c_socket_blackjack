#ifndef GLOBALS_H
#define GLOBALS_H

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ODR DEEZ NUTZ
FILE *LOG_FILE_HANDLE;

// ODR DEEZ NUTZ
char LOG_FILE[256];

void log_open(char *log_file);

void log_close();

void *smalloc(size_t size);

#define noop() (void)0

// ODR DEEZ NUTZ
pthread_mutex_t LOG_LOCK;

#define assert_succ(fun) eassert(fun != -1)

#define eassert(e)                                                             \
	if (!(e)) {                                                                \
		__eassert(#e);                                                         \
	}

#define __eassert(e)                                                           \
	Log("Failed assertion '%s' with errno %s\n", e, strerror(errno));          \
	exit(EXIT_FAILURE);

#define Log(fmt, ...)                                                          \
	{                                                                          \
		pthread_mutex_lock(&LOG_LOCK);                                         \
		fprintf(LOG_FILE_HANDLE, "[%s.%s:%d] " fmt, __FILE_NAME__,             \
				__FUNCTION__, __LINE__, ##__VA_ARGS__);                        \
		printf("[%s.%s:%d] " fmt, __FILE_NAME__, __FUNCTION__, __LINE__,       \
			   ##__VA_ARGS__);                                                 \
		fflush(LOG_FILE_HANDLE);                                               \
		pthread_mutex_unlock(&LOG_LOCK);                                       \
	}

#define smodify(block, mutex_ptr)                                              \
	{                                                                          \
		pthread_mutex_lock(mutex_ptr);                                         \
		block;                                                                 \
		pthread_mutex_unlock(mutex_ptr);                                       \
	}

#endif // GLOBALS_H
