#include "threading_pthread.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>

void *worker_1 (void *arg_void)
{
	char *arg = (char *)arg_void;
	
	printf ("%s 1\n", arg);
	
	sleep (1);	
	printf ("%s 2\n", arg);
	
	return NULL;
}

sc_thread_pool_t pool_hello (worker_1, (void *)"hello");
sc_thread_pool_t pool_hi (worker_1, (void *)"hi");

sc_kernel_mutex_t mutex;

void *mutex_worker (void *arg_void)
{
	char *arg = (char *)arg_void;

	mutex.lock ();

	printf ("%s\n", arg);

	sleep(1);

	mutex.unlock ();

	return NULL;
}

sc_thread_pool_t pool_mutex (mutex_worker, (void *)"worker 1");


int main (int argc, char *argv[])
{
	printf("Threading test start\n");
	pool_hello.execute ();
	pool_hi.execute ();


	printf("Threading test done\n\nMutex test start\n");
	time_t start, end;
	time(&start);
	pool_mutex.execute ();
	time(&end);
	double time_passed = difftime(end, start);
	printf("Mutex test done. Time passed: %f seconds\n", time_passed);

	return 0;
}
