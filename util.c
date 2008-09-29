#include "sys.h"
#include "util.h"
#include <execinfo.h>

static Faulthook *faulthook;
u64 cqctmeminuse;
u64 cqctmemtotal;

void
setfaulthook(Faulthook *h)
{
	faulthook = h;
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
	va_list args;
	xprintf("internal error: ");
	va_start(args, fmt);
	vmsg(fmt, args);
	va_end(args);
	if(faulthook)
		faulthook();
	xabort();
}

char*
xstrdup(char *s)
{
	void *p;
	if(s == 0)
		return NULL;
//	s = strdup(s);
//	if(s == 0)
	p = xmalloc(strlen(s)+1);
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
	t = xmalloc(len+1);
	memcpy(t, s, len);
	return t;
}

void*
xmalloc(size_t size)
{
	void *p;
	int i, nbt;
	void *bt[64];
	char **sym;
	p = malloc(size+sizeof(size_t));
	if (!p)
		fatal("out of memory");
	memset(p, 0, size+sizeof(size_t));
	cqctmeminuse += size;
	cqctmemtotal += size;
	if(0){
		printf("alloc %p\t%ld\n", p, size);
		nbt = backtrace(bt, 64);
		sym = backtrace_symbols(bt, nbt);
		for(i = 0; i < nbt; i++)
			printf("\t%s\n", sym[i]);
		printf("\n");
		free(sym);
	}
	*(size_t*)p = size;
	return p+sizeof(size_t);
}

void
xfree(void *p)
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
xrealloc(void *p, size_t old, size_t new)
{
	void *q;
	q = p-sizeof(size_t);
	q = realloc(q, new+sizeof(size_t));
	if(q == NULL)
		fatal("out of memory");
	p = q+sizeof(size_t);
	memset(p+old, 0, new-old);
	cqctmeminuse += new-old;
	cqctmemtotal += new-old;
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
