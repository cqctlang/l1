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

struct Q
{
	union
	{
		struct
		{
			int x1;
			int y1;
		};
	};
	int x;
};

struct Q q;

typedef struct Z
{
	int bitfield1 : 1;
	int bitfield2 : 2;
	int bitfield3 : 3;
	int (*fn)(void *arg1, char *arg2);
	struct Node f[100];
	struct Node g[10][20][30];
} Z;

Z z;

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
}

int
main(int argc, char *argv[])
{
	locals(argv[0]);
	return 0;
}
