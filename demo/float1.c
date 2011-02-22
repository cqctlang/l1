#include <stdio.h>

float f;
double d;
long double ld;

#define P(a) printf("\t"#a"\t%6d\t%p\n", (int)sizeof(a), &a);

int
main(int argc, char *argv[])
{
	P(f);
	P(d);
	P(ld);
	return 0;
}
