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

sc_thread_pool_t pool_hello (worker_1, (void *)"hello");
sc_thread_pool_t pool_hi (worker_1, (void *)"hi");

int main (int argc, char *argv[])
{
	pool_hello.execute ();
	pool_hi.execute ();
	return 0;
}
