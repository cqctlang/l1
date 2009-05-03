#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "x/include/utf.h"
#include "x/include/fmt.h"

static int
xfdflush(Fmt *f)
{
	Imm n, rv;
	Xfd *xfd;
	n = (char*)f->to-(char*)f->start;
	xfd = (Xfd*)f->farg;
	rv = xfd->write(xfd, f->start, n);
	if(rv == -1)
		return 0;
	f->to = f->start;
	return 1;
}

static void
xfdfmtinit(Fmt *f, Xfd *xfd, char *buf, int size)
{
	f->runes = 0;
	f->start = buf;
	f->to = buf;
	f->stop = buf + size;
	f->flush = xfdflush;
	f->farg = (void*)xfd;
	f->flags = 0;
	f->nfmt = 0;
	fmtlocaleinit(f, 0, 0, 0);
}

void
cvprintf(Xfd *xfd, char *fmt, va_list args)
{
	struct Fmt f;
	char buf[256];

	if(!xfd->write)
		return;
	xfdfmtinit(&f, xfd, buf, sizeof(buf));
	fmtvprint(&f, fmt, args);
	xfdflush(&f);
}

void
cprintf(Xfd *xfd, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	cvprintf(xfd, fmt, args);
	va_end(args);
}
