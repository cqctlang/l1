#include "sys.h"
#include "util.h"
#include "syscqct.h"

Xenv*
mkxenv(Xenv *link)
{
	Xenv *xe;
	xe = emalloc(sizeof(Xenv));
	xe->ht = mkht();
	xe->link = link;
	return xe;
}

void
freexenv(Xenv *xe)
{
	freeht(xe->ht);
	efree(xe);
}

void*
xenvlook(Xenv *xe, char *id)
{
	void *v;

	if(xe == 0)
		return 0;
	v = hget(xe->ht, id, strlen(id));
	if(v)
		return v;
	return xenvlook(xe->link, id);
}

void
xenvbind(Xenv *xe, char *id, void *v)
{
	hput(xe->ht, id, strlen(id), v);
}

void
xenvforeach(Xenv *xe, void (*f)(void *u, char *k, void *v), void *u)
{
	hforeach(xe->ht, f, u);
}

unsigned long
xenvsize(Xenv *xe)
{
	return hnent(xe->ht);
}
