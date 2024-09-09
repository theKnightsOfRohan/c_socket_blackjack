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
void test_tpool() {
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
}
