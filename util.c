#include "sys.h"
#include "util.h"

static Faulthook *faulthook;

void
setfaulthook(Faulthook *h)
{
	faulthook = h;
}

void
vmsg(char *fmt, va_list args)
{
	xvprintf(fmt, args);
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
	if(s == 0)
		return NULL;
	s = strdup(s);
	if(s == 0)
		fatal("out of memory");
	return s;
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
	p = malloc(size);
	if (!p)
		fatal("out of memory");
	memset(p, 0, size);
	return p;
}

void*
xrealloc(void *p, size_t old, size_t new)
{
	p = realloc(p, new);
	if(p == NULL)
		fatal("out of memory");
	memset(p+old, 0, new-old);
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
