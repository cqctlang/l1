#include "sys.h"
#include "util.h"

Imm
xread(int fd, char *p, Imm len)
{
	xabort();
}

char*
readfile(char *filename)
{
	xabort();
}

Imm
xwrite(int fd, char *p, Imm len)
{
	xabort();
}

void
xvprintf(char *fmt, va_list args)
{
	xabort();
}

void
xprintf(char *fmt, ...)
{
	xabort();
}

void*
mmap(void *start, size_t length, int prot, int flags, int fd, off_t off)
{
	if((flags&MAP_ANONYMOUS) == 0)
		xabort();
	return xmalloc(length);
}

int
munmap(void *start, size_t length)
{
	free(start);
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

/* referenced only by lex.yy.o */
FILE *stdin = 0;
FILE *stdout = 0;

/* referenced only by lex.yy.o and c.tab.o */
FILE *stderr = 0;

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

void
exit(int status)
{
	xabort();
}

void
xabort(void)
{
	while(1)
		;
}

void
abort(void)
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
