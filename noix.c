#include "sys.h"
#include "util.h"

static void notimpl(char *s) NORETURN;

static void
notimpl(char *s)
{
	fatal("not implemented: %s", s);
}

Imm
xread(int fd, char *p, Imm len)
{
	notimpl("xread");
}

char*
readfile(char *filename)
{
	notimpl("readfile");
}

Imm
xwrite(int fd, char *p, Imm len)
{
	notimpl("xwrite");
}

void
xvprintf(char *fmt, va_list args)
{
	notimpl("xvprintf");
}

void
xprintf(char *fmt, ...)
{
	notimpl("printf");
}

void*
mmap(void *start, size_t length, int prot, int flags, int fd, off_t off)
{
	if((flags&MAP_ANON) == 0)
		notimpl("mmap");
	return xmalloc(length);
}

int
munmap(void *start, size_t length)
{
	free(start);
	return 0;
}

void
exit(int status)
{
	notimpl("exit");
}

void
xabort(void)
{
	while(1)
		;
}

/* referenced only by lex.yy.o */
//FILE *stdin = 0;
//FILE *stdout = 0;

/* referenced only by lex.yy.o and c.tab.o */
//FILE *stderr = 0;

/* referenced only by lex.yy.o */
ssize_t
read(int fd, void *buf, size_t cnt)
{
	xabort();
}

/* referenced only by lex.yy.o */
int
fileno(FILE *fp)
{
	xabort();
}

/* referenced only by lex.yy.o */
int
isatty(int fd)
{
	xabort();
}

/* referenced only by lex.yy.o and c.tab.o */
int
fprintf(FILE *fp, const char *fmt, ...)
{
	xabort();
}

/* referenced only by c.tab.o */
void
abort()
{
	notimpl("abort");
}

/* referenced only by c.tab.o */
int
fputc(int c, FILE *fp)
{
	xabort();
}

/* referenced only by c.tab.o */
size_t
fwrite(const void *ptr, size_t size, size_t nmeb, FILE *fp)
{
	xabort();
}
