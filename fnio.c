#include "sys.h"
#include "util.h"
#include "syscqct.h"

/* Long experience tells us that these primitives are enough
   to bring up l1 on portable systems. Please don't add 
   anything new here that could be added through lib/syslib.cqct
*/

/* FIXME: It would be nice to have open, append, etc,
   take cval O_ flags and pass them directly to kernel,
   but the definition of some flags varies by os.
   E.g., on osx O_TRUNC == 0x400, on 32-bit linux == 0x200
*/

enum
{
	// these 9P and 9P2000.u definitions come from plan9port
	OREAD	 	= 0,		// open for read
	OWRITE	 	= 1,		// write
	ORDWR	 	= 2,		// read and write
	OEXEC	 	= 3,		// execute, == read but check execute
				   	//   permission
	OTRUNC	 	= 16,		// or'ed in (except for exec),
				   	//   truncate file first
	OCEXEC	 	= 32,		// or'ed in, close on exec
	ORCLOSE	 	= 64,		// or'ed in, remove on close
	ODIRECT	 	= 128,		// or'ed in, direct access
	ONONBLOCK	= 256,		// or'ed in, non-blocking call
	OEXCL	 	= 0x1000,	// or'ed in, exclusive use (create only)
	OLOCK	 	= 0x2000,	// or'ed in, lock after opening
	OAPPEND	 	= 0x4000,	// or'ed in, append only

	// bits in Qid.type
	QTDIR		= 0x80,		// type bit for directories
	QTAPPEND	= 0x40,		// type bit for append only files
	QTEXCL		= 0x20,		// type bit for exclusive use files
	QTMOUNT		= 0x10,		// type bit for mounted channel
	QTAUTH		= 0x08,		// type bit for authentication file
	QTTMP		= 0x04,		// type bit for non-backed-up file
	QTSYMLINK	= 0x02,		// type bit for symbolic link
	QTFILE		= 0x00,		// type bits for plain file
};

/* #define since not all are int-sized */
#define DMDIR		0x80000000	// mode bit for directories
#define DMAPPEND	0x40000000	// mode bit for append only files
#define DMEXCL		0x20000000	// mode bit for exclusive use files
#define DMMOUNT		0x10000000	// mode bit for mounted channel
#define DMAUTH		0x08000000	// mode bit for authentication file
#define DMTMP		0x04000000	// mode bit for non-backed-up file

	// 9P2000.u mode bits
#define DMSYMLINK	0x02000000	// mode bit for symbolic link
#define DMDEVICE	0x00800000	// mode bit for device file
#define DMNAMEDPIPE	0x00200000	// mode bit for named pipe
#define DMSOCKET	0x00100000	// mode bit for socket
#define DMSETUID	0x00080000	// mode bit for setuid
#define DMSETGID	0x00040000	// mode bit for setgid

#define DMREAD		0x4		// mode bit for read permission
#define DMWRITE		0x2		// mode bit for write permission
#define DMEXEC		0x1		// mode bit for execute permission

typedef
struct Qid {
	u8	type;
	u32	vers;
	u64	path;
} Qid;

typedef
struct Dir
{
	u16	type;
	u32	dev;
	Qid	qid;
	u32	mode;
	u32	atime;
	u32	mtime;
	u64	length;
	u32	nuid;
	u32	ngid;
	u32	nmuid;
	char	*name;
	char	*uid;
	char	*gid;
	char	*muid;
	char	*extension;
} Dir;


static int
issysfd(Fd *fd)
{
	if(fd->flags&Ffn)
		return 1;
	else
		return 0;
}

static int
sysfdno(Fd *fd)
{
	if(!issysfd(fd))
		bug();
	return fd->u.fn.fd;
}

// this is derived from plan9port/src/lib9/_p9dir.c,
// but not as sophisticated.
static Dir*
stat2dir(char *name, struct stat *st)
{
	Dir *d;
	char *s;
	char tmp[32];

	d = emalloc(sizeof(Dir));

	s = strrchr(name, '/');
	if(s)
		s++;
	if(!s || !*s)
		s = name;
	if(*s == '/')
		s++;
	if(*s == 0)
		s = "/";
	d->name = xstrdup(s);

	d->type = 0;
	d->dev = 0;
	d->qid.path = ((u64)st->st_dev<<32)|st->st_ino;
	d->mode = st->st_mode&0777;
	d->atime = st->st_atime;
	d->mtime = st->st_mtime;
	d->length = st->st_size;
	d->nuid = st->st_uid;
	d->nuid = st->st_gid;
	d->nmuid = -1;
	snprintf(tmp, sizeof(tmp), "%d", st->st_uid);
	d->uid = xstrdup(tmp);
	snprintf(tmp, sizeof(tmp), "%d", st->st_gid);
	d->gid = xstrdup(tmp);
	d->muid = xstrdup("");
	d->extension = xstrdup("");

	switch(st->st_mode&S_IFMT){
	case S_IFSOCK:
		d->qid.type = QTFILE;
		d->mode |= DMSOCKET;
		break;
	case S_IFLNK:
		d->qid.type = QTSYMLINK;
		d->mode |= DMSYMLINK;
		break;
	case S_IFREG:
		d->qid.type = QTFILE;
		break;
	case S_IFBLK:
		d->qid.type = QTFILE;
		d->qid.path = ('b'<<16)|st->st_rdev;
		d->mode |= DMDEVICE;
		break;
	case S_IFCHR:
		d->qid.type = QTFILE;
		d->qid.path = ('c'<<16)|st->st_rdev;
		d->mode |= DMDEVICE;
		break;
	case S_IFDIR:
		d->qid.type = QTDIR;
		d->mode |= DMDIR;
		break;
	case S_IFIFO:
		d->qid.type = QTFILE;
		d->mode |= DMNAMEDPIPE;
		break;
	default:
		fatal("stat: unrecognized file mode 0x%x\n", st->st_mode);
	}

	return d;
}

static void
freedir(Dir *d)
{
	efree(d->name);
	efree(d->uid);
	efree(d->gid);
	efree(d->muid);
	efree(d->extension);
	efree(d);
}

#define XWR(u,v,k) {memcpy((u),(v),(k)); (u) += (k);}

static Str*
dir2buf(Dir *d)
{
	unsigned len;
	u32 o;
	Str *s;
	char *p;

	len = (sizeof(d->type)
	       +sizeof(d->dev)
	       +sizeof(d->qid.type)
	       +sizeof(d->qid.vers)
	       +sizeof(d->qid.path)
	       +sizeof(d->mode)
	       +sizeof(d->atime)
	       +sizeof(d->mtime)
	       +sizeof(d->length)
	       +sizeof(d->nuid)
	       +sizeof(d->ngid)
	       +sizeof(d->nmuid)
	       +sizeof(u32)		// 32-bit name pointer
	       +sizeof(u32)		// 32-bit uid pointer
	       +sizeof(u32)		// 32-bit gid pointer
	       +sizeof(u32)		// 32-bit muid pointer
	       +sizeof(u32)		// 32-bit extension pointer
	       +strlen(d->name)+1
	       +strlen(d->uid)+1
	       +strlen(d->gid)+1
	       +strlen(d->muid)+1
	       +strlen(d->extension)+1);

	s = mkstrn(len);
	p = strdata(s);
	XWR(p, &d->type, sizeof(d->type));
	XWR(p, &d->dev, sizeof(d->dev));
	XWR(p, &d->qid.type, sizeof(d->qid.type));
	XWR(p, &d->qid.vers, sizeof(d->qid.vers));
	XWR(p, &d->qid.path, sizeof(d->qid.path));
	XWR(p, &d->mode, sizeof(d->mode));
	XWR(p, &d->atime, sizeof(d->atime));
	XWR(p, &d->mtime, sizeof(d->mtime));
	XWR(p, &d->length, sizeof(d->length));
	XWR(p, &d->nuid, sizeof(d->nuid));
	XWR(p, &d->ngid, sizeof(d->ngid));
	XWR(p, &d->nmuid, sizeof(d->nmuid));

	// pointers
	o = p-strdata(s)+5*sizeof(o);
	XWR(p, &o, sizeof(o));
	o += strlen(d->name)+1;
	XWR(p, &o, sizeof(o));
	o += strlen(d->uid)+1;
	XWR(p, &o, sizeof(o));
	o += strlen(d->gid)+1;
	XWR(p, &o, sizeof(o));
	o += strlen(d->muid)+1;
	XWR(p, &o, sizeof(o));

	// pointed-to strings
	XWR(p, d->name, strlen(d->name)+1);
	XWR(p, d->uid, strlen(d->uid)+1);
	XWR(p, d->gid, strlen(d->gid)+1);
	XWR(p, d->muid, strlen(d->muid)+1);
	XWR(p, d->extension, strlen(d->extension)+1);

	return s;
}

static void
l1_mksysfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd, flags;
	Xfd xfd;
	Str *name;
	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to mksysfd");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	name = 0;
	if(argc == 3){
		checkarg(vm, argv, 2, Qstr);
		name = valstr(argv[2]);
	}
	fd = cvalu(valcval(argv[0]));
	flags = cvalu(valcval(argv[1]));
	xfd.fd = fd;
	xfd.read = xfdread;
	xfd.write = xfdwrite;
	xfd.close = xfdclose;
	*rv = mkvalfd(mkfdfn(name, flags, &xfd));
}

static void
l1_issysfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to issysfd");
	checkarg(vm, argv, 0, Qfd);
	fd = valfd(argv[0]);

	if(issysfd(fd))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_sysfdno(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to sysfdno");
	checkarg(vm, argv, 0, Qfd);
	fd = valfd(argv[0]);

	if(!issysfd(fd))
		vmerr(vm, "file descriptor is not a sysfd");

	
	*rv = mkvallitcval(Vint, sysfdno(fd));
}

static void
l1_stat(VM *vm, Imm argc, Val *argv, Val *rv)
{
	struct stat st;
	Str *names;
	char *name;
	Dir *d;
	Str *s;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stat");
	checkarg(vm, argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	if(0 > stat(name, &st)){
		efree(name);
		vmerr(vm, "cannot stat %.*s: %s",
		      (int)names->len, strdata(names),
		      strerror(errno));
	}
	d = stat2dir(name, &st);
	s = dir2buf(d);
	freedir(d);
	efree(name);

	*rv = mkvalstr(s);
}

static void
l1__mapfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd0;
	int fd;
	List *l;
	char *p, *f;
	struct stat st;
	int prot, flags;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to _mapfd");
	checkarg(vm, argv, 0, Qfd);
	f = 0;
	if(argc == 2){
		checkarg(vm, argv, 1, Qstr);
		f = str2cstr(valstr(argv[1]));
	}
	fd0 = valfd(argv[0]);
	if(fd0->flags&Fclosed)
		vmerr(vm, "attempt to map a closed file descriptor");
	if(!issysfd(fd0))
		vmerr(vm, "file descriptor cannot be mapped");
	fd = sysfdno(fd0);
	flags = MAP_PRIVATE;
	prot = 0;
	if(f){
		if(strchr(f, 'p'))
			flags = MAP_PRIVATE; /* default */
		if(strchr(f, 's'))
			flags = MAP_SHARED;

		if(strchr(f, 'r')){
			prot |= PROT_READ;
			if((fd0->flags&Fread) == 0)
				vmerr(vm, "attempt to read map non-readable file descriptor");
		}
		if(strchr(f, 'w')){
			prot |= PROT_WRITE;
			if(flags == MAP_SHARED)
				if((fd0->flags&Fwrite) == 0)
					vmerr(vm, "attempt to write map non-writeable file descriptor");
		}
		if(strchr(f, 'x'))
			prot |= PROT_EXEC;

		efree(f);
	}else{
		if((fd0->flags&Fread) != 0)
			prot |= PROT_READ;
		if((fd0->flags&Fwrite) != 0)
			prot |= PROT_WRITE;
	}
	flags |= MAP_NORESERVE;
	if(0 > fstat(fd, &st)){
		vmerr(vm, "cannot stat fd: %s",
		      strerror(errno));
	}
	if(st.st_size == 0){
		p = 0;
		goto out;
	}
	p = mmap(0, st.st_size, prot, flags, fd, 0);
out:
	l = mklist();
	_listappend(l, mkvallitcval(Vptr, (uptr)p));
	_listappend(l, mkvallitcval(Vuvlong, st.st_size));
	*rv = mkvallist(l);
}

static void
l1__munmap(VM *vm, Imm argc, Val *argv, Val *rv)
{
	void *p;
	size_t len;
	int r;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to _munmap");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	p = (void*)(uptr)cvalu(valcval(argv[0]));
	len = (size_t)cvalu(valcval(argv[1]));
	setlasterrno(0);
	r = munmap(p, len);
	if(r == -1)
		r = -errno;
	*rv = mkvallitcval(Vint, r);
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
	checkarg(vm, argv, 0, Qstr);
	checkarg(vm, argv, 1, Qstr);
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
		vmerr(vm, "access %.*s: %s",
		      (int)names->len, strdata(names),
		      strerror(errno));
}

static void
l1__ioctl(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *fd, *req, *bufp;
	int r;
	Str *bufs;
	char *p;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to ioctl");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	if(!Viskind(argv[2], Qstr) && !Viskind(argv[2], Qcval))
		vmerr(vm, "argument 3 to ioctl must be a cvalue or string");
	fd = valcval(argv[0]);
	req = valcval(argv[1]);
	if(Viskind(argv[2], Qstr)){
		bufs = valstr(argv[2]);
		p = strdata(bufs);
	}else{
		bufp = valcval(argv[2]);
		p = (char*)(uptr)cvalu(bufp);
	}

	setlasterrno(0);
	r = xioctl(cvalu(fd), (unsigned long)cvalu(req), p);
	if(r == -1)
		setlasterrno(errno);	
	*rv = mkvallitcval(Vint, r);
}

static void
l1__open(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	char *name, *mode;
	int oflags, flags, perm;

	setlasterrno(0);
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to open");
	checkarg(vm, argv, 0, Qstr);
	checkarg(vm, argv, 1, Qstr);
	checkarg(vm, argv, 2, Qcval);
	name = str2cstr(valstr(argv[0]));
	mode = str2cstr(valstr(argv[1]));
	perm = cvalu(valcval(argv[2]));

	flags = 0;
	oflags = 0;
	if(strchr(mode, 'r')) {
		flags |= Fread;
		if(strchr(mode, '+'))
			flags |= Fwrite;
	}
	if(strchr(mode, 'w') || strchr(mode, 'a'))
		flags |= Fwrite;
	if(strchr(mode, 'w')) {
		oflags |= O_CREAT|O_TRUNC;
		if(strchr(mode, '+'))
			flags |= Fread;
	}
	if((flags&Fread) && (flags&Fwrite))
		oflags |= O_RDWR;
	else if(flags&Fread)
		oflags |= O_RDONLY;
	else if(flags&Fwrite)
		oflags |= O_WRONLY;

	fd = open(name, oflags, perm); /* ~umask */
	efree(name);
	if(0 > fd){
		efree(mode);
		setlasterrno(errno);
		return;
	}
	if(strchr(mode, 'a'))
		lseek(fd, 0, SEEK_END);
	efree(mode);
	*rv = mkvallitcval(Vint, fd);
}

static void
l1_read(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *s;
	Cval *n;
	Imm r;

	setlasterrno(0);
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to read");
	checkarg(vm, argv, 0, Qfd);
	checkarg(vm, argv, 1, Qcval);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to read from closed file descriptor");
	if((fd->flags&Fread) == 0)
		vmerr(vm, "attempt to read non-readable file descriptor");
	if(fd->flags&Ffn){
		if(!fd->u.fn.read)
			return;	/* nil */
		n = valcval(argv[1]);
		s = mkstrn(cvalu(n));
		r = fd->u.fn.read(&fd->u.fn, strdata(s), cvalu(n));
		if(r == (Imm)-1){
			setlasterrno(errno);
			return;		/* nil */
		}
		if(cvalu(n) > 0 && r == 0)
			return;		/* nil */
		s = strrealloc(s, r);
		*rv = mkvalstr(s);
	}else
		*rv = ccall(vm, fd->u.cl.read, argc-1, argv+1);
}

static void
l1_write(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *s;
	int r;

	setlasterrno(0);
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to write");
	checkarg(vm, argv, 0, Qfd);
	checkarg(vm, argv, 1, Qstr);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to write to closed file descriptor");
	if((fd->flags&Fwrite) == 0)
		vmerr(vm, "attempt to write non-writable file descriptor");
	if(fd->flags&Ffn){
		s = valstr(argv[1]);
		if(!fd->u.fn.write)
			return;	/* nil */
		r = fd->u.fn.write(&fd->u.fn, strdata(s), s->len);
		if(r == -1)
			setlasterrno(errno);
	}else
		ccall(vm, fd->u.cl.write, argc-1, argv+1);
	/* return nil */
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
	if(!issysfd(fd0))
		vmerr(vm, "file descriptor cannot convey file descriptors");
	fd = sysfdno(fd0);
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

static void
l1_seek(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Cval *off, *wh;
	long r;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to seek");
	checkarg(vm, argv, 0, Qfd);
	checkarg(vm, argv, 1, Qcval);
	checkarg(vm, argv, 2, Qcval);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to seek on closed file descriptor");
	if(!issysfd(fd))
		vmerr(vm, "file descriptor does not support seek");
	off = valcval(argv[1]);
	wh = valcval(argv[2]);
	r = xlseek(sysfdno(fd), (long)cvalu(off), (int)cvalu(wh));
	if(r == -1)
		r = -errno;
	*rv = mkvallitcval(Vlong, r);
}

static void
l1__popen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fds[3];
	Imm m;
	char **xargv;
	int xrv;
	int flags;
	Cval *cv;
	List *ls;

	if(argc == 0)
		vmerr(vm, "wrong number of arguments to popen");
	flags = 0;
	if(Viskind(argv[argc-1], Qcval)){
		cv = valcval(argv[argc-1]);
		flags = cvalu(cv);
		argc--;
		if(argc == 0)
			vmerr(vm, "wrong number of arguments to popen");
	}else if(!Viskind(argv[argc-1], Qstr))
		vmerr(vm, "final argument to popen must be a string or cvalue");

	for(m = 0; m < argc; m++)
		checkarg(vm, argv, m, Qstr);
	xargv = emalloc((argc+1)*sizeof(char*)); /* null terminated */
	for(m = 0; m < argc; m++)
		xargv[m] = str2cstr(valstr(argv[m]));
	xrv = xpopen(argc, xargv, flags, fds);
	for(m = 0; m < argc; m++)
		efree(xargv[m]);
	efree(xargv);
	if(xrv < 0)
		vmerr(vm, "%s", strerror(-xrv));

	ls = mklist();
	*rv = mkvallist(ls);
	listappend(vm, ls, mkvallitcval(Vint, fds[0]));
	if(flags&PopenFullDuplex)
		listappend(vm, ls, mkvallitcval(Vint, dup(fds[0])));
	else
		listappend(vm, ls, mkvallitcval(Vint, fds[1]));
	if(flags&(PopenNoErr|PopenStderr|PopenStderrOnStdout))
		return;
	listappend(vm, ls, mkvallitcval(Vint, fds[2]));	
}

static int
setfdsin(VM *vm, List *l, fd_set *f)
{
	Val v;
	Imm i, m;
	int n, no;
	Fd *fd;

	m = listlen(l);
	n = -1;
	for(i = 0; i < m; i++){
		v = listref(l, i);
		if(!Viskind(v, Qfd))
			vmerr(vm, "select on non file descriptor");
		fd = valfd(v);
		if(!issysfd(fd))
			vmerr(vm, "select on unselectable file descriptor");
		no = sysfdno(fd);
		FD_SET(no, f);
		if(no > n)
			n = no;
	}
	return n;
}

static void
setfdsout(VM *vm, List *il, fd_set *f, List *ol)
{
	Val v;
	Imm i, m;
	Fd *fd;

	m = listlen(il);
	for(i = 0; i < m; i++){
		v = listref(il, i);
		fd = valfd(v);
		if(FD_ISSET(sysfdno(fd), f))
			listappend(vm, ol, v);
	}
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
l1__sockpair(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd[2];
	List *l;

	if(argc != 0)
		vmerr(vm, "wrong number of arguments to sockpair");
	if(0 > newchan(&fd[0], &fd[1]))
		vmerr(vm, "sockpair: %s", strerror(errno));
	l = mklist();
	listappend(vm, l, mkvallitcval(Vint, fd[0]));
	listappend(vm, l, mkvallitcval(Vint, fd[1]));
	*rv = mkvallist(l);
}

static void
l1_select(VM *vm, Imm argc, Val *argv, Val *rv)
{
	fd_set rfds, wfds, efds;
	int n, s;
	List *r, *w, *e, *rl, *t;
	Cval *cv;
	Val v;
	struct timeval tv, *tvp;

	if(argc != 3 && argc != 4)
		vmerr(vm, "wrong number of arguments to select");
	checkarg(vm, argv, 0, Qlist);
	checkarg(vm, argv, 1, Qlist);
	checkarg(vm, argv, 2, Qlist);
	tvp = 0;
	if(argc == 4){
		checkarg(vm, argv, 3, Qlist);
		tvp = &tv;
		t = vallist(argv[3]);
		if(listlen(t) != 2)
			vmerr(vm, "bad timeout specifier");
		v = listref(t, 0);
		if(!Viskind(v, Qcval))
			vmerr(vm, "bad timeout specifier");
		cv = valcval(v);
		tv.tv_sec = cvalu(cv);
		v = listref(t, 1);
		if(!Viskind(v, Qcval))
			vmerr(vm, "bad timeout specifier");
		cv = valcval(v);
		tv.tv_usec = cvalu(cv);
	}

	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
	n = -1;
	s = setfdsin(vm, vallist(argv[0]), &rfds);
	n = MAX(n, s);
	s = setfdsin(vm, vallist(argv[1]), &wfds);
	n = MAX(n, s);
	s = setfdsin(vm, vallist(argv[2]), &efds);
	n = MAX(n, s);
	n = select(n+1, &rfds, &wfds, &efds, tvp);
	if(0 > n)
		vmerr(vm, "select: %s", strerror(errno));
	r = mklist();
	w = mklist();
	e = mklist();
	setfdsout(vm, vallist(argv[0]), &rfds, r);
	setfdsout(vm, vallist(argv[1]), &wfds, w);
	setfdsout(vm, vallist(argv[2]), &efds, e);
	rl = mklist();
	listappend(vm, rl, mkvallist(r));
	listappend(vm, rl, mkvallist(w));
	listappend(vm, rl, mkvallist(e));
	*rv = mkvallist(rl);
}

void
fnio(Env env)
{
	FN(access);
	FN(_ioctl);
	FN(issysfd);
	FN(_mapfd);
	FN(mksysfd);
	FN(_munmap);
	FN(_open);
	FN(_popen);
	FN(read);
	FN(_recvfd);
	FN(seek);
	FN(select);
	FN(_socket);
	FN(_sockpair);
	FN(stat);
	FN(sysfdno);
	FN(write);
}
