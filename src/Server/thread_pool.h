#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>

typedef void *(*thread_func)(void *arg);

struct tpool_work {
	thread_func func;
	void *arg;
	struct tpool_work *next;
};

struct tpool_worker {
	struct tpool_work *work;
	pthread_t thread;
	volatile bool is_working;
};

typedef struct ThreadPool {
	struct tpool_worker *thread_arr;

	struct tpool_work *work_queue_head;
	pthread_mutex_t work_mutex;

	size_t working_count;
	size_t thread_count;
	bool stop;

	void (*add_work)(struct ThreadPool *self, thread_func func, void *arg);
	void (*wait)(struct ThreadPool *self);
	void (*destroy)(struct ThreadPool *self);
} ThreadPool;

ThreadPool *tpool_init(size_t thread_count);

#define smodify(block, mutex_ptr)                                              \
	do {                                                                       \
		pthread_mutex_lock(mutex_ptr);                                         \
		block;                                                                 \
		pthread_mutex_unlock(mutex_ptr);                                       \
	} while (false);

#endif // THREAD_POOL_H
