#include <stdio.h>
#include <stdlib.h>

typedef struct Node Node;
struct Node {
	int v;
	Node *next;
};

static Node*
insert(Node *h, int v)
{
	Node *p, *q;

	p = malloc(sizeof(Node));
	p->v = v;
	p->next = 0;
	if(!h)
		return p;
	q = h;
	while(q->next)
		q = q->next;
	if(v > 1)
		q = q->next; /* oops */
	q->next = p;
	return h;
}

static void
dowork(int n)
{
	int i;
	Node *head;
	head = 0;
	for(i = 0; i < n; i++)
		head = insert(head, i);
}

int
main(int argc, char *argv[])
{
	int m;
	m = 5;
	if(argc > 1)
		m = atoi(argv[1]);
	dowork(m);
	return 0;
}
