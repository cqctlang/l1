#include "sys.h"
#include "util.h"

enum {
	Maxfaulthook = 5,
};
typedef void(Faulthook)(void);
static Faulthook *faulthook[Maxfaulthook];
static unsigned nfh;
u64 cqctmeminuse;
u64 cqctmemtotal;

unsigned
max(unsigned x, unsigned y)
{
	return x>y?x:y;
}

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
fatal(char *fmt, ...)
{
	unsigned n;
	va_list args;
	xprintf("internal error: ");
	va_start(args, fmt);
	xvprintf(fmt, args);
	va_end(args);
	xprintf("\n");
	n = nfh;
	while(n-- != 0)
		faulthook[n]();
	xprintf("please report this l1 failure\n");
	xabort();
}

void
bug()
{
	fatal("bug");
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
		return 0;
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
		return 0;
	t = emalloc(len+1);
	memcpy(t, s, len);
	return t;
}

void*
emalloc(size_t size)
{
	char *p;
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

size_t
esize(void *vp)
{
	char *p;
	p = vp;
	if(p == 0)
		return 0;
	p -= sizeof(size_t);
	return sizeof(size_t)+*(size_t*)p;
}

void
efree(void *vp)
{
	char *p;
	p = vp;
	if(p == 0)
		return;
	p -= sizeof(size_t);
	cqctmeminuse -= *(size_t*)p;
	free(p);
}

void*
erealloc(void *p, size_t old, size_t new)
{
	char *q;
	q = emalloc(new);
	if(new > old){
		memcpy(q, p, old);
		cqctmeminuse += new-old;
		cqctmemtotal += new-old;
	}else{
		memcpy(q, p, new);
		cqctmeminuse -= old-new;
	}
	efree(p);
	return q;
}

/* c = a - b */
void
tvdiff(struct timeval *a, struct timeval *b, struct timeval *c)
{
        c->tv_sec = a->tv_sec - b->tv_sec;
        c->tv_usec = a->tv_usec - b->tv_usec;
	if(a->tv_usec >= b->tv_usec)
		c->tv_usec = a->tv_usec - b->tv_usec;
	else{
                c->tv_sec -= 1;
                c->tv_usec = a->tv_usec+1000000 - b->tv_usec;
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
xisblank(int c)
{
	return (c == ' ') || (c == '\t');
}

int
xisspace(int c)
{
	return c == ' ' || ('\t' <= c && c <= '\r');
}

int
xisodigit(int c)
{
	return ('0' <= c) && (c <= '7');
}

int
xisdigit(int c)
{
	return ('0' <= c) && (c <= '9');
}

int
xisxdigit(int c)
{
	return(xisdigit(c)
	       || (('a' <= c) && (c <= 'f'))
	       || (('A' <= c) && (c <= 'F')));
}

int
xislower(int c)
{
	return ('a' <= c) && (c <= 'z');
}

int
xisupper(int c)
{
	return ('A' <= c) && (c <= 'Z');
}

int
xisalpha(int c)
{
	return xisupper(c) || xislower(c);
}

int
xisalnum(int c)
{
	return xisalpha(c) || xisdigit(c);
}

int
xisascii(int c)
{
	return c >= 0 && c <= 0x7f;
}

int
xisprint(int c)
{
	return xisgraph(c) || xisspace(c);
}

int
xispunct(int c)
{
	return xisgraph(c) && !xisalpha(c);
}

int
xiscntrl(int c)
{
	return (0 <= c && c <= 0x1f) || c == 0x7f;
}

int
xtolower(int c)
{
	if('A' <= c && c <= 'Z')
		return c-'A'+'a';
	return c;
}

int
xtoupper(int c)
{
	if('a' <= c && c <= 'z')
		return c-'a'+'A';
	return c;
}

char**
copystrv(char **lp)
{
	unsigned n, i, nlp;
	char **p, **rv, *s;

	n = 0;
	nlp = 0;

	if(lp != 0){
		p = lp;
		while(*p){
			n += strlen(*p)+1;
			nlp++;
			p++;
		}
	}
	rv = emalloc((nlp+1)*sizeof(char*)+n);
	p = rv;
	s = (char*)(rv+nlp+1);
	for(i = 0; i < nlp; i++){
		p[i] = s;
		strcpy(s, lp[i]);
		s += strlen(lp[i])+1;
	}
	p[nlp] = 0;

	return rv;
}
