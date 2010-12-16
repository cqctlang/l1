#include <stdio.h>
#include <stdlib.h>

static int
f(const void *a, const void *b)
{
	int x, y;
	x = *(int*)a;
	y = *(int*)b;
	return x-y;
}

int
main(int argc, char *argv[])
{
	int a[4];
	qsort(a, 4, sizeof(int), f);
	return 0;
}
