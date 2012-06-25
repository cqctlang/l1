#include <stdio.h>
#include <stdlib.h>

extern int	a0(int x);


int
main(int argc, char *argv[])
{
	int v, w;
	v = 1;
	w = a0(v);
	return 0;
}
