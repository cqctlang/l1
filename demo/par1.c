#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

enum
{
	N = 5,
	M = 10000000,
};

int p[N];

static void
xsync()
{
}

static void
checkpoint(int i)
{
}

static void
child(int i)
{
	xsync();
	while(p[i] < M){
		p[i]++;
		if(p[i]%1000000 == 0)
			checkpoint(i);
	}
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
}

int
main(int argc, char *argv[])
{
	doit(N);
	return 0;
}
