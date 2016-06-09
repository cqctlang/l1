typedef struct Node Node;
struct Node {
	int v;
	Node *next;
};

typedef
enum
{
	u0 = 0,
	u1,
	u2,
} U;


typedef
enum V
{
	v0 = 0x00000000,
	v1 = 0x00000001,
	v2 = 0x00000002,
	v3 = 0x00000004,
	v4 = 0x00000040,
	v5 = 0x00000080,
	v6 = 0x80000000
} V;

typedef
enum W
{
	w0 = -3,
	w1,
	w2,
	w3,
	w4,
	w5,
	w6,
} W;

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

typedef struct Z
{
	int bitfield1 : 1;
	int bitfield2 : 2;
	int bitfield3 : 3;
	int (*fn)(void *arg1, char *arg2);
	struct Node f[100];
	struct Node g[10][20][30];
	struct Node h[0];
	int i[0];
} Z;

