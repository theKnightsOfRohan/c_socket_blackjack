#include "../../src/globals.h"
#include "thread_pool_test.h"
#include <assert.h>

int main(int argc, char *argv[]) {
	char *log_file = NULL;

	if (argc > 1) {
		log_file = argv[1];
	}

	log_open(log_file);

	test_tpool();
}
