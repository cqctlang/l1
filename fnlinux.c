#include "sys.h"
#include "util.h"
#include "syscqct.h"

#include <sys/timerfd.h>

void
l1_timerfd_create(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cl, *fl;
	Fd *fd;
	Xfd xfd;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to timerfd_create");
	checkarg(vm, argv, 0, Qcval);
	checkarg(vm, argv, 1, Qcval);
	cl = valcval(argv[0]);
	fl = valcval(argv[1]);
	xfd.fd = timerfd_create(cl->val, fl->val);
	if(0 > xfd.fd)
		vmerr(vm, "timerfd_create: %s", strerror(errno));
	xfd.read = xfdread;
	xfd.write = xfdwrite;
	xfd.close = xfdclose;
	fd = mkfdfn(mkstr0("timerfd"), Fread, &xfd);
	*rv = mkvalfd(fd);
}

void
l1_timerfd_settime(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Val v;
	Cval *fl, *cv;
	List *l;
	struct itimerspec it;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to timerfd_settime");
	checkarg(vm, argv, 0, Qfd);
	checkarg(vm, argv, 1, Qcval);
	checkarg(vm, argv, 2, Qlist);
	fd = valfd(argv[0]);
	fl = valcval(argv[1]);
	l = vallist(argv[2]);

	if((fd->flags&Ffn) == 0)
		vmerr(vm, "bad file descriptor");
	if(listlen(l) != 4)
		vmerr(vm, "bad timer specifier");
	
	v = listref(l, 0);
	if(Vkind(v) != Qcval)
		vmerr(vm, "bad timer specifier");
	cv = valcval(v);
	it.it_interval.tv_sec = cv->val;
	v = listref(l, 1);
	if(Vkind(v) != Qcval)
		vmerr(vm, "bad timer specifier");
	cv = valcval(v);
	it.it_interval.tv_nsec = cv->val;
	v = listref(l, 2);
	if(Vkind(v) != Qcval)
		vmerr(vm, "bad timer specifier");
	cv = valcval(v);
	it.it_value.tv_sec = cv->val;
	v = listref(l, 3);
	if(Vkind(v) != Qcval)
		vmerr(vm, "bad timer specifier");
	cv = valcval(v);
	it.it_value.tv_nsec = cv->val;

	if(0 > timerfd_settime(fd->u.fn.fd, fl->val, &it, 0))	       
		vmerr(vm, "timerfd_settime: %s", strerror(errno));
}

void
fnlinux(Env *env)
{
	FN(timerfd_create);
	FN(timerfd_settime);
}
