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
};

int
doit()
{
	int i, np;

	np = 0;
	for(i = 0; i < N; i++){
		switch(fork()){
		case 0:
			return child(i);
		case -1:
			fprintf(stderr, "fork: %s\n", strerror(errno));
			exit(1);
		}
		np++;
	}
}

int
main(int argc, char *argv[])
{
	doit(N);
	return 0;
}
