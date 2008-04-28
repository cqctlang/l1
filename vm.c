#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

typedef
enum {
	Qundef = 0,
	Qnil,
	Qnulllist,
	Qcval,
	Qcl,
	Qbox,
	Qpair,
	Qstr,
} Qkind;

typedef struct Vimm Vimm;
typedef struct Vcval Vcval;
typedef struct Box Box;
typedef struct Pair Pair;
typedef struct Str Str;
typedef struct Vec Vec;

struct Val {
	Qkind qkind;
	union {
		Head *hd;
		Imm imm;
		Cval cval;
		Closure *cl;
		Box *box;
		Pair *pair;
		Str *str;
		Vec *vec;
	} u;
};

struct Env {
	HT *ht;
};

struct Closure {
	Head hd;
	Code *code;
	unsigned long entry;
	unsigned dlen;
	Val *display;
	char *id;
	Imm fp;			/* of continuation */
};

struct Box {
	Head hd;
	Val v;
};

struct Vimm {
	Head hd;
	Imm imm;
};

struct Vcval {
	Head hd;
	Cval cval;
};

struct Pair {
	Head hd;
	Val car;
	Val cdr;
};

struct Str {
	Head hd;
	unsigned long len;
	char *s;
};

struct Vec {
	Head hd;
	unsigned long len;
	Val *vec;
};

struct VM {
	Val stack[Maxstk];
	Env *topenv;
	Imm sp, fp, pc;
	Closure *clx;
	Insn *ibuf;
	Val ac, cl;
	unsigned char gcpause, gcrun;
	int cm, cgc;
	pthread_t t;
	jmp_buf esc;
};

static void freeval(Val *val);
static void vmsetcl(VM *vm, Closure *cl);
static void strinit(Str *str, Lits *lits);

static Val Xundef;
static Val Xnil;
static Val Xnulllist;
static unsigned long long tick;
static unsigned long gcepoch = 2;

static Head eol;		/* end-of-list sentinel */

static char *opstr[Iopmax] = {
	[Iadd] = "+",
	[Iand] = "&",
	[Idiv] = "/",
	[Imod] = "%",
	[Imul] = "*",
	[Ior] = "|",
	[Ishl] = "<<",
	[Ishr] = ">>",
	[Isub] = "-",
	[Icmpeq] = "==",
	[Icmpneq] = "!=",
	[Icmpgt] = ">",
	[Icmpge] = ">=",
	[Icmplt] = "<",
	[Icmple] = "<=",
};

typedef
struct Rootset {
	Head *roots;
	Head *last;
	Head *before_last;
	Head *this;
	Head *(*getlink)(Head *);
	Head *(*getolink)(Head *);
	void (*setlink)(Head *, Head *);
} Rootset;

static Rootset roots;
static Rootset stores;

#define GCCOLOR(i) ((i)%3)
enum {
	GCfree = 3,
	GCrate = 1000,
};

static unsigned long long nextgctick = GCrate;

Heap heapcode, heapcl, heapimm, heapbox, heapcval, heappair, heapstr;
static Code *kcode;

static void*
read_and_clear(void *pp)
{
	void **p = (void**)pp;
	void *q;
	q = 0;
	asm("xchg %0,%1" : "=r"(q), "=m"(*p) : "0"(q), "m"(*p) : "memory");
	return q;
}

static void
writebarrier()
{
	/* force previous writes to be visible to other processors
	   before subsequent ones. */
	/* on contemporary x86 (including core 2 duo),
	   processor-ordering memory model precludes
	   need for explict barrier such as sfence.  if
	   the underlying memory were in WC mode (see
	   intel vol 3, chapter 10), things would be different. */
}

Head*
galloc(Heap *heap)
{
	Head *o, *ap, *fp;
	unsigned m;

retry:
	if(heap->free){
		o = heap->free;
		heap->free = o->link;
		if(o->link == &eol)
			fatal("o->link == &eol (1)");
		o->link = 0;
		if(o->state != -1)
			fatal("galloc bad state %d", o->state);
		o->state = 0;
	}else if(heap->swept){
		heap->free = (Head*)read_and_clear(&heap->swept);
		goto retry;
	}else{
		ap = heap->alloc;
		fp = 0;
		for(m = 0; m < AllocBatch; m++){
			o = xmalloc(heap->sz);
			o->heap = heap;
			o->alink = ap;
			o->link = fp;
			if(o->link == &eol)
				fatal("o->link == &eol (2)");
			o->color = GCfree;
			o->state = -1;
			ap = o;
			fp = o;
		}
		heap->alloc = o;
		heap->free = o;
		goto retry;
	}
	
	o->color = GCCOLOR(gcepoch);

	return o;
}

static void
sweepheap(Heap *heap, unsigned color)
{
	Head *p;
	p = heap->alloc;
	while(p){
		if(p->color == color){
			if(heap->free1)
				heap->free1(p);
			if(p->state != 0)
				fatal("sweep heap bad state %d", p->state);
			p->link = heap->sweep;
			p->slink = 0;
			p->state = -1;
			heap->sweep = p;
			p->color = GCfree;
		}
		p = p->alink;
	}
	if(heap->swept == 0){
		heap->swept = heap->sweep;
		heap->sweep = 0;
	}
}

static void
sweep(unsigned color)
{
	sweepheap(&heapcode, color);
	sweepheap(&heapcl, color);
	sweepheap(&heapimm, color);
	sweepheap(&heapbox, color);
	sweepheap(&heapcval, color);
	sweepheap(&heapstr, color);
}

static void
freeheap(Heap *heap)
{
	Head *p, *q;
	p = heap->alloc;
	while(p){
		q = p->alink;
		free(p);
		p = q;
	}
}

static Head*
valhead(Val *v)
{
	switch(v->qkind){
	case Qundef:
	case Qnil:
	case Qnulllist:
	case Qcval:
		return 0;
		break;
	default:
		return v->u.hd;
		break;
	}
}

/* called on roots by marker.  called on stores by mutator. */
static void
addroot(Rootset *rs, Head *h)
{
	int x;

	if(h == 0)
		return;
	if(rs->getlink(h))
		return;		/* already on rootlist */
	x = h->state;
	if(x > 2 || x < 0)
		fatal("addroot bad state %d", x);
	h->state++;
	rs->setlink(h, rs->roots);
	writebarrier();
	rs->roots = h;
}

static Head*
removeroot(Rootset *rs)
{
	Head *h;
	int x;

	if(rs->this == rs->before_last){
		rs->before_last = rs->last;
		rs->this = rs->last = rs->roots;
	}
	if(rs->this == rs->before_last)
		return 0;
	h = rs->this;
	rs->this = rs->getlink(h);
	x = h->state;
	if(x > 2 || x <= 0)
		fatal("remove root bad state %d", x);
	h->state--;
	rs->setlink(h, 0);
	return h;
}

static int
rootsetempty(Rootset *rs)
{
	return rs->before_last == rs->roots;
}

static Head*
getrootslink(Head *h)
{
	return h->link;
}

static Head*
getstoreslink(Head *h)
{
	return h->slink;
}

static void
setrootslink(Head *h, Head *v)
{
	h->link = v;
}

static void
setstoreslink(Head *h, Head *v)
{
	h->slink = v;
}

static void
markhead(Head *hd, unsigned color)
{
	Ictx ictx;
	Head *c;

	if(hd == 0)
		return;

	if(hd->color == color)
		return;

	hd->color = color;
	if(hd->heap->iter == 0)
		return;
	memset(&ictx, 0, sizeof(ictx));
	while(1){
		c = hd->heap->iter(hd, &ictx);
		if(c == 0)
			break;
		if(c->color != color)
			addroot(&roots, c);
	}
}

static void
markrs(Rootset *rs, unsigned color)
{
	Head *h;

	while(1){
		h = removeroot(rs);
		if(h == 0)
			break;
		markhead(h, color);
	}
}

static void
mark(unsigned color)
{
	markrs(&stores, GCCOLOR(gcepoch));
	markrs(&roots, GCCOLOR(gcepoch));
}

static void
bindingroot(void *u, char *k, void *v)
{
	addroot(&roots, valhead((Val*)v));
}

static void
rootset(VM *vm)
{
	unsigned m;

	/* never collect these things */
	addroot(&roots, (Head*)kcode); 

	if(vm == 0)
		return;

	for(m = vm->sp; m < Maxstk; m++)
		addroot(&roots, valhead(&vm->stack[m]));
	hforeach(vm->topenv->ht, bindingroot, 0);
	addroot(&roots, valhead(&vm->ac));
	addroot(&roots, valhead(&vm->cl));
}

static void
rootsetreset(Rootset *rs)
{
	rs->roots = &eol;
	rs->last = &eol;
	rs->before_last = &eol;
	rs->this = &eol;
}

static void
gcreset()
{
	rootsetreset(&roots);
	rootsetreset(&stores);
}

static void
gc(VM *vm)
{
	rootset(vm);
	gcepoch++;
	mark(GCCOLOR(gcepoch));
	sweep(GCCOLOR(gcepoch-2));
	while(!rootsetempty(&stores))
		mark(GCCOLOR(gcepoch));
	gcreset();
}

enum {
	GCdie = 0x1,
	GCdied = 0x2,
	GCpaused = 0x4,
	GCresume = 0x8,
	GCrun = 0x10,
	GCrunning = 0x20,
};

static int
waitmutator(VM *vm)
{
	char b;
	
	vm->gcpause = 1;
	if(0 > read(vm->cgc, &b, 1))
		fatal("gc synchronization failure"); 
	if(b == GCdie)
		return 1;
	if(b != GCpaused)
		fatal("gc protocol botch");
	return 0;
}

static void
resumemutator(VM *vm)
{
	char b;

	vm->gcpause = 0;
	b = GCresume;
	if(0 > write(vm->cgc, &b, 1))
		fatal("gc synchronization failure");
}

static void
waitgcrun(VM *vm)
{
	char b;
	
	if(0 > read(vm->cgc, &b, 1))
		fatal("gc synchronization failure"); 
	if(b == GCdie){
		b = GCdied;
		if(0 > write(vm->cgc, &b, 1))
			fatal("gc sychronization failure");
		pthread_exit(0);
	}
	if(b != GCrun)
		fatal("gc protocol botch");

	gcreset();
	rootset(vm);
	gcepoch++;
	vm->gcrun = 1;
	b = GCrunning;
	if(0 > write(vm->cgc, &b, 1))
		fatal("gc synchronization failure");

}

static void
gcsync(int fd, char t, char r)
{
	char b;
	if(0 > write(fd, &t, 1))
		fatal("gc synchronization failure");
	if(0 > read(fd, &b, 1))
		fatal("gc synchronization failure");
	if(b != r)
		fatal("gc protocol botch");
}

static int
needsgc()
{
	if(tick >= nextgctick){
		nextgctick = tick+GCrate;
		return 1;
	}else
		return 0;
}

static void
gcpoll(VM *vm)
{
	if(vm->gcpause)
		gcsync(vm->cm, GCpaused, GCresume);
	else if(vm->gcrun)
		return;
	else if(needsgc())
		gcsync(vm->cm, GCrun, GCrunning);
}

static void
gckill(VM *vm)
{
	gcsync(vm->cm, GCdie, GCdied);
	close(vm->cm);
	close(vm->cgc);
	pthread_join(vm->t, 0);
}

#if 0
static int
#else
static void*
#endif
gcchild(void *p)
{
	VM *vm = (VM*)p;
	int die = 0;
	char b;

	while(!die){
		waitgcrun(vm);
		mark(GCCOLOR(gcepoch));
		sweep(GCCOLOR(gcepoch-2));
		die = waitmutator(vm);
		while(!rootsetempty(&stores)){
			if(!die)
				resumemutator(vm);
			mark(GCCOLOR(gcepoch));
			if(!die)
				die = waitmutator(vm);
		}
		vm->gcrun = 0;
		if(!die)
			resumemutator(vm);
	}

	b = GCdied;
	if(0 > write(vm->cgc, &b, 1))
		fatal("gc sychronization failure");
	pthread_exit(0);
	return 0;
}

static void
newchan(int *left, int *right)
{
	int fd[2];

	if(0 > socketpair(PF_UNIX, SOCK_STREAM, 0, fd))
		fatal("cannot allocate channel");
	*left = fd[0];
	*right = fd[1];
}

static void
concurrentgc(VM *vm)
{
	newchan(&vm->cm, &vm->cgc);
	vm->gcpause = 0;

	if(0 > pthread_create(&vm->t, 0, gcchild, vm))
		fatal("pthread create failed");
}

static Head*
iterbox(Head *hd, Ictx *ictx)
{
	Box *box;
	box = (Box*)hd;
	if(ictx->n > 0)
		return 0;
	ictx->n = 1;
	return valhead(&box->v);
}

static Head*
itercl(Head *hd, Ictx *ictx)
{
	Closure *cl;
	cl = (Closure*)hd;
	if(ictx->n > cl->dlen)
		return 0;
	if(ictx->n == cl->dlen){
		ictx->n++;
		return (Head*)cl->code;
	}
	return valhead(&cl->display[ictx->n++]);
}

static Head*
iterpair(Head *hd, Ictx *ictx)
{
	Pair *pair;
	pair = (Pair*)hd;

	switch(ictx->n++){
	case 0:
		return valhead(&pair->car);
	case 1:
		return valhead(&pair->cdr);
	default:
		return 0;
	}
}

Closure*
mkcl(Code *code, unsigned long entry, unsigned len, char *id)
{
	Closure *cl;
	cl = (Closure*)galloc(&heapcl);
	cl->code = code;
	cl->entry = entry;
	cl->dlen = len;
	cl->display = xmalloc(cl->dlen*sizeof(Val));
	cl->id = xstrdup(id);
	return cl;
}

static void
freecl(Head *hd)
{
	Closure *cl;
	cl = (Closure*)hd;
	free(cl->display);
	free(cl->id);
}

static void
strinit(Str *str, Lits *lits)
{
	str->len = lits->len;
	str->s = xmalloc(str->len);
	memcpy(str->s, lits->s, str->len);
}

static void
freestr(Head *hd)
{
	Str *str;
	str = (Str*)hd;
	free(str->s);
}

Env*
mkenv()
{
	Env *env;
	env = xmalloc(sizeof(Env));
	env->ht = mkht();
	return env;
}

Val*
envgetbind(Env *env, char *id)
{
	Val *v;

	v = hget(env->ht, id);
	if(!v){
		v = xmalloc(sizeof(Val));
		hput(env->ht, id, v);
	}
	return v;
}

static void
envbind(Env *env, char *id, Val *val)
{
	Val *v;

	v = envgetbind(env, id);
	*v = *val;
}

static int
envlookup(Env *env, char *id, Val *val)
{
	Val *v;

	v = envgetbind(env, id);
	if(v->qkind == Qundef)
		return 0;
	*val = *v;
	return 1;
}

static void
freebinding(void *u, char *id, void *v)
{
	Val *val;
	val = (Val*)v;
	freeval(val);
}

void
freeenv(Env *env)
{
	hforeach(env->ht, freebinding, 0);
	freeht(env->ht);
	free(env);
}

static void
mkvalimm(Imm imm, Val *vp)
{
	vp->qkind = Qcval;
	initcval(&vp->u.cval, 0, imm);
}

static void
mkvalcval(Type *t, Imm imm, Val *vp)
{
	vp->qkind = Qcval;
	initcval(&vp->u.cval, t, imm);
}

static void
mkvalcval2(Cval *cv, Val *vp)
{
	vp->qkind = Qcval;
	vp->u.cval = *cv;
}

static void
mkvalcl(Closure *cl, Val *vp)
{
	vp->qkind = Qcl;
	vp->u.cl = cl;
}

static void
mkvalbox(Val *boxed, Val *vp)
{
	Box *box;
	box = (Box*)galloc(&heapbox);
	box->v = *boxed;
	vp->qkind = Qbox;
	vp->u.box = box;
}

static void
mkvalpair(Val *car, Val *cdr, Val *vp)
{
	Pair *pair;
	pair = (Pair*)galloc(&heappair);
	pair->car = *car;
	pair->cdr = *cdr;
	vp->qkind = Qpair;
	vp->u.pair = pair;
}

static Imm
valimm(Val *v)
{
	if(v->qkind != Qcval)
		fatal("valimm on non-cval");
	return v->u.cval.val;
}

static Cval*
valcval(Val *v)
{
	if(v->qkind != Qcval)
		fatal("valcval on non-cval");
	return &v->u.cval;
}

static Closure*
valcl(Val *v)
{
	if(v->qkind != Qcl)
		fatal("valcl on non-closure");
	return v->u.cl;
}

static Pair*
valpair(Val *v)
{
	if(v->qkind != Qpair)
		fatal("valpair on non-pair");
	return v->u.pair;
}

static Str*
valstr(Val *v)
{
	if(v->qkind != Qstr)
		fatal("valstr on non-string");
	return v->u.str;
}

static void
valboxed(Val *v, Val *dst)
{
	if(v->qkind != Qbox)
		fatal("valboxed on non-box");
	*dst = v->u.box->v;
}

static Cval*
valboxedcval(Val *v)
{
	return &v->u.box->v.u.cval;
}

static int
zeroval(Val *v)
{
	Cval *cv;

	switch(v->qkind){
	case Qcval:
		cv = valcval(v);
		return cv->val == 0;
	default:
		fatal("branch on non-integer value");
		return 0;
	}
}

static void
freeval(Val *v)
{
	free(v);
}

static void
putbox(VM *vm, Val *box, Val *boxed)
{
	if(box->qkind != Qbox)
		fatal("putbox on non-box");
	if(boxed->qkind == Qbox)
		fatal("boxing boxes is insane");
	if(vm->gcrun)
		addroot(&stores, valhead(&box->u.box->v));
	box->u.box->v = *boxed;
}

static void
putval(VM *vm, Val *v, Location *loc)
{
	Val *dst;

	switch(loc->kind){
	case Lreg:
		switch(loc->idx){
		case Rac:
			vm->ac = *v;
			break;
		case Rsp:
			vm->sp = valimm(v);
			break;
		case Rfp:
			vm->fp = valimm(v);
			break;
		case Rpc:
			vm->pc = valimm(v);
			break;
		case Rcl:
			vm->cl = *v;
			vmsetcl(vm, valcl(&vm->cl));
			break;
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		dst = &vm->stack[(vm->fp+1)+loc->idx];
		if(loc->indirect)
			putbox(vm, dst, v);
		else
			*dst = *v;
		break;
	case Llocal:
		dst = &vm->stack[(vm->fp-1)-loc->idx];
		if(loc->indirect)
			putbox(vm, dst, v);
		else
			*dst = *v;
		break;
	case Ldisp:
		dst = &vm->clx->display[loc->idx];
		if(loc->indirect)
			putbox(vm, dst, v);
		else
			*dst = *v;
		break;
	case Ltopl:
		dst = loc->val;
		*dst = *v;
		break;
	default:
		fatal("bug");
	}
}

static void
printsrc(Closure *cl, Imm pc)
{
	Code *code;
	
	code = cl->code;
	while(pc >= 0){
		if(code->labels[pc] && code->labels[pc]->src){
			printf("%s:%u\n",
			       code->labels[pc]->src->filename,
			       code->labels[pc]->src->line);
			return;
		}
		pc--;
	}
	printf("(no source information)\n");
}

static void
vmerr(VM *vm, char *fmt, ...)
{
	va_list args;
	Imm pc, fp, narg;
	Closure *cl;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	fflush(stderr);
	
	/* dump stack trace */
	pc = vm->pc-1;		/* vm loop increments pc after fetch */
	fp = vm->fp;
	cl = vm->clx;
	while(fp != 0){
		printsrc(cl, pc);
		narg = valimm(&vm->stack[fp]);
		pc = valimm(&vm->stack[fp+narg+1]);
		pc--;		/* return address is insn following call */
		cl = valcl(&vm->stack[fp+narg+2]);
		fp = valimm(&vm->stack[fp+narg+3]);
	}

	longjmp(vm->esc, 1);
}

static void
getval(VM *vm, Location *loc, Val *vp)
{
	Val *p;

	switch(loc->kind){
	case Lreg:
		switch(loc->idx){
		case Rac:
			*vp = vm->ac;
			return;
		case Rsp:
			mkvalimm(vm->sp, vp);
			return;
		case Rfp:
			mkvalimm(vm->fp, vp);
			return;
		case Rpc:
			mkvalimm(vm->pc, vp);
			return;
		case Rcl:
			*vp = vm->cl;
			return;
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		p = &vm->stack[(vm->fp+1)+loc->idx];
		if(loc->indirect)
			valboxed(p, vp);
		else
			*vp = *p;
		return;
	case Llocal:
		p = &vm->stack[(vm->fp-1)-loc->idx];
		if(loc->indirect)
			valboxed(p, vp);
		else
			*vp = *p;
		return;
	case Ldisp:
		p = &vm->clx->display[loc->idx];
		if(loc->indirect)
			valboxed(p, vp);
		else
			*vp = *p;
		return;
	case Ltopl:
		p = loc->val;
		if(p->qkind == Qundef)
			vmerr(vm, "reference to undefined variable %s",
			      topvecid(loc->idx, vm->clx->code->topvec));
		*vp = *p;
		return;
	default:
		fatal("bug");
	}
}

static Cval*
getcval(VM *vm, Location *loc, Cval *tmp)
{
	Val *p;

	switch(loc->kind){
	case Lreg:
		switch(loc->idx){
		case Rac:
			return valcval(&vm->ac);
		case Rsp:
			initcval(tmp, 0, vm->sp);
			return tmp;
		case Rfp:
			initcval(tmp, 0, vm->fp);
			return tmp;
		case Rpc:
			initcval(tmp, 0, vm->pc);
			return tmp;
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		p = &vm->stack[(vm->fp+1)+loc->idx];
		if(loc->indirect)
			return valboxedcval(p);
		return valcval(p);
	case Llocal:
		p = &vm->stack[(vm->fp-1)-loc->idx];
		if(loc->indirect)
			return valboxedcval(p);
		return valcval(p);
	case Ldisp:
		p = &vm->clx->display[loc->idx];
		if(loc->indirect)
			return valboxedcval(p);
		return valcval(p);
	case Ltopl:
		p = loc->val;
		if(p->qkind == Qundef)
			vmerr(vm, "reference to undefined variable %s",
			      topvecid(loc->idx, vm->clx->code->topvec));
		return valcval(p);
	default:
		fatal("bug");
	}
}

static void
getvalrand(VM *vm, Operand *r, Val *vp)
{
	switch(r->okind){
	case Oloc:
		getval(vm, &r->u.loc, vp);
		break;
	case Ocval:
		mkvalcval2(&r->u.cval, vp);
		break;
	case Olits:
		vp->qkind = Qstr;
		vp->u.str = (Str*)galloc(&heapstr);
		strinit(vp->u.str, r->u.lits);
		break;
	case Onil:
		vp->qkind = Qnil;
		break;
	default:
		fatal("bug");
	}
}

static Cval*
getcvalrand(VM *vm, Operand *r, Cval *tmp)
{
	switch(r->okind){
	case Oloc:
		return getcval(vm, &r->u.loc, tmp);
	case Ocval:
		return &r->u.cval;
	default:
		fatal("bug");
	}
}

static void
putvalrand(VM *vm, Val *v, Operand *r)
{
	if(r->okind != Oloc)
		fatal("bad destination");
	putval(vm, v, &r->u.loc);
}

static void
printval(Val *v)
{
	Cval *cv;
	Closure *cl;
	Pair *pair;
	Str *str;
	Val bv;

	if(v == 0){
		printf("(no value)");
		return;
	}

	switch(v->qkind){
	case Qcval:
		cv = valcval(v);
		printf("<cval %llu>", cv->val);
		break;
	case Qcl:
		cl = valcl(v);
		if(cl->id)
			printf("<closure %s>", cl->id);
		else
			printf("<continuation %p>", cl);
		break;
	case Qundef:
		printf("<undefined>");
		break;
	case Qnil:
		printf("<nil>");
		break;
	case Qnulllist:
		printf("<null>");
		break;
	case Qbox:
		printf("<box ");
		valboxed(v, &bv);
		printval(&bv);
		printf(">");
		break;
	case Qpair:
		pair = valpair(v);
		printf("<pair %p>", pair);
		break;
	case Qstr:
		str = valstr(v);
		printf("%.*s", (int)str->len, str->s);
		break;
	default:
		printf("<unprintable type %d>", v->qkind);
		break;
	}
}

static Imm
binopimm(ikind op, Imm i1, Imm i2)
{
	switch(op){
	case Iadd:
		return i1+i2;
	case Iand:
		return i1&i2;
	case Idiv:
		return i1/i2;	/* FIXME: trap div by zero */
	case Imod:
		return i1%i2;	/* FIXME: trap div by zero */
	case Imul:
		return i1*i2;
	case Ior:
		return i1|i2;
	case Ishl:
		return i1<<i2;
	case Ishr:
		return i1>>i2;
	case Isub:
		return i1-i2;
	case Ixor:
		return i1^i2;
	case Icmpeq:
		return i1==i2 ? 1 : 0;
	case Icmpneq:
		return i1!=i2 ? 1 : 0;
	case Icmpgt:
		return i1>i2 ? 1 : 0;
	case Icmpge:
		return i1>=i2 ? 1 : 0;
	case Icmplt:
		return i1<i2 ? 1 : 0;
	case Icmple:
		return i1<=i2 ? 1 : 0;
	default:
		fatal("unsupport binary operator %d on immediates", op);
		return 0;
	}
}

static Imm
binopstr(ikind op, Str *s1, Str *s2)
{
	unsigned long len;
	int x;

	switch(op){
	case Icmpeq:
		if(s1->len != s2->len)
			return 0;
		else
			return memcmp(s1->s, s2->s, s1->len) ? 0 : 1;
	case Icmpneq:
		if(s1->len != s2->len)
			return 1;
		else
			return memcmp(s1->s, s2->s, s1->len) ? 1 : 0;
	case Icmpgt:
		len = s1->len > s2->len ? s2->len : s1->len;
		x = memcmp(s1, s2, len);
		return x > 0 ? 1 : 0;
	case Icmpge:
		len = s1->len > s2->len ? s2->len : s1->len;
		x = memcmp(s1, s2, len);
		return x >= 0 ? 1 : 0;
	case Icmplt:
		len = s1->len > s2->len ? s2->len : s1->len;
		x = memcmp(s1, s2, len);
		return x < 0 ? 1 : 0;
	case Icmple:
		len = s1->len > s2->len ? s2->len : s1->len;
		x = memcmp(s1, s2, len);
		return x <= 0 ? 1 : 0;
	default:
		fatal("unsupported binary operator %d on strings", op);
		return 0;
	}
}


static void
xunop(VM *vm, ikind op, Operand *op1, Operand *dst)
{
	Val rv;
	Cval *cv, tmp;
	Imm imm, nv;
	
	cv = getcvalrand(vm, op1, &tmp);
	imm = cv->val;

	switch(op){
	case Ineg:
		nv = -imm;
		break;
	case Iinv:
		nv = ~imm;
		break;
	case Inot:
		if(imm)
			nv = 0;
		else
			nv = 1;
		break;
	default:
		fatal("unknown unary operator %d", op);
	}

	mkvalcval(cv->type, nv, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xbinopcval(VM *vm, ikind op, Operand *op1, Operand *op2, Operand *dst)
{
	Val rv;
	Cval *cv, tmp1, tmp2;
	Imm i1, i2, nv;

	cv = getcvalrand(vm, op1, &tmp1);
	i1 = cv->val;
	cv = getcvalrand(vm, op2, &tmp2);
	i2 = cv->val;

	nv = binopimm(op, i1, i2);
	mkvalcval(cv->type, nv, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xbinop(VM *vm, ikind op, Operand *op1, Operand *op2, Operand *dst)
{
	Val v1, v2, rv;
	Cval *cv;
	Str *s1, *s2;
	Imm i1, i2, nv;

	getvalrand(vm, op1, &v1);
	getvalrand(vm, op2, &v2);
	if(v1.qkind != v2.qkind)
		vmerr(vm, "incompatible operands to binary %s", opstr[op]);

	if(v1.qkind == Qcval){
		cv = valcval(&v1);
		i1 = cv->val;
		cv = valcval(&v2);
		i2 = cv->val;
		nv = binopimm(op, i1, i2);
		mkvalcval(cv->type, nv, &rv);
		putvalrand(vm, &rv, dst);
		return;
	}

	if(v1.qkind == Qstr){
		s1 = valstr(&v1);
		s2 = valstr(&v2);
		nv = binopstr(op, s1, s2); /* assume comparison */
		mkvalcval(0, nv, &rv);
		putvalrand(vm, &rv, dst);
		return;
	}

	fatal("binop on unsupported operands");
}



static void
xclo(VM *vm, Operand *dl, Ctl *label, Operand *dst)
{
	Closure *cl;
	Val rv;
	Cval *cv, tmp;
	Imm m, len;

	/* dl is number of values to copy from stack into display */
	/* label points to instruction in current closure's code */
	/* captured variables are in display order on stack,
	   from low to high stack address */

	cv = getcvalrand(vm, dl, &tmp);
	len = cv->val;

	cl = mkcl(vm->clx->code, label->insn, len, label->label);
	for(m = 0; m < len; m++)
		cl->display[m] = vm->stack[vm->sp+m];
	vm->sp += m;

	mkvalcl(cl, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xkg(VM *vm, Operand *dst)
{
	Closure *k;
	Val rv;
	Imm len;

	len = Maxstk-vm->sp;
	k = mkcl(kcode, 0, len, 0);
	memcpy(k->display, &vm->stack[vm->sp], len*sizeof(Val));
	k->fp = vm->fp;

	mkvalcl(k, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xkp(VM *vm)
{
	Closure *k;
	k = vm->clx;
	vm->fp = k->fp;
	vm->sp = Maxstk-k->dlen;
	memcpy(&vm->stack[vm->sp], k->display, k->dlen*sizeof(Val));
}

static void
xmov(VM *vm, Operand *src, Operand *dst)
{
	Val v;
	getvalrand(vm, src, &v);
	putvalrand(vm, &v, dst);
}

static void
xjnz(VM *vm, Operand *src, Ctl *label)
{
	Cval *cv, tmp;
	cv = getcvalrand(vm, src, &tmp);
	if(cv->val != 0)
		vm->pc = label->insn;
}

static void
xjz(VM *vm, Operand *src, Ctl *label)
{
	Val v;
	getvalrand(vm, src, &v);
	if(zeroval(&v))
		vm->pc = label->insn;
}

static void
checkoverflow(VM *vm, unsigned dec)
{
	if(dec > vm->sp)
		fatal("stack overflow");
}

static void
vmpush(VM *vm, Val *v)
{
	checkoverflow(vm, 1);
	vm->stack[--vm->sp] = *v;
}

static void
vmpop(VM *vm, unsigned n)
{
	vm->sp += n;
}

static void
xpush(VM *vm, Operand *op)
{
	Val v;
	getvalrand(vm, op, &v);
	vmpush(vm, &v);
}

static void
xbox(VM *vm, Operand *op)
{
	Val v, bv;
	getvalrand(vm, op, &v);
	mkvalbox(&v, &bv);
	putvalrand(vm, &bv, op);
}

static void
xbox0(VM *vm, Operand *op)
{
	Val bv;
	mkvalbox(&Xundef, &bv);
	putvalrand(vm, &bv, op);
}

static void
xprint(VM *vm, Operand *op)
{
	Val v;
	getvalrand(vm, op, &v);
	printval(&v);
	printf("\n");
}

static void
xcar(VM *vm, Operand *op, Operand *dst)
{
	Val v;
	Pair *p;
	getvalrand(vm, op, &v);
	if(v.qkind != Qpair)
		vmerr(vm, "car on non-pair");
	p = valpair(&v);
	putvalrand(vm, &p->car, dst);
}

static void
xcdr(VM *vm, Operand *op, Operand *dst)
{
	Val v;
	Pair *p;
	getvalrand(vm, op, &v);
	if(v.qkind != Qpair)
		vmerr(vm, "cdr on non-pair");
	p = valpair(&v);
	putvalrand(vm, &p->cdr, dst);
}

static void
xcons(VM *vm, Operand *car, Operand *cdr, Operand *dst)
{
	Val carv, cdrv, rv;

	getvalrand(vm, car, &carv);
	getvalrand(vm, cdr, &cdrv);
	mkvalpair(&carv, &cdrv, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisnull(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qnulllist)
		mkvalimm(1, &rv);
	else if(v.qkind == Qpair)
		mkvalimm(0, &rv);
	else
		vmerr(vm, "isnull on non-list");
	putvalrand(vm, &rv, dst);
}

static void
xnull(VM *vm, Operand *dst)
{
	putvalrand(vm, &Xnulllist, dst);
}

static void
xvlist(VM *vm, Operand *op, Operand *dst)
{
	Val v, *vp;
	Imm sp, n, i;
	Val rv;

	getvalrand(vm, op, &v);
	sp = valimm(&v);
	vp = &vm->stack[sp];
	n = valimm(vp);
	rv = Xnulllist;
	for(i = n; i > 0; i--)
		mkvalpair(&vm->stack[sp+i], &rv, &rv);
	putvalrand(vm, &rv, dst);
}

static void* gotab[Iopmax];

static void
vmsetcl(VM *vm, Closure *cl)
{
	unsigned k;
	Insn *i;
	vm->clx = cl;
	vm->ibuf = vm->clx->code->insn;
	if(vm->ibuf->go == 0){
		i = vm->ibuf;
		for(k = 0; k < vm->clx->code->ninsn; k++){
			i->go = gotab[i->kind];
			i++;
		}
	}
}

void
dovm(VM *vm, Closure *cl)
{
	Insn *i;
	Val val, haltv, zero;
	Closure *halt;
	Imm narg, onarg;

	gotab[Iadd]	= &&Iadd;
	gotab[Iand]	= &&Iand;
	gotab[Ibox]	= &&Ibox;
	gotab[Ibox0]	= &&Ibox0;
	gotab[Icall]	= &&Icall;
	gotab[Icallt]	= &&Icallt;
	gotab[Icar]	= &&Icar;
	gotab[Icdr]	= &&Icdr;
	gotab[Iclo]	= &&Iclo;
	gotab[Icmpeq] 	= &&Icmpeq;
	gotab[Icmpgt] 	= &&Icmpgt;
	gotab[Icmpge] 	= &&Icmpge;
	gotab[Icmplt] 	= &&Icmplt;
	gotab[Icmple] 	= &&Icmple;
	gotab[Icmpneq] 	= &&Icmpneq;
	gotab[Icons] 	= &&Icons;
	gotab[Iding] 	= &&Iding;
	gotab[Idiv] 	= &&Idiv;
	gotab[Iframe] 	= &&Iframe;
	gotab[Igc] 	= &&Igc;
	gotab[Ihalt] 	= &&Ihalt;
	gotab[Iinv] 	= &&Iinv;
	gotab[Iisnull] 	= &&Iisnull;
	gotab[Ijmp] 	= &&Ijmp;
	gotab[Ijnz] 	= &&Ijnz;
	gotab[Ijz] 	= &&Ijz;
	gotab[Ikg] 	= &&Ikg;
	gotab[Ikp] 	= &&Ikp;
	gotab[Imod] 	= &&Imod;
	gotab[Imov] 	= &&Imov;
	gotab[Imul] 	= &&Imul;
	gotab[Ineg] 	= &&Ineg;
	gotab[Inot] 	= &&Inot;
	gotab[Inull] 	= &&Inull;
	gotab[Ior] 	= &&Ior;
	gotab[Inop] 	= &&Inop;
	gotab[Iprint] 	= &&Iprint;
	gotab[Ipush] 	= &&Ipush;
	gotab[Iret] 	= &&Iret;
	gotab[Ishl] 	= &&Ishl;
	gotab[Ishr] 	= &&Ishr;
	gotab[Isub] 	= &&Isub;
	gotab[Ivlist] 	= &&Ivlist;
	gotab[Ixor] 	= &&Ixor;

	if(!envlookup(vm->topenv, "halt", &haltv))
		fatal("broken vm");
	halt = valcl(&haltv);

	checkoverflow(vm, 4);
	mkvalimm(0, &zero);
	vmpush(vm, &zero);	/* arbitrary fp */
	vmpush(vm, &haltv);
	mkvalimm(halt->entry, &val);
	vmpush(vm, &val);
	vmpush(vm, &zero);	/* narg */
	vm->fp = vm->sp;

	mkvalcl(cl, &vm->cl);
	vmsetcl(vm, cl);
	vm->pc = vm->clx->entry;

	if(setjmp(vm->esc) != 0)
		return;		/* error throw */

	while(1){
		i = &vm->ibuf[vm->pc++];
		tick++;
		gcpoll(vm);
		goto *(i->go);
		fatal("bug");
	Inop:
		continue;
	Iding:
		printf("ding\n");
		continue;
	Iinv:
	Ineg:
	Inot:
		xunop(vm, i->kind, &i->op1, &i->dst);
		continue;
	Iadd:
	Iand:
	Idiv:
	Imod:
	Imul:
	Ior:
	Ishl:
	Ishr:
	Isub:
	Ixor:
		xbinopcval(vm, i->kind, &i->op1, &i->op2, &i->dst);
		continue;
	Icmplt:
	Icmple:
	Icmpgt:
	Icmpge:
	Icmpeq:
	Icmpneq:
		xbinop(vm, i->kind, &i->op1, &i->op2, &i->dst);
		continue;
	Imov:
		xmov(vm, &i->op1, &i->dst);
		continue;
	Ipush:
		xpush(vm, &i->op1);
		continue;
	Icall:
		getvalrand(vm, &i->op1, &vm->cl);
		vmsetcl(vm, valcl(&vm->cl));
		vm->pc = vm->clx->entry;
		vm->fp = vm->sp;
		continue;
	Icallt:
		getvalrand(vm, &i->op1, &vm->cl);
		vmsetcl(vm, valcl(&vm->cl));
		/* shift current arguments over previous arguments */
		narg = valimm(&vm->stack[vm->sp]);
		onarg = valimm(&vm->stack[vm->fp]);
		vm->fp = vm->fp+onarg-narg;
		memmove(&vm->stack[vm->fp], &vm->stack[vm->sp],
			(narg+1)*sizeof(Val));
		vm->sp = vm->fp;
		vm->pc = vm->clx->entry;
		continue;
	Iframe:
		mkvalimm(vm->fp, &val);
		vmpush(vm, &val);
		vmpush(vm, &vm->cl);
		mkvalimm(i->dstlabel->insn, &val);
		vmpush(vm, &val);
		continue;
	Igc:
		// gc(vm);
		continue;
	Iret:
		vm->sp = vm->fp+valimm(&vm->stack[vm->fp])+1;/*narg+1*/
		vm->fp = valimm(&vm->stack[vm->sp+2]);
		vm->cl = vm->stack[vm->sp+1];
		vmsetcl(vm, valcl(&vm->cl));
		vm->pc = valimm(&vm->stack[vm->sp]);
		vmpop(vm, 3);
		continue;
	Ijmp:
		vm->pc = i->dstlabel->insn;
		continue;
	Ijnz:
		xjnz(vm, &i->op1, i->dstlabel);
		continue;
	Ijz:
		xjz(vm, &i->op1, i->dstlabel);
		continue;
	Iclo:
		xclo(vm, &i->op1, i->dstlabel, &i->dst); 
		/* vm->sp has been updated */
		continue;
	Ikg:
		xkg(vm, &i->dst);
		continue;
	Ikp:
		xkp(vm);
		/* vm->sp, vm->fp have been updated */
		continue;
	Ibox:
		xbox(vm, &i->op1);
		continue;
	Ibox0:
		xbox0(vm, &i->op1);
		continue;
	Iprint:
		xprint(vm, &i->op1);
		continue;
	Ihalt:
		printf("halted (ac = ");
		printval(&vm->ac);
		printf(")\n");
		return;
	Icar:
		xcar(vm, &i->op1, &i->dst);
		continue;
	Icdr:
		xcdr(vm, &i->op1, &i->dst);
		continue;
	Icons:
		xcons(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Inull:
		xnull(vm, &i->dst);
		continue;
	Iisnull:
		xisnull(vm, &i->op1, &i->dst);
		continue;
	Ivlist:
		xvlist(vm, &i->op1, &i->dst);
		continue;
	}
}

VM*
mkvm(Env *env)
{
	VM *vm;
	Val val;
	
	vm = xmalloc(sizeof(VM));
	vm->sp = Maxstk;
	vm->ac = Xundef;
	vm->topenv = env;
	
	mkvalcl(gcthunk(), &val);
	envbind(vm->topenv, "gc", &val);
	mkvalcl(dingthunk(), &val);
	envbind(vm->topenv, "ding", &val);
	mkvalcl(printthunk(), &val);
	envbind(vm->topenv, "print", &val);
	mkvalcl(haltthunk(), &val);
	envbind(vm->topenv, "halt", &val);
	mkvalcl(callcc(), &val);
	envbind(vm->topenv, "callcc", &val);
	mkvalcl(carthunk(), &val);
	envbind(vm->topenv, "car", &val);
	mkvalcl(cdrthunk(), &val);
	envbind(vm->topenv, "cdr", &val);
	mkvalcl(consthunk(), &val);
	envbind(vm->topenv, "cons", &val);
	mkvalcl(nullthunk(), &val);
	envbind(vm->topenv, "null", &val);
	mkvalcl(isnullthunk(), &val);
	envbind(vm->topenv, "isnull", &val);

	concurrentgc(vm);

	return vm;
}

void
freevm(VM *vm)
{
	gckill(vm);
	free(vm);
}

void
initvm()
{
	Xundef.qkind = Qundef;
	Xnil.qkind = Qnil;
	Xnulllist.qkind = Qnulllist;

	heapbox.id = "box";
	heapcl.id = "closure";
	heapcode.id = "code";
	heapcval.id = "cval";
	heapimm.id = "immediate";
	heappair.id = "pair";
	heapstr.id = "string";

	heapbox.sz = sizeof(Box);
	heapcl.sz = sizeof(Closure);
	heapcode.sz = sizeof(Code);
	heapcval.sz = sizeof(Vcval);
	heapimm.sz = sizeof(Vimm);
	heappair.sz = sizeof(Pair);
	heapstr.sz = sizeof(Str);

	heapcode.free1 = freecode;
	heapcl.free1 = freecl;
	heapstr.free1 = freestr;

	heapbox.iter = iterbox;
	heapcl.iter = itercl;
	heappair.iter = iterpair;

	kcode = contcode();

	roots.getlink = getrootslink;
	roots.setlink = setrootslink;
	roots.getolink = getstoreslink;

	stores.getlink = getstoreslink;
	stores.setlink = setstoreslink;
	stores.getolink = getrootslink;

	gcreset();
}

void
finivm()
{
	gcreset();		/* clear store set (FIXME: still needed?) */
	gc(0);
	gc(0);	/* must run two epochs without mutator to collect everything */

	freecode((Head*)kcode);

	freeheap(&heapcode);
	freeheap(&heapcl); 
	freeheap(&heapimm);
	freeheap(&heapbox);
	freeheap(&heapcval);
	freeheap(&heappair);
	freeheap(&heapstr);
}
