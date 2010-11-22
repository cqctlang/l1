#include <stdio.h>
#include <stdlib.h>
#include "locals1.h"

static struct Q q;
static Z z;

#define P(a) printf("\t"#a"\t%6d\t%p\n", (int)sizeof(a), &a);

static void
locals(char *argv0)
{
	int a;
	unsigned long b;
	char *c;
	unsigned long *const d = 0;
	int e[100];
	struct Node f[100];
	Node g[100];
	Node *h[100];
	X i[100];
	Y j[100];
	const unsigned long *k;
	Node l;
	int m[100][100];
	Node n[100][100];
	static char *o;
	Node p[0];
	V q;
	W r;
	U s;

	printf("%s locals (actual):\n", argv0);
	P(a);
	P(b);
	P(c);
	P(d);
	P(e);
	P(f);
	P(g);
	P(h);
	P(i);
	P(j);
	P(k);
	P(l);
	P(m);
	P(n);
	P(o);
	P(p);
	P(q);
	P(r);
	P(s);
}

int
main(int argc, char *argv[])
{
	z.bitfield1 = 0;
	q.x = 0;
	locals(argv[0]);
	return 0;
}
