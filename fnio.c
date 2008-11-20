#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
fdclose(Fd *fd)
{
	close(fd->fd);
}

static int
fmtfdflush(Fmt *f)
{
	Fd *fd;
	fd = (Fd*)f->farg;
	if(fd->flags&Fclosed)
		return -1;
	if((fd->flags&Fwrite) == 0)
		return -1;
	if(0 > xwrite(fd->fd, f->start, f->to-f->start))
		return -1;
	f->to = f->start;
	return 0;
}

static void
dofdprint(VM *vm, Fd *fd, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	Fmt f;
	char buf[256];

	memset(&f, 0, sizeof(f));
	f.farg = fd;
	f.start = buf;
	f.to = buf;
	f.stop = buf+sizeof(buf);
	f.flush = fmtfdflush;
	dofmt(vm, &f, fmt, fmtlen, argc, argv);
	fmtfdflush(&f);
}

static void
l1_printf(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *fmts;
	if(argc < 1)
		vmerr(vm, "wrong number of arguments to printf");
	if(argv[0]->qkind != Qstr)
		vmerr(vm, "operand 1 to printf must be a format string");
	fmts = valstr(argv[0]);
	dofdprint(vm, vmstdout(vm), fmts->s, fmts->len, argc-1, argv+1);
}

static void
l1_print(VM *vm, Imm argc, Val *argv, Val *rv)
{
	static char *fmt = "%a\n";
	dofdprint(vm, vmstdout(vm), fmt, strlen(fmt), argc, argv);
}

static void
l1_fprintf(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *fmts;
	if(argc < 2)
		vmerr(vm, "wrong number of arguments to fprintf");
	if(argv[0]->qkind != Qfd)
		vmerr(vm, "operand 1 to fprintf must be a file descriptor");
	if(argv[1]->qkind != Qstr)
		vmerr(vm, "operand 2 to fprintf must be a format string");
	fd = valfd(argv[0]);
	fmts = valstr(argv[1]);
	dofdprint(vm, fd, fmts->s, fmts->len, argc-2, argv+2);
}

static void
l1_mapfile(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	Str *names, *map;
	char *p, *name;
	struct stat st;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mapfile");
	checkarg(vm, "mapfile", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	fd = open(name, O_RDONLY);
	efree(name);
	if(0 > fd)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	if(0 > fstat(fd, &st)){
		close(fd);
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	}
	p = mmap(0, st.st_size, PROT_READ|PROT_WRITE,
		 MAP_NORESERVE|MAP_PRIVATE, fd, 0);
	close(fd);
	if(p == MAP_FAILED)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	map = mkstrk(p, st.st_size, Smmap);
	*rv = mkvalstr(map);
}

static void
l1_open(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	int xfd;
	Str *names;
	char *name, *mode;
	int oflags, flags;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to open");
	checkarg(vm, "open", argv, 0, Qstr);
	checkarg(vm, "open", argv, 1, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	mode = str2cstr(valstr(argv[1]));

	flags = 0;
	oflags = 0;
	if(strchr(mode, 'r'))
		flags |= Fread;
	if(strchr(mode, 'w')){
		flags |= Fwrite;
		oflags |= O_CREAT;
	}
	if((flags&Fwrite) && !strchr(mode, 'a'))
		oflags |= O_TRUNC;

	if((flags&Fread) && (flags&Fwrite))
		oflags |= O_RDWR;
	else if(flags&Fread)
		oflags |= O_RDONLY;
	else if(flags&Fwrite)
		oflags |= O_WRONLY;

	xfd = open(name, oflags, 0777); /* ~umask */
	efree(name);
	efree(mode);
	if(0 > xfd)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	fd = mkfd(names, xfd, flags, fdclose);
	*rv = mkvalfd(fd);
}

static void
l1_read(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *s;
	char *buf;
	Cval *n;
	Imm r;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to read");
	checkarg(vm, "read", argv, 0, Qfd);
	checkarg(vm, "read", argv, 1, Qcval);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to read from closed file descriptor");
	n = valcval(argv[1]);
	buf = emalloc(n->val);	/* FIXME: check sign, <= SSIZE_MAX */
	r = xread(fd->fd, buf, n->val);
	if(r == (Imm)-1)
		vmerr(vm, "read error: %s", strerror(errno));
	if(n->val > 0 && r == 0)
		return;		/* nil */
	s = mkstrk(buf, r, Smalloc);
	*rv = mkvalstr(s);
}

static void
l1_write(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *s;
	int r;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to write");
	checkarg(vm, "write", argv, 0, Qfd);
	checkarg(vm, "write", argv, 1, Qstr);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to write to closed file descriptor");
	s = valstr(argv[1]);
	r = xwrite(fd->fd, s->s, s->len);
	if(r == -1)
		vmerr(vm, "write error: %s", strerror(errno));
	/* return nil */
}

static void
l1_popen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	int xfd;
	Imm m;
	char **xargv;

	if(argc == 0)
		vmerr(vm, "wrong number of arguments to popen");
	for(m = 0; m < argc; m++)
		checkarg(vm, "popen", argv, m, Qstr);
	xargv = emalloc((argc+1)*sizeof(char*)); /* null terminated */
	for(m = 0; m < argc; m++)
		xargv[m] = str2cstr(valstr(argv[m]));

	xfd = xpopen(argc, xargv);
	for(m = 0; m < argc; m++)
		efree(xargv[m]);
	efree(xargv);
	if(xfd == -1)
		vmerr(vm, "popen error: %s", strerror(errno));
	fd = mkfd(mkstr0("<pipe>"), xfd, Fread|Fwrite, fdclose);
	*rv = mkvalfd(fd);
}

void
fnio(Env *env)
{
	FN(fprintf);
	FN(mapfile);
	FN(open);
	FN(popen);
	FN(print);		/* FIXME: remove: held for test suite */
	FN(printf);
	FN(read);
	FN(write);
}
