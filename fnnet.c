#include "sys.h"
#include "util.h"
#include "syscqct.h"

static int
parseaddr(const char *s, struct in_addr *addr)
{
	struct hostent* h;
	if(*s == '\0')
		return 0;
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
	char *buf = 0;
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
sa2str(struct sockaddr_in *sa, char *buf, unsigned long sz)
{
	static char addr[32];
	strcpy(addr, inet_ntoa(sa->sin_addr));
	snprintf(buf, sz, "%s:%d", addr, ntohs(sa->sin_port));
}

static void
nodelay(int fd)
{
	int optval = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

static void
reuseaddr(int fd)
{
	int optval = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

static void
l1__socket(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	Cval *dom, *type, *prot;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to socket");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	checkarg(vm, argv, 2, Qcval);
	dom = valcval(argv[0]);
	type = valcval(argv[1]);
	prot = valcval(argv[2]);
	fd = socket((int)cvalu(dom), (int)cvalu(type), (int)cvalu(prot));
	if(0 > fd)
		vmerr(vm, "socket: %s", strerror(errno));
	*rv = mkvallitcval(Vint, fd);
}

static void
l1__tcpopen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	Str *str;
	char *s;
	struct sockaddr_in saddr;

	setlasterrno(0);
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tcpopen");
	checkarg(vm, argv, 0, Qstr);
	str = valstr(argv[0]);
	s = str2cstr(str);
	if(0 > parseip(s, &saddr)){
		efree(s);
		vmerr(vm, "unrecognized address: %.*s",
		      (int)str->len, strdata(str));
	}
	efree(s);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > fd)
		vmerr(vm, "tcpopen: %s", strerror(errno));
	if(0 > connect(fd, (struct sockaddr*)&saddr, sizeof(saddr))){
		setlasterrno(errno);
		return;
	}
	nodelay(fd);
	*rv = mkvallitcval(Vint, fd);	
}

static void
l1__tcplisten(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	Str *str;
	char *s;
	struct sockaddr_in saddr;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tcplisten");
	checkarg(vm, argv, 0, Qstr);
	str = valstr(argv[0]);
	s = str2cstr(str);
	if(0 > parseip(s, &saddr)){
		efree(s);
		vmerr(vm, "unrecognized address: %.*s",
		      (int)str->len, strdata(str));
	}
	efree(s);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > fd)
		vmerr(vm, "tcplisten: %s", strerror(errno));
	reuseaddr(fd);
	if(0 > bind(fd, (struct sockaddr*)&saddr, sizeof(saddr)))
		vmerr(vm, "tcplisten: %s", strerror(errno));
	if(0 > listen(fd, 4096))
		vmerr(vm, "tcplisten: %s", strerror(errno));
	*rv = mkvallitcval(Vint, fd);	
}

static void
l1__tcpaccept(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	Fd *fd0;
	Xfd *xfd0;
	struct sockaddr_in saddr;
	socklen_t len;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tcpaccept");
	checkarg(vm, argv, 0, Qfd);
	fd0 = valfd(argv[0]);
	if((fd0->flags&Ffn) == 0)
		vmerr(vm, "file descriptor is not a listener");
	xfd0 = &fd0->u.fn;
	len = sizeof(saddr);
	fd = accept(xfd0->fd, (struct sockaddr*)&saddr, &len);
	if(0 > fd)
		vmerr(vm, "tcpaccept: %s", strerror(errno));
	nodelay(fd);
	*rv = mkvallitcval(Vint, fd);
}

static void
l1_getpeername(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Xfd *xfd;
	struct sockaddr_in sa;
	int r;
	socklen_t len;
	char buf[128];

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to getpeername");
	checkarg(vm, argv, 0, Qfd);
	fd = valfd(argv[0]);
	xfd = &fd->u.fn;
	len = sizeof(sa);
	r = getpeername(xfd->fd, (struct sockaddr*)&sa, &len);
	if(0 > r)
		vmerr(vm, "getpeername: %s", strerror(errno));
	sa2str(&sa, buf, sizeof(buf));
	*rv = mkvalstr(mkstr0(buf));
}

static void
l1_getsockname(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Xfd *xfd;
	struct sockaddr_in sa;
	int r;
	socklen_t len;
	char buf[128];

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to getsockname");
	checkarg(vm, argv, 0, Qfd);
	fd = valfd(argv[0]);
	xfd = &fd->u.fn;
	len = sizeof(sa);
	r = getsockname(xfd->fd, (struct sockaddr*)&sa, &len);
	if(0 > r)
		vmerr(vm, "getsockname: %s", strerror(errno));
	sa2str(&sa, buf, sizeof(buf));
	*rv = mkvalstr(mkstr0(buf));
}

static void
l1__unixopen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	Str *str;
	struct sockaddr_un sa;

	setlasterrno(0);
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to unixopen");
	checkarg(vm, argv, 0, Qstr);
	str = valstr(argv[0]);
	if(str->len > 104)
		/* 104 is the lesser of freebsd's and linux's
		   max unix pathname */
		vmerr(vm, "unix domain pathname is too long");
	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(0 > fd){
		setlasterrno(errno);
		return;
	}
	sa.sun_family = AF_UNIX;
	memcpy(sa.sun_path, strdata(str), str->len);
	if(0 > connect(fd, (struct sockaddr*)&sa,
		       sizeof(sa_family_t)+str->len)){
		setlasterrno(errno);
		return;
	}
	*rv = mkvallitcval(Vint, fd);	
}

static void
l1__recvfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int rfd;
	int fd;
	Fd *fd0;
	struct cmsghdr *cmsg;
	struct msghdr msg;
	struct iovec iv;
	unsigned char byte;
	char buf[CMSG_SPACE(sizeof(rfd))];

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to recvfd");
	checkarg(vm, argv, 0, Qfd);
	fd0 = valfd(argv[0]);
	if((fd0->flags&Ffn) == 0)
		vmerr(vm, "file descriptor cannot convey file descriptors");
	fd = fd0->u.fn.fd;
	memset(&msg, 0, sizeof(msg));
	iv.iov_base = &byte;
	iv.iov_len = sizeof(byte);
	msg.msg_iov = &iv;
	msg.msg_iovlen = 1;
	msg.msg_control = buf;
	msg.msg_controllen = sizeof(buf);
	if(0 > recvmsg(fd, &msg, 0)){
		setlasterrno(errno);
		return;
	}
	cmsg = CMSG_FIRSTHDR(&msg);
	if(cmsg == 0
	   || cmsg->cmsg_level != SOL_SOCKET
	   || cmsg->cmsg_type != SCM_RIGHTS
	   || cmsg->cmsg_len != CMSG_LEN(sizeof(rfd)))
		vmerr(vm, "no file descriptor to be received");
	memcpy(&rfd, CMSG_DATA(cmsg), sizeof(rfd));
	*rv = mkvallitcval(Vint, rfd);	
}

void
fnnet(Env env)
{
	FN(getpeername);
	FN(getsockname);
	FN(_recvfd);
	FN(_socket);
	FN(_tcpaccept);
	FN(_tcplisten);
	FN(_tcpopen);
	FN(_unixopen);
}
