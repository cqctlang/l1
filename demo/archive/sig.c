#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

static unsigned n;

void
sigalrm(int sig)
{
	printf("alarm %d\n", ++n);
}


int
main(int argc, char *argv[])
{
	struct itimerval it;

	memset(&it, 0, sizeof(it));
	it.it_value.tv_usec = it.it_interval.tv_usec = 100000;
	signal(SIGALRM, sigalrm);
	setitimer(ITIMER_REAL, &it, 0);
	while(n < 10)
		;
	exit(0);
}

