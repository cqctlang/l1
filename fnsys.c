#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum {
	Maxtrace = 1000000,	/* Trace records while profiler engaged  */
	Maxdefer = 1000000,	/* Deferred closure and code garbage */
};

typedef
struct Trace {
	Imm pc;
	Closure *cl;
} Trace;

typedef
struct Profiler
{
	Trace *trace;
	u32 ntrace;
	Head **defer;
	u32 ndefer;
	Freeheadfn freecl, freecode;
} Profiler;

static Profiler *prof;
static void	finiprof();


static void
l1_getpid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	*rv = mkvallitcval(Vulong, getpid());
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
	*rv = mkvallitcval(Vulong, tod);
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
	*rv = mkvallitcval(Vulong, r);
}

static void
profsample(VM *vm)
{
	Trace *tr;
	if(prof->ntrace >= Maxtrace){
		xprintf("profile: trace buffer exhausted, "
			"profile suspended\n");
		setproftimer(0, 0);
		return;
	}
	tr = &prof->trace[prof->ntrace++];
	tr->pc = vm->pc;
	/* we assume that pc is on next insn, but it may not be if profiler
	   interrupted the insn fetch!  we catch only the easy, necessary
	   case of pc==0 */
	if(tr->pc != 0)
		tr->pc -= 1;
	tr->cl = vm->clx;
}

static Tab*
dotrs(VM *vm)
{
	char buf[256];
	Imm pc;
	Closure *cl;
	Code *code;
	Val k, v;
	Cval *cv;
	Trace *tr;
	Ctl **ls;
	Tab *tab;
	
	tab = mktab();
	tr = prof->trace;
	while(tr < prof->trace+prof->ntrace){
		pc = tr->pc;
		cl = tr->cl;
		tr++;
		code = cl->code;
		if(cl->cfn || cl->ccl)
			snprintf(buf, sizeof(buf), "%30s\t(builtin %s)",
				 cl->id, cl->cfn ? "function" : "closure");
		else{
			while(1){
				ls = code->labels;
				if(ls[pc] && ls[pc]->src){
					snprintf(buf, sizeof(buf),
						 "%30s\t(%s:%u)", cl->id,
						 ls[pc]->src->filename,
						 ls[pc]->src->line);
					break;
				}
				if(pc == 0){
					snprintf(buf, sizeof(buf),
						 "%30s", "(unknown code)");
					break;
				}
				pc--;
			}
		}
		k = mkvalstr(mkstr0(buf));
		v = tabget(tab, k);
		if(v){
			cv = valcval(v);
			v = mkvallitcval(Vuvlong, 1+cv->val);
		}else
			v = mkvallitcval(Vuvlong, 1);
		tabput(vm, tab, k, v);
	}
	xprintf("profile: %d samples\n", prof->ntrace);
	return tab;
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
deferfree(Head *hd)
{
	if(prof->ndefer >= Maxdefer-1){
		prof->defer[prof->ndefer++] = hd;
		setproftimer(0, 0);
		finiprof();
		xprintf("profile: deferral buffer exhausted, "
			"profile aborted\n");
		return 0;
	}
	prof->defer[prof->ndefer++] = hd;
	return 0;
}

static void
initprof()
{
	prof = xmalloc(sizeof(Profiler));
	prof->trace = xmalloc(Maxtrace*sizeof(Trace));
	prof->defer = xmalloc(Maxdefer*sizeof(Head*));
	prof->freecl = getfreeheadfn(Qcl);
	prof->freecode = getfreeheadfn(Qcode);
	setfreeheadfn(Qcl, deferfree);
	setfreeheadfn(Qcode, deferfree);
}

static void
finiprof()
{
	Head *hd;

	while(prof->ndefer-- > 0){
		hd = prof->defer[prof->ndefer];
		switch(hd->qkind){
		case Qcl:
			prof->freecl(hd);
			break;
		case Qcode:
			prof->freecode(hd);
			break;
		default:
			fatal("unexpected object in deferral buffer");
		}
		heapfree(hd);
	}
	setfreeheadfn(Qcl, prof->freecl);
	setfreeheadfn(Qcode, prof->freecode);
	xfree(prof->trace);
	xfree(prof->defer);
	xfree(prof);
	prof = 0;
}

static void
l1_profon(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to profon");
	if(prof)
		return;
	initprof();
	setproftimer(1000, doprof);
}

static void
l1_profoff(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to profoff");
	if(prof == 0)
		return;
	setproftimer(0, 0);
	*rv = mkvaltab(dotrs(vm));
	finiprof();
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
