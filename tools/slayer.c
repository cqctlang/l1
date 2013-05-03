
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int
main(int argc,char **argv) {
	int s;

	if(argc<2) {
		fprintf(stderr,"usage: %s <signum>\n",argv[0]);
		exit(1);
	}

	s=atoi(argv[1]);

	kill(getpid(),s);

	exit(0);
}
