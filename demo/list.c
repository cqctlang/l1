#include <stdio.h>
#include <stdlib.h>

typedef struct Node Node;
struct Node {
	int v;
	Node *next;
};
Node *head;

void
insert(int v)
{
	Node *p, *q;

	p = malloc(sizeof(Node));
	p->v = v;
	p->next = 0;
	if(!head){
		head = p;
		return;
	}
	q = head;
	while(q->next)
		q = q->next;
	q->next = p;
}

int
main(int argc, char *argv[])
{
	int i;
	int m;
	if(argc > 1)
		m = atoi(argv[1]);
	for(i = 0; i < m; i++)
		insert(i);
	printf("list is done\n");
}
