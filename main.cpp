#include "threading_pthread.h"
#include <stdio.h>
#include <unistd.h>

void *worker_1 (void *arg_void)
{
	char *arg = (char *)arg_void;
	
	printf ("%s 1\n", arg);
	
	sleep (1);
	
	printf ("%s 2\n", arg);
	
	return NULL;
}

sc_thread_pool_t pool_message1 (worker_1, (void *)"message 1");
sc_thread_pool_t pool_message2 (worker_1, (void *)"message 2");

sc_kernel_mutex_t mutex;

void *mutex_worker (void *arg_void)
{
	char *arg = (char *)arg_void;

	//mutex.lock ();

	printf ("%s 1\n", arg);
	
	sleep (1);
	
	printf ("%s 2\n", arg);

	//mutex.unlock ();

	return NULL;
}

sc_thread_pool_t pool_mutex1 (mutex_worker, (void *)"worker 1");
sc_thread_pool_t pool_mutex2 (mutex_worker, (void *)"worker 2");





int main (int argc, char *argv[])
{
	pool_message1.execute ();
	pool_message2.execute ();

	pool_mutex1.execute ();
	pool_mutex2.execute ();

	return 0;
}
