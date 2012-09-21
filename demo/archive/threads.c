#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

int nt;

static void*
doit(void *p)
{
	pthread_t t1, t2;
	long n;

	nt++;
	n = (long)p;
	printf("level %d\n", n);
	if(n == 0)
		execl("/bin/ls", "/bin/ls", 0);
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
