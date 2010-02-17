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

static void
casget(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
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
		*rv = mkvalstr(callget(vm, valas(disp[0]), beg->val, len->val));
		return;
	}
	for(i = 0; i < NWAY; i++){
		if(idx[i] == a && cache[i]){
			nhit++;
			goto havei;
		}
	}
	s = callget(vm, valas(disp[0]), a<<NBITS, 1<<NBITS);
	i = rand()%NWAY;
	if(cache[i])
		gcunpersist(vm, mkvalstr(cache[i]));
	gcpersist(vm, s);
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
	Range *r;
	Cval *beg, *len;

	r = valrange(argv[1]);
	beg = r->beg;
	len = r->len;
	callput(vm, valas(disp[0]), beg->val, len->val, valstr(argv[2]));
}

static void
casmap(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	*rv = mkvalvec(callmap(vm, valas(disp[0])));
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
	tabput(mtab, mkvalstr(mkstr0("get")),
	       mkvalcl(mkccl("casget", casget, 1, mkvalas(tas))));
	tabput(mtab, mkvalstr(mkstr0("put")),
	       mkvalcl(mkccl("casget", casput, 1, mkvalas(tas))));
	tabput(mtab, mkvalstr(mkstr0("map")),
	       mkvalcl(mkccl("casget", casmap, 1, mkvalas(tas))));
	tabput(mtab, mkvalstr(mkstr0("dispatch")),
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
