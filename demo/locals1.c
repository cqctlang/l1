#include <stdio.h>
#include <stdlib.h>

typedef struct Node Node;
struct Node {
	int v;
	Node *next;
};

typedef
enum X
{
	x0, x1, x2, x3,
} X;

typedef
enum Y
{
	y0, y1, y2, y3,
	y4 = 18446744073709551615ULL, 
} Y;

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
}

int
main(int argc, char *argv[])
{
	locals(argv[0]);
	return 0;
}
