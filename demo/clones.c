#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

long depth;

static void
work()
{
	int i;
	for(i = 0; i < 100000; i++)
		i++;
}

static void*
doit(void *p)
{
	pthread_t t1, t2;
	long n, i;

	n = (long)p;
	printf("clone level %d\n", n);
	if(n == 0)
		return;
	pthread_create(&t1, 0, doit, (void*)(n-1));
	pthread_create(&t2, 0, doit, (void*)(n-1));
	for(i = 0; i < 100; i++)
		work();
	pthread_join(t1, 0);
	pthread_join(t2, 0);

	return 0;
}

int
main(int argc, char *argv[])
{
	if(argc > 1)
		depth = (long)atoi(argv[1]);
	else
		depth = 2;
	doit((void*)depth);
	return 0;
}
