#include "sys.h"
#include "util.h"
#include "syscqct.h"

/* FIXME: It would be nice to have open, append, etc,
   take cval O_ flags and pass them directly to kernel,
   but the definition of some flags varies by os.
   E.g., on osx O_TRUNC == 0x400, on 32-bit linux == 0x200
*/


static void
fdclose(Xfd *xfd)
{
	close(xfd->fd);
}

static Imm
fdread(Xfd *xfd, char *buf, Imm len)
{
	return xread(xfd->fd, buf, len);
}

static Imm
fdwrite(Xfd *xfd, char *buf, Imm len)
{
	return xwrite(xfd->fd, buf, len);
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
l1_access(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char *name, *mode;
	int flags;
	int xrv;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to access");
	checkarg(vm, "access", argv, 0, Qstr);
	checkarg(vm, "access", argv, 1, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	mode = str2cstr(valstr(argv[1]));

	flags = 0;
	if(strchr(mode, 'r'))
		flags |= R_OK;
	if(strchr(mode, 'w'))
		flags |= W_OK;
	if(strchr(mode, 'x'))
		flags |= X_OK;
	if(strchr(mode, 'f'))
		flags |= F_OK;
	xrv = access(name, flags);
	efree(name);
	efree(mode);
	if(xrv == 0)
		*rv = mkvalcval2(cval1);
	else if(errno == EACCES
		|| errno == ENOENT
		|| errno == EROFS
		|| errno == ENOTDIR
		|| errno == ETXTBSY)
		*rv = mkvalcval2(cval0);
	else
		vmerr(vm, "access %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
}

static void
l1_open(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Xfd xfd;
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

	xfd.fd = open(name, oflags, 0777); /* ~umask */
	efree(name);
	efree(mode);
	if(0 > xfd.fd)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	xfd.read = fdread;
	xfd.write = fdwrite;
	xfd.close = fdclose;
	fd = mkfdfn(names, flags, &xfd);
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
	if((fd->flags&Fread) == 0)
		vmerr(vm, "attempt to read non-readable file descriptor");
	if(fd->flags&Ffn){
		n = valcval(argv[1]);
		buf = emalloc(n->val);	/* FIXME: check sign, <= SSIZE_MAX */
		if(!fd->u.fn.read)
			return;	/* nil */
		r = fd->u.fn.read(&fd->u.fn, buf, n->val);
		if(r == (Imm)-1)
			vmerr(vm, "read error: %s", strerror(errno));
		if(n->val > 0 && r == 0)
			return;		/* nil */
		s = mkstrk(buf, r, Smalloc);
		*rv = mkvalstr(s);
	}else
		*rv = dovm(vm, fd->u.cl.read, argc-1, argv+1);
}

static void
l1_write(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *s;
	int r;
	Val x;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to write");
	checkarg(vm, "write", argv, 0, Qfd);
	checkarg(vm, "write", argv, 1, Qstr);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to write to closed file descriptor");
	if((fd->flags&Fwrite) == 0)
		vmerr(vm, "attempt to write non-writable file descriptor");
	if(fd->flags&Ffn){
		s = valstr(argv[1]);
		if(!fd->u.fn.write)
			return;	/* nil */
		r = fd->u.fn.write(&fd->u.fn, s->s, s->len);
		if(r == -1)
			vmerr(vm, "write error: %s", strerror(errno));
	}else{
		x = dovm(vm, fd->u.cl.write, argc-1, argv+1);
		if(x->qkind != Qnil)
			vmerr(vm, "write error");
	}
	/* return nil */
}

static void
l1_popen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Xfd xfd;
	Imm m;
	char **xargv;

	if(argc == 0)
		vmerr(vm, "wrong number of arguments to popen");
	for(m = 0; m < argc; m++)
		checkarg(vm, "popen", argv, m, Qstr);
	xargv = emalloc((argc+1)*sizeof(char*)); /* null terminated */
	for(m = 0; m < argc; m++)
		xargv[m] = str2cstr(valstr(argv[m]));

	xfd.fd = xpopen(argc, xargv);
	for(m = 0; m < argc; m++)
		efree(xargv[m]);
	efree(xargv);
	if(xfd.fd < 0)
		vmerr(vm, "%s", strerror(-xfd.fd));
	xfd.read = fdread;
	xfd.write = fdwrite;
	xfd.close = fdclose;
	fd = mkfdfn(mkstr0("<pipe>"), Fread|Fwrite, &xfd);
	*rv = mkvalfd(fd);
}

void
fnio(Env *env)
{
	FN(access);
	FN(mapfile);
	FN(open);
	FN(popen);
	FN(read);
	FN(write);
}
