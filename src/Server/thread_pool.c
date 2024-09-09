#include "thread_pool.h"
#include "../globals.h"
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct tpool_worker *tpool_get_available_worker(ThreadPool *self) {
	for (int i = 0; i < self->thread_count; i++) {
		if (self->thread_arr[i].is_working == false) {
			return &self->thread_arr[i];
		}
	}

	return NULL;
}

struct worker_start_args {
	ThreadPool *pool;
	struct tpool_worker *worker;
	int id;
};

void *tpool_worker_start(void *arg) {
	struct worker_start_args *work_args = (struct worker_start_args *)arg;
	ThreadPool *pool = work_args->pool;
	struct tpool_worker *worker = work_args->worker;
	struct tpool_work *current_work = NULL;
	int id = work_args->id;

	worker->is_working = true;

	Log("thread id %d: Initialized thread\n", id);

	while (!pool->stop || pool->work_queue_head != NULL) {
		pthread_mutex_lock(&pool->work_mutex);
		if (pool->work_queue_head == NULL) {
			worker->is_working = false;
			pthread_mutex_unlock(&pool->work_mutex);
			continue;
		} else {
			Log("thread id %d: took work\n", id);
			worker->is_working = true;

			current_work = pool->work_queue_head;
			pool->work_queue_head = pool->work_queue_head->next;
			pthread_mutex_unlock(&pool->work_mutex);

			current_work->func(current_work->arg);

			free(current_work);

			current_work = NULL;

			Log("thread id %d: finished work\n", id);
		}
	}

	Log("thread id %d: killing thread\n", id);

	free(work_args);

	return NULL;
}

// true = added work, false = did not add work
void tpool_add_work(ThreadPool *self, thread_func func, void *arg) {
	struct tpool_work *new_work = smalloc(sizeof(struct tpool_work));
	new_work->arg = arg;
	new_work->func = func;
	new_work->next = NULL;

	smodify(
		{
			struct tpool_work *last = self->work_queue_head;
			int count = 1;

			if (last == NULL) {
				self->work_queue_head = new_work;
				goto end;
			}

			while (last->next != NULL) {
				last = last->next;
				count++;
			}

			last->next = new_work;

		end:
			Log("Thread pool adding work, new size is %d\n", count);
		},
		&self->work_mutex);
}

void tpool_wait(ThreadPool *self) {
	Log("Thread pool waiting threa\n");

	while (self->work_queue_head != NULL) {
		continue;
	}
}

void tpool_destroy(ThreadPool *self) {
	self->stop = true;
	tpool_wait(self);

	for (int i = 0; i < self->thread_count; i++) {
		pthread_join(self->thread_arr[i].thread, NULL);
	}

	free(self->thread_arr);

	pthread_mutex_lock(&self->work_mutex);
	pthread_mutex_destroy(&self->work_mutex);

	free(self);

	Log("Successfully destroyed thread pool\n");
}

ThreadPool *tpool_init(size_t thread_count) {
	ThreadPool *ret = smalloc(sizeof(ThreadPool));

	assert_succ(pthread_mutex_init(&ret->work_mutex, NULL));

	ret->working_count = 0;
	ret->thread_count = thread_count;
	ret->stop = false;

	ret->thread_arr = smalloc(sizeof(struct tpool_worker) * thread_count);
	ret->work_queue_head = NULL;
	for (int i = 0; i < thread_count; i += 1) {
		struct tpool_worker *worker = &ret->thread_arr[i];

		worker->work = smalloc(sizeof(struct tpool_work));

		worker->is_working = false;

		struct worker_start_args *work_args =
			smalloc(sizeof(struct worker_start_args));

		work_args->worker = worker;
		work_args->pool = ret;
		work_args->id = i;

		pthread_create(&worker->thread, NULL, &tpool_worker_start, work_args);
	}

	ret->add_work = &tpool_add_work;
	ret->destroy = &tpool_destroy;
	ret->wait = &tpool_wait;

	Log("Successfully created thread pool with size %lu\n",
		(unsigned long)thread_count);

	return ret;
}
