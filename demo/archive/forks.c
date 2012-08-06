#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

long depth;

static void
doit(long n)
{
	int p0, p1;
	printf("fork level %ld\n", n);
	if(n == 0)
		return;

	switch(p0 = fork()){
	case 0:
		doit(n-1);
		exit(0);
	case -1:
		fprintf(stderr, "fork: %s\n", strerror(errno));
		exit(1);
	}

	switch(p1 = fork()){
	case 0:
		doit(n-1);
		exit(0);
	case -1:
		fprintf(stderr, "fork: %s\n", strerror(errno));
		exit(1);
	}

	waitpid(p0, 0, 0);
	waitpid(p1, 0, 0);
}

int
main(int argc, char *argv[])
{
	if(argc > 1)
		depth = (long)atoi(argv[1]);
	else
		depth = 2;
	doit(depth);
	return 0;
}
