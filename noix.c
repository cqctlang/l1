#include "sys.h"
#include "util.h"

char sysarch[256];
char sysos[256];

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
xwrite(int fd, void *p, Imm len)
{
	notimpl("xwrite");
}

void
xvprintf(char *fmt, va_list args)
{
	vprintf(fmt, args);
}

void
xprintf(char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	xvprintf(fmt, args);
	va_end(args);
}

void*
mmap(void *start, size_t length, int prot, int flags, int fd, off_t off)
{
	if((flags&MAP_ANON) == 0)
		notimpl("mmap");
	return emalloc(length);
}

int
munmap(void *start, size_t length)
{
	efree(start);
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
FILE *stdin = 0;
FILE *stdout = 0;

/* referenced only by lex.yy.o and c.tab.o */
FILE *stderr = 0;

/* referenced only by lex.yy.o */
void
clearerr(FILE *fp)
{
	notimpl("clearerr");
}

/* referenced only by lex.yy.o */
int*
__errno_location()
{
	notimpl("__errno_location");
}

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

char*
dirname(char *path)
{
	xabort();
}

int
xmunmap(void *p, Imm len)
{
	notimpl("xmunmap");
}

Imm
usec(void)
{
	notimpl("usec");
}

void
initos()
{
	strcpy(sysarch, "unknown");
	strcpy(sysos, "unknown");
}
