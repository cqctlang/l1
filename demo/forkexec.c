#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int
main(int argc, char *argv[])
{

	if(argc < 2){
		printf("usage: %s command [arguments]\n", argv[0]);
		exit(1);
	}
	
	switch(fork()){
	case -1:
		printf("fork: %s", strerror(errno));
		exit(1);
	case 0:
		execv(argv[1], argv+1);
		printf("exec: %s", strerror(errno));
	default:
		wait(0);
		exit(0);
	}
}
