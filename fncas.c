#include "sys.h"
#include "util.h"
#include "syscqct.h"


enum {
	NWAY = 4,
	NBITS = 12,
};

static Str* cache[NWAY];
static unsigned idx[NWAY];
static Imm nhit, nmiss, nspan;

/* not right for dispatch calls */
static void
callmethod(VM *vm, Val this, Val id, Imm argc, Val *argv, Val *rv)
{
	Val v, xargv[4];
	As *tas;
	Closure *cl;
	Imm xargc;

	if(this->qkind != Qas)
		vmerr(vm, "cas: callmethod botch");
	tas = valas(this);
	if(id->qkind != Qstr)
		vmerr(vm, "cas: callmethod botch");
	v = tabget(tas->mtab, id);
	if(v == 0)
		vmerr(vm, "cas: callmethod botch");
	cl = valcl(v);
	if(argc > 4)
		vmerr(vm, "cas: callmethod botch");
	xargc = argc;
	memcpy(xargv+1, argv+1, (argc-1)*sizeof(Val));
	xargv[0] = this;
	*rv = dovm(vm, cl, xargc, xargv);
}

static void
casget(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Val vr;
	Range *r;
	Cval *beg, *len;
	unsigned i;
	Imm a;
	Str *s;

	checkarg(vm, "casget", argv, 1, Qrange);
	r = valrange(argv[1]);
	beg = r->beg;
	len = r->len;
	a = beg->val>>NBITS;
	if(((beg->val+len->val)>>NBITS) > a){
		/* spans cache boundary...just read directly */
		nspan++;
		callmethod(vm, disp[0], mkvalstr(mkstr0("get")),
			   argc, argv, rv);
		return;
	}
	for(i = 0; i < NWAY; i++){
		if(idx[i] == a && cache[i]){
			nhit++;
			goto havei;
		}
	}
	vr = argv[1];
	gcprotect(vm, vr);
	argv[1] = mkvalrange(mkcval(vm->litdom, vm->litns->base[Vptr],
				    a<<NBITS),
			     mkcval(vm->litdom, vm->litns->base[Vptr],
				    1<<NBITS));
	callmethod(vm, disp[0], mkvalstr(mkstr0("get")), argc, argv, rv);
	argv[1] = vr;
	gcunprotect(vm, vr);
	i = rand()%NWAY;
	if(cache[i])
		gcunprotect(vm, mkvalstr(cache[i]));
	gcprotect(vm, *rv);
	s = valstr(*rv);
	cache[i] = s;
	idx[i] = a;
	nmiss++;
havei:
	a<<= NBITS;
	*rv = mkvalstr(strslice(cache[i], beg->val-a, beg->val-a+len->val));
}

static void
casput(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	callmethod(vm, disp[0], mkvalstr(mkstr0("put")), argc, argv, rv);
}

static void
casmap(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	callmethod(vm, disp[0], mkvalstr(mkstr0("map")), argc, argv, rv);
}

static void
casdispatch(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	fatal("casdispatch not implemented");
}

static As*
mkcas(As *tas)
{
	Tab *mtab;
	mtab = mktab();
	_tabput(mtab, mkvalstr(mkstr0("get")),
		mkvalcl(mkccl("casget", casget, 1, mkvalas(tas))));
	_tabput(mtab, mkvalstr(mkstr0("put")),
		mkvalcl(mkccl("casget", casput, 1, mkvalas(tas))));
	_tabput(mtab, mkvalstr(mkstr0("map")),
		mkvalcl(mkccl("casget", casmap, 1, mkvalas(tas))));
	_tabput(mtab, mkvalstr(mkstr0("dispatch")),
		mkvalcl(mkccl("casget", casdispatch, 1, mkvalas(tas))));
	return mkastab(mtab, 0);
}

static void
l1_mkcas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkcas");
	checkarg(vm, "mkcas", argv, 0, Qas);
	as = mkcas(valas(argv[0]));
	*rv = mkvalas(as);
}

static void
l1_casstat(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	l = mklist();
	listappend(vm, l, mkvallitcval(Vuvlong, nhit));
	listappend(vm, l, mkvallitcval(Vuvlong, nmiss));
	listappend(vm, l, mkvallitcval(Vuvlong, nspan));
	*rv = mkvallist(l);
}

void
fncas(Env *env)
{
	FN(casstat);
	FN(mkcas);
}
