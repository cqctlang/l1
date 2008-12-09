#include "sys.h"
#include "util.h"

enum {
	Maxfaulthook = 5,
};
typedef void(Faulthook)();
static Faulthook *faulthook[Maxfaulthook];
static unsigned nfh;
u64 cqctmeminuse;
u64 cqctmemtotal;

int
cqctfaulthook(Faulthook *h, int in)
{
	unsigned n;

	if(in){
		/* insert */
		if(nfh >= Maxfaulthook)
			return -1;
		faulthook[nfh++] = h;
		return 0;
	}else{
		/* remove */
		for(n = 0; n < nfh; n++)
			if(faulthook[n] == h){
				memmove(faulthook+n, faulthook+n+1,
					(nfh-1)*sizeof(Faulthook*));
				nfh--;
				return 0;
			}
		return -1;
	}
}

void
vmsg(char *fmt, va_list args)
{
	xvprintf(fmt, args);
	xprintf("\n");
}

void
msg(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vmsg(fmt, args);
	xprintf("\n");
	va_end(args);
}

void
fatal(char *fmt, ...)
{
	unsigned n;
	va_list args;
	xprintf("internal error: ");
	va_start(args, fmt);
	vmsg(fmt, args);
	va_end(args);
	n = nfh;
	while(n-- != 0)
		faulthook[n]();
	xabort();
}

size_t
xstrnlen(char *s, size_t max)
{
	size_t rv;
	rv = 0;
	while(rv < max && *s != '\0'){
		s++;
		rv++;
	}
	return rv;
}

char*
xstrdup(char *s)
{
	void *p;
	if(s == 0)
		return NULL;
//	s = strdup(s);
//	if(s == 0)
	p = emalloc(strlen(s)+1);
	if(p == 0)
		fatal("out of memory");
	memcpy(p, s, strlen(s));
	return p;
}

char*
xstrndup(char *s, unsigned long len)
{
	char *t;
	if(s == 0)
		return NULL;
	t = emalloc(len+1);
	memcpy(t, s, len);
	return t;
}

void*
emalloc(size_t size)
{
	void *p;
	p = malloc(size+sizeof(size_t));
	if(p == 0)
		fatal("out of memory");
	*(size_t*)p = size;
	p += sizeof(size_t);
	memset(p, 0, size);
	cqctmeminuse += size;
	cqctmemtotal += size;
	return p;
}

void
efree(void *p)
{
	if(0)
		printf("free %p\n", p);
	if(p == 0)
		return;
	p -= sizeof(size_t);
	cqctmeminuse -= *(size_t*)p;
	free(p);
}

void*
erealloc(void *p, size_t old, size_t new)
{
	size_t d;
	p -= sizeof(size_t);
	/* assert(*(size_t*)p == old); */
	p = realloc(p, new+sizeof(size_t));
	if(p == 0)
		fatal("out of memory");
	*(size_t*)p = new;
	p += sizeof(size_t);
	if(new > old){
		d = new-old;
		memset(p+old, 0, d);
		cqctmeminuse += d;
		cqctmemtotal += d;
	}else{
		d = old-new;
		cqctmeminuse += d;
	}
	return p;
}

/* c = a - b */
void
tvdiff(struct timeval *a, struct timeval *b, struct timeval *c)
{
        c->tv_sec = a->tv_sec - b->tv_sec;
        c->tv_usec = a->tv_usec - b->tv_usec;
        if (c->tv_usec < 0) {
                c->tv_sec -= 1;
                c->tv_usec += 1000000;
        }
}

char*
strnchr(char *s, int c, unsigned long len)
{
	char *e;
	e = s+len;
	while(s < e){
		if(*s == c)
			return s;
		s++;
	}
	return 0;
}

int
xisgraph(int c)
{
	return ('!' <= c) && (c <= '~');
}

int
xisspace(int c)
{
	return c == ' ' || ('\t' <= c && c <= 'r');
}
