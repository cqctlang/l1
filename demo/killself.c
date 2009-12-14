#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	printf("about to SIGSTOP myself (%d)\n", getpid());
	kill(getpid(), SIGSTOP);
	printf("resumed from SIGSTOP (%d)\n", getpid());
	return 0;
}
