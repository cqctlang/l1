#include "sys.h"
#include "util.h"

void
warn(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
}

void
fatal(char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	fflush(stderr);
	abort();
}

void
fatalno(char *fmt, ...)
{
	int err;
	va_list args;

	err = errno;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, ": %s\n", strerror(err));
	va_end(args);
	fflush(stderr);
	abort();
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
xrealloc(void *p, size_t size)
{
	p = realloc(p, size);
	if(p == NULL)
		fatal("out of memory");
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
