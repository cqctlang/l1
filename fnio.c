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
l1__recvfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int rfd;
	int fd;
	struct cmsghdr *cmsg;
	struct msghdr msg;
	struct iovec iv;
	unsigned char byte;
	char buf[CMSG_SPACE(sizeof(rfd))];

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to recvfd");
	checkarg(vm, argv, 0, Qcval);
	fd = cvalu(valcval(argv[0]));
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
	if(Vkind(argv[argc-1]) == Qcval){
		cv = valcval(argv[argc-1]);
		flags = cvalu(cv);
		argc--;
		if(argc == 0)
			vmerr(vm, "wrong number of arguments to popen");
	}else if(Vkind(argv[argc-1]) != Qstr)
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

void
fnio(Env env)
{
	FN(_popen);
	FN(_recvfd);
	FN(stat);
}
