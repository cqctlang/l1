#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static void*
doit(void *p)
{
	pthread_t t1, t2;
	long n;

	n = (long)p;
	printf("doit %d\n", n);
	if(n == 0)
		return;
	pthread_create(&t1, 0, doit, (void*)(n-1));
	pthread_create(&t2, 0, doit, (void*)(n-1));
	pthread_join(t1, 0);
	pthread_join(t2, 0);

	return 0;
}

int
main(int argc, char *argv[])
{
	if(argc > 1)
		doit((void*)(long)atoi(argv[1]));
	else
		doit((void*)2);
	return 0;
}
