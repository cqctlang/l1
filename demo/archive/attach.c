#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum
{
	N = 10,
};

static int f = 1;

static void
child(int i)
{
	while(f)
		;
	printf("%d released\n", getpid());
	exit(0);
}

static void
dowait()
{
	int i;
	for(i = 0; i < N; i++)
		waitpid(-1, 0, 0);
}

static void
doit()
{
	int i, np;

	np = 0;
	for(i = 0; i < N; i++){
		switch(fork()){
		case 0:
			child(i);
			exit(-1);
		case -1:
			fprintf(stderr, "fork: %s\n", strerror(errno));
			exit(1);
		}
		np++;
	}
	dowait();
	printf("done!\n");
}

int
main(int argc, char *argv[])
{
	doit(N);
	return 0;
}
