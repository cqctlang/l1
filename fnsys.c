#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Tab *prof;		/* FIXME: gc vulnerable with multiple VMs */

static void
l1_getpid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	*rv = mkvallitcval(vm, Vulong, getpid());
}

static void
l1_gettimeofday(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Imm tod;
	struct timeval tv;

	gettimeofday(&tv, 0);
	tod = tv.tv_sec;
	tod *= 1000000;
	tod += tv.tv_usec;
	*rv = mkvallitcval(vm, Vulong, tod);
}

static void
l1_randseed(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to randseed");
	arg0 = argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	srand((unsigned int)cv->val);
}

static void
l1_rand(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val arg0;
	Imm r;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rand");
	arg0 = argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	if(cv->val > RAND_MAX)
		vmerr(vm, "operand to rand exceeds RAND_MAX (%d)", RAND_MAX);
	if(cv->val == 0)
		vmerr(vm, "operand to rand must be positive");
	
	r = rand();
	r %= cv->val;
	*rv = mkvallitcval(vm, Vulong, r);
}

enum {
	Maxtr = 1000000,
};
typedef
struct Tr {
	Imm pc;
	Closure *cl;
} Tr;
static Tr trs[Maxtr];
static u32 ntr;

static void
profsample(VM *vm)
{
	struct Tr *tr;
	if(ntr >= Maxtr){
		xprintf("profile trace buffer exhausted\n");
		return;
	}
	tr = &trs[ntr++];
	tr->pc = vm->pc-1;
	tr->cl = vm->clx;
}

static void
dotrs(VM *vm)
{
	char buf[256];
	Imm pc;
	Closure *cl;
	Code *code;
	Val k, v;
	Cval *cv;
	struct Tr *tr;
	Ctl **ls;

	tr = trs;
	while(tr < trs+ntr){
		pc = tr->pc;		/* vm loop increments pc after fetch */
		cl = tr->cl;
		tr++;
		code = cl->code;
		if(cl->cfn || cl->ccl)
			snprintf(buf, sizeof(buf), "%20s\t(builtin %s)", cl->id,
				 cl->cfn ? "function" : "closure");
		else
			while(1){
				ls = code->labels;
				if(ls[pc] && ls[pc]->src){
					snprintf(buf, sizeof(buf),
						 "%20s\t(%s:%u)", cl->id,
						 ls[pc]->src->filename,
						 ls[pc]->src->line);
					break;
				}
				if(pc == 0){
					snprintf(buf, sizeof(buf),
						 "(unknown code)");
					break;
				}
				pc--;
			}
		k = mkvalstr(mkstr0(buf));
		v = tabget(prof, k);
		if(v){
			cv = valcval(v);
			v = mklitcval(Vuvlong, 1+cv->val);
		}else
			v = mklitcval(Vuvlong, 1);
		tabput(vm, prof, k, v);
	}
}

static void
doprof()
{
	VM **vmp, *vm;

	vmp = vms;
	while(vmp < vms+Maxvms){
		vm = *vmp++;
		if(vm == 0)
			continue;
		profsample(vm);
	}
}

static int
dontfree(Head *hd)
{
	return 0;
}

static void
l1_profoff(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to profon");
	if(prof == 0)
		return;
	dotrs(vm);
	*rv = mkvaltab(prof);
	vm->prof = 0;		/* gc rootset */
	prof = 0;
	setproftimer(0, 0);
}

static void
l1_profon(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to profon");
	if(prof)
		return;
	setheapfree(Qcode, dontfree);
	setheapfree(Qcl, dontfree);
	prof = mktab();
	ntr = 0;
	vm->prof = prof;
	setproftimer(1000, doprof);
}

void
fnsys(Env *env)
{
	FN(getpid);
	FN(gettimeofday);
	FN(profoff);
	FN(profon);
	FN(rand);
	FN(randseed);
}
