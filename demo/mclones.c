#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

enum
{
	N = 2,
	M = 10,
};
long depth;


static void*
xmalloc(int sz)
{
	return malloc(sz);
}

static void
xfree(void *p)
{
	free(p);
}

static void
work()
{
	int i;
	void *p[M];
	for(i = 0; i < M; i++)
		p[i] = xmalloc(i);
	for(i = 0; i < M; i++)
		xfree(p[i]);
}

static void*
doit(void *p)
{
	pthread_t t[N];
	unsigned long i;
	long n;

	n = (long)p;
	printf("clone level %ld\n", n);
	if(n == 0)
		return 0;
	for(i = 0; i < N; i++)
		pthread_create(&t[i], 0, doit, (void*)(n-1));
	work();
	for(i = 0; i < N; i++)
		pthread_join(t[i], 0);
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
