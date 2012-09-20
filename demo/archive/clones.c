#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

long depth;

static void
work()
{
	int i;
	for(i = 0; i < 10000000; i++)
		i++;
}

static void*
doit(void *p)
{
	pthread_t t1, t2;
	long n;

	n = (long)p;
	printf("clone level %ld\n", n);
	work();
	if(n == 0)
		return 0;
	pthread_create(&t1, 0, doit, (void*)(n-1));
	pthread_create(&t2, 0, doit, (void*)(n-1));
	pthread_join(t1, 0);
	pthread_join(t2, 0);

	return 0;
}

int
main(int argc, char *argv[])
{
	pthread_t t;
	if(argc > 1)
		depth = (long)atoi(argv[1]);
	else
		depth = 2;
	pthread_create(&t, 0, doit, (void*)depth);
	pthread_join(t, 0);
	return 0;
}
