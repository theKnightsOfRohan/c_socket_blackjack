#include "../../src/Server/thread_pool.h"
#include "../../src/globals.h"
#include <stdio.h>
#include <stdlib.h>

void *random_work(void *arg) {
	int seed = *(int *)arg;
	srand(seed);
	int num = rand() % 20;
	Log("The random number was %d with a seed of %d\n", num, seed);
	return NULL;
}

// Does shit just work
void functional_test() {
	Log("=======> FUNCTIONAL_TEST\n");
	ThreadPool *pool = tpool_init(4);
	int seeds[] = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
	for (int i = 0; i < 10; i++) {
		Log("Adding work with seed %d\n", seeds[i]);
		pool->add_work(pool, &random_work, &seeds[i]);
	}

	Log("Waiting pool...\n");
	pool->wait(pool);
	Log("Pool waited. Destroying pool...\n");
	pool->destroy(pool);
	Log("Pool destroyed.\n");

	Log("<======= FUNCTIONAL_TEST\n")
}

struct cross_product_args {
	int *a;
	int *b;
	int size;
	int target;
	int *output;
	pthread_mutex_t *out_mutex;
};

void *cross_product(void *arg) {
	struct cross_product_args *args = (struct cross_product_args *)arg;

	int target_index = args->target;

	int index1 = (target_index + 1) % args->size;
	int index2 = (target_index + 2) % args->size;

	int plus = args->a[index1] * args->b[index2];
	int minus = args->a[index2] * args->b[index1];

	int final = plus - minus;

	smodify({ args->output[target_index] = final; }, args->out_mutex);

	free(arg);

	return NULL;
}

// Is the thread pool implementation thread-safe
void safety_test() {
	Log("=======> SAFETY_TEST\n");

	pthread_mutex_t out_mutex;
	pthread_mutex_init(&out_mutex, NULL);

	ThreadPool *pool = tpool_init(10);

	int size = 10000;

	Log("Initializing arrays of size %d\n", size);

	int *a = smalloc(sizeof(int) * size);
	int *b = smalloc(sizeof(int) * size);
	int *output = smalloc(sizeof(int) * size);

	for (int i = 0; i < size; i++) {
		int val = rand();
		a[size] = val;
		b[size] = val;
	}

	Log("Adding all work to thread pool\n");

	for (int i = 0; i < size; i++) {
		struct cross_product_args *args =
			smalloc(sizeof(struct cross_product_args));

		args->a = a;
		args->b = b;
		args->size = size;
		args->output = output;
		args->target = i;
		args->out_mutex = &out_mutex;

		pool->add_work(pool, cross_product, args);
	}

	Log("Work added. Waiting pool...\n");
	pool->wait(pool);

	Log("Work finished. Destroying pool...\n");
	pool->destroy(pool);

	Log("Pool destroyed. Checking output...\n");
	for (int i = 0; i < size; i++) {
		assert(output[i] == 0);
	}

	Log("Output correct.\n");

	Log("<======= SAFETY_TEST\n");
}

void test_tpool() {
	Log("=======> TPOOL_TEST\n");
	functional_test();
	safety_test();
	Log("<======= TPOOL_TEST\n");
}
