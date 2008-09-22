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
	buf = xmalloc(st.st_size+1);
	if(0 > xread(fd, buf, st.st_size)){
		free(buf);
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
	if(0 > pthread_create(&t, 0, fn, arg))
		return 0;
	return t;
}

void
threadexit(void *vp)
{
	pthread_exit(vp);
}

void
threadwait(Thread t)
{
	pthread_join(t, 0);
}
