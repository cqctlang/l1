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

void
freexenvall(Xenv *xe)
{
	Xenv *l;
	if(xe == 0)
		return;
	l = xe->link;
	freexenv(xe);
	freexenvall(l);
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
xenvupdate(Xenv *xe, char *id, void *v)
{
	if(xe == 0)
		fatal("xenvupdate on unbound identifier: %s", id);
	if(hget(xe->ht, id, strlen(id))){
		hput(xe->ht, id, strlen(id), v);
		return;
	}
	xenvupdate(xe->link, id, v);
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

static void
wrapbind(void *u, char *k, void *v)
{
	xenvbind(u, k, v);
}

Xenv*
xenvcopy(Xenv *xe)
{
	Xenv *nxe;
	if(xe == 0)
		return 0;
	nxe = mkxenv(xenvcopy(xe->link));
	xenvforeach(xe, wrapbind, nxe);
	return nxe;
}

struct arg {
	HT *ht;
	Xenv *fr;
};

static void
diff1(void *u, char *id, void *v)
{
	struct arg *a;
	a = u;
	if(xenvlook(a->fr, id) != v)
		hput(a->ht, id, strlen(id), v);
}

/* assume TO and FR bind the same identifiers;
   return bindings in TO with different value.
   bindings returned in caller-freed hash table,
   possibly empty. */
HT*
xenvdiff(Xenv *fr, Xenv *to)
{
	struct arg a;
	a.ht = mkht();
	a.fr = fr;
	xenvforeach(to, diff1, &a);
	return a.ht;
}

static void
appl1(void *u, char *id, void *v)
{
	Xenv *xe;
	xe = u;
	xenvbind(xe, id, v);
}

void
xenvapply(Xenv *xe, HT *d)
{
	hforeach(d, appl1, xe);
}
