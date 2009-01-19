#include "sys.h"
#include "util.h"

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

char*
readfile(char *filename)
{
	char *buf;
	struct stat st;
	int fd;

	if(0 > stat(filename, &st))
		return 0;
	fd = open(filename, O_RDONLY);
	if(0 > fd)
		return 0;
	buf = emalloc(st.st_size+1);
	if(0 > xread(fd, buf, st.st_size)){
		efree(buf);
		close(fd);
		return 0;
	}
	close(fd);
	return buf;
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

void
xvprintf(char *fmt, va_list args)
{
	vfprintf(stdout, fmt, args);
	fflush(stdout);
}

void
xprintf(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	xvprintf(fmt, args);
	va_end(args);
}

void
xabort()
{
	abort();
}

void
newchan(int *left, int *right)
{
	int fd[2];

	if(0 > socketpair(PF_UNIX, SOCK_STREAM, 0, fd))
		fatal("cannot allocate channel");
	*left = fd[0];
	*right = fd[1];
}

void
chanclose(int c)
{
	close(c);
}

int
chanreadb(int c, char *b)
{
	if(0 > read(c, b, 1))
		return -1;
	return 0;
}

int
chanwriteb(int c, char *b)
{
	if(0 > write(c, b, 1))
		return -1;
	return 0;
}

Thread
newthread(void* (*fn)(void*), void *arg)
{
	Thread t;
	if(0 > pthread_create((pthread_t*)&t, 0, fn, arg))
		return 0;
	return t;
}

void
threadinit()
{
	sigset_t mask;
	sigfillset(&mask);
	pthread_sigmask(SIG_BLOCK, &mask, 0);
}

void
threadexit(void *vp)
{
	pthread_exit(vp);
}

void
threadwait(Thread t)
{
	pthread_join((pthread_t)t, 0);
}

void
setproftimer(u32 usec, void(*fn)())
{
	struct itimerval it;
	memset(&it, 0, sizeof(it));
	it.it_interval.tv_usec = usec;
	it.it_value.tv_usec = usec;
	if(fn)
		signal(SIGPROF, fn);
	if(0 > setitimer(ITIMER_PROF, &it, 0))
		fatal("setitimer: %s", strerror(errno));
	if(fn == 0)
		signal(SIGPROF, SIG_DFL);
}

static void
cloexec(int fd)
{
	int f;
	f = fcntl(fd, F_GETFD);
	if(f == -1)
		fatal("fcntl: %s", strerror(errno));
	f |= FD_CLOEXEC;
	if(fcntl(fd, F_SETFD, f))
		fatal("fcntl: %s", strerror(errno));
}

int
xpopen(Imm argc, char **argv)
{
	int io[2], ctl[2];
	Imm rv;
	int err;

	newchan(&io[0], &io[1]);
	newchan(&ctl[0], &ctl[1]);
	cloexec(ctl[1]);
	switch(fork()){
	case 0:
		close(io[0]);
		close(ctl[0]);
		dup2(io[1], 0);
		execvp(argv[0], argv);
		err = errno;
		xwrite(ctl[1], (char*)&err, sizeof(err));
		exit(1);
	case -1:
		return -errno;
	default:
		close(io[1]);
		close(ctl[1]);
		rv = xread(ctl[0], (char*)&err, sizeof(err));
		close(ctl[0]);
		if(rv == 0)
			return io[0];
		if(rv == -1)
			fatal("popen: cannot communicate with child: %s",
			      strerror(errno));
		/* child exec failed */
		close(io[0]);
		return -err; /* -errno of child */
	}
}
