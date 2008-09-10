#include "sys.h"
#include "util.h"
#include "syscqct.h"

static void
freefdclose(Fd *fd)
{
	close(fd->fd);
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
	free(name);
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
	free(name);
	free(mode);
	if(0 > xfd)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	fd = mkfd(names, xfd, flags, freefdclose);
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
	buf = xmalloc(n->val);	/* FIXME: check sign, <= SSIZE_MAX */
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

void
fnio(Env *env)
{
	FN(mapfile);
	FN(open);
	FN(read);
	FN(write);
}
