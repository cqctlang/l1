#include "sys.h"
#include "util.h"
#include "syscqct.h"

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
	if(0 > recvmsg(fd, &msg, 0)) {
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
	if(Vkind(argv[argc-1]) == Qcval) {
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
}
