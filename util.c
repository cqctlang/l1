#include "sys.h"
#include "util.h"

static Faulthook *faulthook;

void
setfaulthook(Faulthook *h)
{
	faulthook = h;
}

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
	fprintf(stderr, "internal error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	fflush(stderr);
	if(faulthook)
		faulthook();
	abort();
}

void
fatalno(char *fmt, ...)
{
	int err;
	va_list args;

	err = errno;
	va_start(args, fmt);
	fprintf(stderr, "internal error: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, ": %s\n", strerror(err));
	va_end(args);
	fflush(stderr);
	if(faulthook)
		faulthook();
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

Imm
xread(int fd, char *p, Imm len)
{
	Imm nr;
	ssize_t rv;
	
	nr = 0;
	while(nr < len){
		rv = read(fd, p, len-nr);
		if(0 > rv && errno == EINTR)
			continue;
		if(0 > rv)
			return -1;
		if(0 == rv)
			return nr;
		nr += rv;
		p += rv;
	}
	return nr;
}

Imm
xwrite(int fd, char *p, Imm len)
{
	Imm ns;
	ssize_t rv;

	ns = 0;
	while(ns < len){
		rv = write(fd, p, len-ns);
		if(0 > rv && (errno == EINTR || errno == EAGAIN))
			continue;
		if(0 > rv)
			return -1;
		ns += rv;
		p += rv;
	}
	return ns;
}

char *
ipstr(struct sockaddr_in *sa)
{
	static char buf[128];
	static char addr[32];
	static char serv[32];
	struct servent *servent;

	strcpy(addr, inet_ntoa(sa->sin_addr));
	servent = getservbyport(sa->sin_port, "tcp");
	if(servent){
		strcpy(serv, servent->s_name);
		sprintf(buf, "%s:%s", addr, serv);
	}else
		sprintf(buf, "%s:%d", addr, ntohs(sa->sin_port));
	return buf;
}

static int
parseaddr(const char *s, struct in_addr *addr)
{
	struct hostent* h;
	h = gethostbyname(s);
	if(!h)
		return -1;
	*addr = *((struct in_addr *) h->h_addr); /* network order */
	return 0;
}

static int
parseport(const char *s, unsigned short *port)
{
	char *p;
	struct servent *se;
	unsigned long l;

	se = getservbyname(s, "tcp");
	if(se){
		*port = se->s_port;
		return 0;
	}
	l = strtoul(s, &p, 10);
	if(*p != '\0')
		return -1;
	*port = (short)htons(l);
	return 0;
}

int
parseip(char *s, struct sockaddr_in *addr)
{
	char *buf = NULL;
	char *p;
	int ret = -1;

	buf = xstrdup(s);
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	addr->sin_port = htons(0);
	if((p = strchr(buf, ':'))){
		/* HOST:PORT */
		*p++ = '\0';
		if(0 > parseaddr(buf, &addr->sin_addr))
			goto out;
		if(0 > parseport(p, &addr->sin_port))
			goto out;
	}else if ((p = strchr(buf, '.'))){
		/* HOST */
		if(0 > parseaddr(buf, &addr->sin_addr))
			goto out;
	}else{
		/* PORT or HOST? */
		if(0 > parseport(buf, &addr->sin_port)
		   && 0 > parseaddr(buf, &addr->sin_addr))
			goto out;
	}
	ret = 0;
out:
	free(buf);
	return ret;
}

void
nodelay(int fd)
{
	int optval = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

