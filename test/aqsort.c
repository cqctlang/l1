/* gcc -static -g -o aqsort aqsort.c */
/* solution:
0 46 2 32 4 54 6 34 8 48 10 36 12 58 14 38 16 50 18 40 20 56 22 42 24 52 26 44 28 60 30 1 3 5 7 9 11 13 15 17 19 21 23 25 27 29 31 33 35 37 39 41 43 45 47 49 51 53 55 57 59 61 62 63
*/

#include <stdio.h>
#include <stdlib.h>

#define N 64

int ncmp;

int
cmpx(const void *px, const void *py)
{
	return 0;
}

void
antiqsort(int n)
{
	int *p;
	int i;

	p = malloc(n*sizeof(*p));
	for(i = 0; i < n; i++)
		p[i] = i;
	_quicksort(p, n, sizeof(*p), cmpx);
	free(p);
}

int
main(int argc, char *argv[])
{
	antiqsort(N);
	printf("%d comparisons\n", ncmp);
	return 0;
}
