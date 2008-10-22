#include "sys.h"
#include "util.h"
#include "syscqct.h"

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

static int
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
	efree(buf);
	return ret;
}

static void
nodelay(int fd)
{
	int optval = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

static void
fdclose(Fd *fd)
{
	close(fd->fd);
}

static void
l1_opentcp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *str;
	char *s;
	int xfd;
	struct sockaddr_in saddr;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to opentcp");
	checkarg(vm, "opentcp", argv, 0, Qstr);
	str = valstr(argv[0]);
	s = str2cstr(str);
	if(0 > parseip(s, &saddr))
		vmerr(vm, "unrecognized address: %.*s", (int)str->len, str->s);
	efree(s);
	xfd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > xfd)
		vmerr(vm, "opentcp: %s", strerror(errno));
	if(0 > connect(xfd, (struct sockaddr*)&saddr, sizeof(saddr)))
		vmerr(vm, "opentcp: %s", strerror(errno));
	nodelay(xfd);
	fd = mkfd(str, xfd, Fread|Fwrite, fdclose);
	*rv = mkvalfd(fd);
}

void
fnnet(Env *env)
{
	FN(opentcp);
}
