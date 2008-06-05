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
	Qas,
	Qbox,
	Qcl,
	Qcode,
	Qdom,
	Qns,
	Qpair,
	Qrange,
	Qstr,
	Qtab,
	Qtype,
	Qvec,
	Qxtn,
	Qnkind
} Qkind;

enum {
	/* type representations */ 
	Rundef,
	Rvoid,
	Ru8le,
	Ru16le,
	Ru32le,
	Ru64le,
	Rs8le,
	Rs16le,
	Rs32le,
	Rs64le,
	Ru8be,
	Ru16be,
	Ru32be,
	Ru64be,
	Rs8be,
	Rs16be,
	Rs32be,
	Rs64be,
	Rnrep, 
};

static char* repname[Rnrep+1] = {
	[Rundef]=	"<undefined>",
	[Rvoid]=	"@void",
	[Ru8le]=	"@u8le",
	[Ru16le]=	"@u16le",
	[Ru32le]=	"@u32le",
	[Ru64le]=	"@u64le",
	[Rs8le]=	"@s8le", 
	[Rs16le]=	"@s16le",
	[Rs32le]=	"@s32le",
	[Rs64le]=	"@s64le",
	[Ru8be]=	"@u8be", 
	[Ru16be]=	"@u16be",
	[Ru32be]=	"@u32be",
	[Ru64be]=	"@u64be",
	[Rs8be]=	"@u8be", 
	[Rs16be]=	"@u16be",
	[Rs32be]=	"@us32be",
	[Rs64be]=	"@us64be",
	[Rnrep]=	"<help me, i'm broken>",
};

enum {
	Tabinitsize=1024,	/* power of 2 */
	Typepos=0,
	Idpos=1,
	Maxvms=1024,
};

struct Heap {
	char *id;
	unsigned sz;
	void (*free1)(Head *hd);
	Head* (*iter)(Head *hd, Ictx *ictx);
	Head *alloc, *swept, *sweep, *free;
};

typedef struct Vcval Vcval;
typedef struct As As;
typedef struct Box Box;
typedef struct Pair Pair;
typedef struct Dom Dom;
typedef struct Ns Ns;
typedef struct Range Range;
typedef struct Str Str;
typedef struct Tab Tab;
typedef struct Vec Vec;
typedef struct Xtypename Xtypename;

struct Val {
	Qkind qkind;
	union {
		Head *hd;
		Cval cval;
		As *as;
		Box *box;
		Closure *cl;
		Dom *dom;
		Ns *ns;
		Pair *pair;
		Range *range;
		Str *str;
		Tab *tab;
		Type *type;
		Vec *vec;
		Xtypename *xtn;
	} u;
};

struct Env {
	HT *ht;
};

typedef void (Cfn)(VM *vm, Imm argc, Val *argv, Val *rv);
typedef void (Ccl)(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv);

struct Closure {
	Head hd;
	Code *code;
	unsigned long entry;
	unsigned dlen;
	Val *display;
	char *id;
	Imm fp;			/* of continuation, always >0 */
	Cfn *cfn;
	Ccl *ccl;
};

struct Box {
	Head hd;
	Val v;
};

typedef struct Tabidx Tabidx;
struct Tabidx {
	u32 idx;
	Tabidx *link;
};

typedef
struct Tabx {
	u32 nxt, lim;
	u32 sz;
	Val *key;
	Val *val;
	Tabidx **idx;
} Tabx;

struct Tab {
	Head hd;
	u32 cnt;		/* key/val pairs stored */
	Tabx *x;		/* current storage, atomically swappable */
};

struct As {
	Head hd;
	Closure *dispatch;
};

struct Ns {
	Head hd;

	/* interface for all instances */
	Closure *enumsym;
	Closure *enumtype;
	Closure *looksym;
	Closure *looktype;

	/* data for instances created by @names */
	/* but these could be pushed into closure defining interface */
	Tab *type;
	Tab *sym;
};

struct Dom {
	Head hd;
	As *as;
	Ns *ns;
};

struct Pair {
	Head hd;
	Val car;
	Val cdr;
};

struct Range {
	Head hd;
	Cval beg;
	Cval len;
};

struct Str {
	Head hd;
	u32 len; /* FIXME: the reason it's not u64 is there's no 64-bit %.*s */
	char *s;
};

struct Vec {
	Head hd;
	Imm len;
	Val *vec;
};

struct Xtypename {
	Head hd;
	unsigned xtkind;	/* = Tbase, Tstruct, ... */
	unsigned basename;	/* base */
	unsigned rep;		/* base, ptr, enum; = Ru8le ... */
	Str *tid;		/* typedef */
	Str *tag;		/* struct, union, enum */
	Val cnt;		/* arr */
	Val sz;			/* struct */
	Xtypename *link;	/* ptr, arr, func (return type) */
	Vec *field;		/* struct, union */
	Vec *param;		/* abstract declarators for func */
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
static void vmerr(VM *vm, char *fmt, ...) __attribute__((noreturn));
static Cval* valcval(Val *v);
static Pair* valpair(Val *v);
static Range* valrange(Val *v);
static Str* valstr(Val *v);
static Vec* valvec(Val *v);
static Xtypename* valxtn(Val *v);
static void mkvalstr(Str *str, Val *vp);
static void mkvalxtn(Xtypename *xtn, Val *vp);

static Val* vecref(Vec *vec, Imm idx);
static void _vecset(Vec *vec, Imm idx, Val *v);
static void vecset(VM *vm, Vec *vec, Imm idx, Val *v);

static Val Xundef;
static Val Xnil;
static Val Xnulllist;
static unsigned long long tick;
static unsigned long gcepoch = 2;

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

typedef struct Root Root;
struct Root {
	Head *hd;
	Root *link;
};

typedef
struct Rootset {
	Root *roots;
	Root *last;
	Root *before_last;
	Root *this;
} Rootset;

static Rootset roots;
static Rootset stores;

#define GCCOLOR(i) ((i)%3)
enum {
	GCfree = 3,
	GCrate = 1000,
};

static unsigned long long nextgctick = GCrate;

static void freecl(Head*);
static void freestr(Head*);
static void freetab(Head*);
static void freevec(Head*);

static Head *iteras(Head*, Ictx*);
static Head *iterbox(Head*, Ictx*);
static Head *itercl(Head*, Ictx*);
static Head *iterdom(Head*, Ictx*);
static Head *iterns(Head*, Ictx*);
static Head *iterpair(Head*, Ictx*);
static Head *itertab(Head*, Ictx*);
static Head *itervec(Head*, Ictx*);
static Head *iterxtn(Head*, Ictx*);

static Heap heap[Qnkind] = {
	[Qas]	= { "as", sizeof(As), 0, iteras },
	[Qbox]	= { "box", sizeof(Box),	0, iterbox },
	[Qcl]	= { "closure", sizeof(Closure), freecl, itercl },
	[Qcode]	= { "code", sizeof(Code), freecode, 0 },
	[Qdom]	= { "domain", sizeof(Dom), 0, iterdom },
	[Qns]	= { "ns", sizeof(Ns), 0, iterns },
	[Qpair]	= { "pair", sizeof(Pair), 0, iterpair },
	[Qrange] = { "range", sizeof(Range), 0, 0 },
	[Qstr]	= { "string", sizeof(Str), freestr, 0 },
	[Qtab]	= { "table", sizeof(Tab), freetab, itertab },
	[Qvec]	= { "vector", sizeof(Vec), freevec, itervec },
	[Qxtn]	= { "typename", sizeof(Xtypename), 0, iterxtn },
};

static u32 nohash(Val*);
static u32 hashcval(Val*);
static u32 hashptr(Val*);
static u32 hashptrv(Val*);
static u32 hashrange(Val*);
static u32 hashstr(Val*);
static u32 hashxtn(Val*);

static int eqcval(Val*, Val*);
static int eqptr(Val*, Val*);
static int eqptrv(Val*, Val*);
static int eqrange(Val*, Val*);
static int eqstr(Val*, Val*);
static int eqxtn(Val*, Val*);

typedef struct Hashop {
	u32 (*hash)(Val*);
	int (*eq)(Val*, Val*);
} Hashop;

static Hashop hashop[Qnkind] = {
	[Qundef] = { nohash, 0 },
	[Qnil]	= { hashptrv, eqptrv },
	[Qnulllist] = { hashptrv, eqptrv },
	[Qas]	= { hashptr, eqptr },
	[Qbox]	= { nohash, 0 },
	[Qcl]	= { hashptr, eqptr },
	[Qcode]	= { nohash, 0 },
	[Qcval]	= { hashcval, eqcval },
	[Qdom]	= { hashptr, eqptr },
	[Qns]	= { hashptr, eqptr },
	[Qpair]	= { hashptr, eqptr },
	[Qrange] =  { hashrange, eqrange },
	[Qstr]	= { hashstr, eqstr },
	[Qtab]	= { hashptr, eqptr },
	[Qtype]	= { hashptr, eqptr },
	[Qvec]	= { hashptr, eqptr },
	[Qxtn]	= { hashxtn, eqxtn },
};

static Code *kcode, *cccode;

static void*
read_and_clear(void *pp)
{
	void **p = (void**)pp;
	void *q;
	q = 0;
	asm("xchg %0,%1" : "=r"(q), "=m"(*p) : "0"(q), "m"(*p) : "memory");
	return q;
}

static inline
void atomic_inc(int *p)
{
	asm("lock incl %0" : "+m" (*p));
}

static inline
void atomic_dec(int *p)
{
	asm("lock decl %0" : "+m" (*p));
}

static void
writebarrier()
{
	/* force previous writes to be visible to other processors
	   before subsequent ones. */
	/* on x86 (through core 2 duo),
	   processor-ordering memory model precludes
	   need for explict barrier such as sfence.  if
	   the underlying memory were in WC mode (see
	   intel vol 3, chapter 10), things would be different. */
}

static Head*
halloc(Heap *heap)
{
	Head *o, *ap, *fp;
	unsigned m;

retry:
	if(heap->free){
		o = heap->free;
		heap->free = o->link;
		o->link = 0;
		if(o->state != -1)
			fatal("halloc bad state %d", o->state);
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
			if(p->state != 0 || p->inrootset)
				fatal("sweep heap (%s) bad state %d",
				      heap->id, p->state);
			p->link = heap->sweep;
			heap->sweep = p;
			p->state = -1;
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
	unsigned i;
	Heap *hp;

	for(i = 0; i < Qnkind; i++){
		hp = &heap[i];
		if(hp->id)
			sweepheap(hp, color);
	}
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
	case Qtype:
		return 0;
		break;
	default:
		return v->u.hd;
		break;
	}
}

static Root*
newroot()
{
	return xmalloc(sizeof(Root));
}

static void
freeroot(Root *r)
{
	free(r);
}

/* called on roots by marker.  called on stores by mutator. */
static void
addroot(Rootset *rs, Head *h)
{
	Root *r;
	int x;

	if(h == 0)
		return;
	if(h->inrootset)
		return;

	/* test if already on a rootlist */
	x = h->state;
	if(x > 2 || x < 0)
		fatal("addroot bad state %d", x);
	atomic_inc(&h->state);

	r = newroot();
	h->inrootset = 1;
	r->hd = h;
	r->link = rs->roots;
	writebarrier();
	rs->roots = r;
}

static Head*
removeroot(Rootset *rs)
{
	Head *h;
	Root *r;
	int x;

	if(rs->this == rs->before_last){
		rs->before_last = rs->last;
		rs->this = rs->last = rs->roots;
	}
	if(rs->this == rs->before_last)
		return 0;
	r = rs->this;
	rs->this = r->link;
	h = r->hd;
	h->inrootset = 0;
	x = h->state;
	if(x > 2 || x <= 0)
		fatal("remove root bad state %d", x);
	atomic_dec(&h->state);
	return h;
}

static int
rootsetempty(Rootset *rs)
{
	return rs->before_last == rs->roots;
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
	addroot(&roots, (Head*)cccode); 

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
	Root *r, *nxt;

	r = rs->roots;
	while(r){
		nxt = r->link;
		freeroot(r);
		r = nxt;
	}
	rs->roots = 0;
	rs->last = 0;
	rs->before_last = 0;
	rs->this = 0;
}

static void
gcreset()
{
	rootsetreset(&roots);
	rootsetreset(&stores);
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

static void
gc(VM *vm)
{
	gcreset();
	rootset(vm);
	gcepoch++;
	mark(GCCOLOR(gcepoch));
	sweep(GCCOLOR(gcepoch-2));
	while(!rootsetempty(&stores))
		mark(GCCOLOR(gcepoch));
}

static void*
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

static Imm
typesize(VM *vm, Type *t)
{
	switch(t->kind){
	case Tbase:
		return basesize[t->base];
	case Ttypedef:
		return typesize(vm, t->link);
	case Tstruct:
	case Tunion:
		vmerr(vm, "need to evaluate struct/union size");
		break;
	case Tenum:
		vmerr(vm, "need to get enums right");
		break;
	case Tptr:
		return ptrsize;
	case Tarr:
		vmerr(vm, "need to evaluate array size");
		break;
	case Tfun:
		vmerr(vm, "attempt to compute size of function type");
		break;
	default:
		fatal("typesize bug");
	}
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

Code*
newcode()
{
	return (Code*)halloc(&heap[Qcode]);
}

Closure*
mkcl(Code *code, unsigned long entry, unsigned len, char *id)
{
	Closure *cl;
	cl = (Closure*)halloc(&heap[Qcl]);
	cl->code = code;
	cl->entry = entry;
	cl->dlen = len;
	cl->display = xmalloc(cl->dlen*sizeof(Val));
	cl->id = xstrdup(id);
	return cl;
}

Closure*
mkcfn(char *id, Cfn *cfn)
{
	Closure *cl;
	cl = mkcl(cccode, 0, 0, id);
	cl->cfn = cfn;
	return cl;
}

Closure*
mkccl(char *id, Ccl *ccl, unsigned dlen, ...)
{
	Closure *cl;
	va_list args;
	Val *vp;
	unsigned m;

	va_start(args, dlen);
	cl = mkcl(cccode, 0, dlen, id);
	cl->ccl = ccl;
	for(m = 0; m < dlen; m++){
		vp = va_arg(args, Val*);
		cl->display[m] = *vp;
	}
	va_end(args);
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


/* http://www.cris.com/~Ttwang/tech/inthash.htm */
static u32
hash6432shift(u64 key)
{
	key = (~key) + (key << 18);
	key = key ^ (key >> 31);
	key = key * 21;
	key = key ^ (key >> 11);
	key = key + (key << 6);
	key = key ^ (key >> 22);
	return (u32)key;
}

static u32
hashptr32shift(void *p)
{
	uintptr_t key;
	key = (uintptr_t)p;
	key = (~key) + (key << 18);
	key = key ^ (key >> 31);
	key = key * 21;
	key = key ^ (key >> 11);
	key = key + (key << 6);
	key = key ^ (key >> 22);
	return (u32)key;
}

/* one-at-a-time by jenkins */
static u32
shash(char *s, Imm len)
{
	unsigned char *p = (unsigned char*)s;
	u32 h;

	h = 0;
	while(len > 0){
		h += *p;
		h += h<<10;
		h ^= h>>6;
		p++;
		len--;
	}
	h += h<<3;
	h ^= h>>11;
	h += h<<15;
	return h;
}

static u32
nohash(Val *val)
{
	fatal("bad type of key (%d) to table operation", val->qkind);
}

static u32
hashptr(Val *val)
{
	return hashptr32shift(valhead(val));
}

static int
eqptr(Val *a, Val *b)
{
	return valhead(a)==valhead(b);
}

static u32
hashptrv(Val *val)
{
	return hashptr32shift(val);
}

static int
eqptrv(Val *a, Val *b)
{
	return a==b;
}

static u32
hashcval(Val *val)
{
	Cval *cv;
	cv = valcval(val);
	return hash6432shift(cv->val);
}

static int
eqcval(Val *a, Val *b)
{
	Cval *cva, *cvb;
	cva = valcval(a);
	cvb = valcval(b);
	return cva->val==cvb->val;
}

static u32
hashrange(Val *val)
{
	Range *r;
	r = valrange(val);
	return hash6432shift(r->beg.val)^hash6432shift(r->len.val);
}

static int
eqrange(Val *a, Val *b)
{
	Range *ra, *rb;
	ra = valrange(a);
	rb = valrange(b);
	return ra->beg.val==rb->beg.val && ra->len.val==rb->len.val;
}

static u32
hashstr(Val *val)
{
	Str *s;
	s = valstr(val);
	return shash(s->s, s->len);
}

static int
eqstr(Val *a, Val *b)
{
	Str *sa, *sb;
	sa = valstr(a);
	sb = valstr(b);
	if(sa->len != sb->len)
		return 0;
	return memcmp(sa->s, sb->s, sa->len) ? 0 : 1;
}

static u32
hashxtn(Val *val)
{
	Val v;
	u32 x;
	Xtypename *xtn;
	Vec *vec;
	Imm m;
	
	xtn = valxtn(val);
	switch(xtn->xtkind){
	case Tbase:
		return hash6432shift(xtn->basename);
	case Ttypedef:
		mkvalstr(xtn->tid, &v);
		return hashstr(&v)<<xtn->xtkind;
	case Tstruct:
	case Tunion:
	case Tenum:
		mkvalstr(xtn->tag, &v);
		return hashstr(&v)<<xtn->xtkind;
	case Tptr:
		mkvalxtn(xtn->link, &v);
		return hashxtn(&v)<<xtn->xtkind;;
	case Tarr:
		mkvalxtn(xtn->link, &v);
		x = hashxtn(&v)<<xtn->xtkind;
		if(xtn->cnt.qkind == Qcval)
			x ^= hashcval(&xtn->cnt);
		return x;
	case Tfun:
		mkvalxtn(xtn->link, &v);
		x = hashxtn(&v)<<xtn->xtkind;
		for(m = 0; m < xtn->param->len; m++){
			x <<= 1;
			vec = valvec(vecref(xtn->param, m));
			x ^= hashxtn(vecref(vec, Typepos));
		}
		return x;
	default:
		fatal("bug");
	}
}

static int
eqxtn(Val *a, Val *b)
{
	Val va, vb;
	Xtypename *xa, *xb;
	Imm m;

	xa = valxtn(a);
	xb = valxtn(b);

	if(xa->xtkind != xb->xtkind)
		return 0;

	switch(xa->xtkind){
	case Tbase:
		return xa->basename == xb->basename;
	case Ttypedef:
		mkvalstr(xa->tid, &va);
		mkvalstr(xb->tid, &vb);
		return eqstr(&va, &vb);
	case Tstruct:
	case Tunion:
	case Tenum:
		mkvalstr(xa->tag, &va);
		mkvalstr(xb->tag, &vb);
		return eqstr(&va, &vb);
	case Tptr:
		mkvalxtn(xa->link, &va);
		mkvalxtn(xb->link, &vb);
		return eqxtn(&va, &vb);
	case Tarr:
		if(xa->cnt.qkind != xb->cnt.qkind)
			return 0;
		if(xa->cnt.qkind == Qcval){
			if(!eqcval(&xa->cnt, &xb->cnt))
				return 0;
		}
		mkvalxtn(xa->link, &va);
		mkvalxtn(xb->link, &vb);
		return eqxtn(&va, &vb);
	case Tfun:
		if(xa->param->len != xb->param->len)
			return 0;
		mkvalxtn(xa->link, &va);
		mkvalxtn(xb->link, &vb);
		if(!eqxtn(&va, &vb))
			return 0;
		for(m = 0; m < xa->param->len; m++)
			if(!eqxtn(vecref(valvec(vecref(xa->param, m)),Typepos),
				  vecref(valvec(vecref(xb->param, m)),Typepos)))
				return 0;
		return 1;
	default:
		fatal("bug");
	}
}

static Str*
mkstr0(char *s)
{
	Str *str;
	str = (Str*)halloc(&heap[Qstr]);
	str->len = strlen(s);
	str->s = xmalloc(str->len);
	memcpy(str->s, s, str->len);
	return str;
}

static Str*
mkstr(char *s, unsigned long len)
{
	Str *str;
	str = (Str*)halloc(&heap[Qstr]);
	str->len = len;
	str->s = xmalloc(str->len);
	memcpy(str->s, s, str->len);
	return str;
}

static Str*
mkstrn(unsigned long len)
{
	Str *str;
	str = (Str*)halloc(&heap[Qstr]);
	str->len = len;
	str->s = xmalloc(str->len);
	return str;
}

static void
strinit(Str *str, Lits *lits)
{
	str->len = lits->len;
	str->s = xmalloc(str->len);
	memcpy(str->s, lits->s, str->len);
}

static Str*
strslice(Str *str, Imm beg, Imm end)
{
	return mkstr(str->s+beg, end-beg);
}

static void
freestr(Head *hd)
{
	Str *str;
	str = (Str*)hd;
	free(str->s);
}

static int
listlen(Val *v, Imm *rv)
{
	Imm m;
	Pair *p;

	m = 0;
	while(v->qkind == Qpair){
		m++;
		p = valpair(v);
		v = &p->cdr;
	}
	if(v->qkind != Qnulllist)
		return 0;
	*rv = m;
	return 1;
}

static Vec*
mkvec(Imm len)
{
	Vec *vec;

	vec = (Vec*)halloc(&heap[Qvec]);
	vec->len = len;
	vec->vec = xmalloc(len*sizeof(Val));
	return vec;
}

static Vec*
mkvecnil(Imm len)
{
	Vec *vec;
	Imm i;

	vec = mkvec(len);
	for(i = 0; i < len; i++)
		vec->vec[i] = Xnil;
	return vec;
}

static Val*
vecref(Vec *vec, Imm idx)
{
	return &vec->vec[idx];
}

static void
_vecset(Vec *vec, Imm idx, Val *v)
{
	vec->vec[idx] = *v;
}

static void
vecset(VM *vm, Vec *vec, Imm idx, Val *v)
{
	if(vm->gcrun)
		addroot(&stores, valhead(&vec->vec[idx]));
	_vecset(vec, idx, v);
}

static Head*
itervec(Head *hd, Ictx *ictx)
{
	Vec *vec;
	vec = (Vec*)hd;
	if(ictx->n >= vec->len)
		return 0;
	return valhead(&vec->vec[ictx->n++]);
}

static void
freevec(Head *hd)
{
	Vec *vec;
	vec = (Vec*)hd;
	free(vec->vec);
}

static Tabx*
mktabx(u32 sz)
{
	Tabx *x;
	x = xmalloc(sizeof(Tabx));
	x->sz = sz;
	x->lim = 2*sz/3;
	x->val = xmalloc(x->lim*sizeof(Val)); /* matches Xundef */
	x->key = xmalloc(x->lim*sizeof(Val)); /* matches Xundef */
	x->idx = xmalloc(x->sz*sizeof(Tabidx*));
	return x;
}

static void
freetabx(Tabx *x)
{
	free(x->val);
	free(x->key);
	free(x->idx);
	free(x);
}

static Tab*
_mktab(Tabx *x)
{
	Tab *tab;
	tab = (Tab*)halloc(&heap[Qtab]);
	tab->x = x;
	return tab;
}

static Tab*
mktab()
{
	return _mktab(mktabx(Tabinitsize));
}

static Head*
itertab(Head *hd, Ictx *ictx)
{
	Tab *tab;
	u32 idx;
	Tabx *x;

	tab = (Tab*)hd;

	if(ictx->n == 0){
		ictx->x = x = tab->x;
	}else
		x = ictx->x;
		
	if(ictx->n >= 2*x->nxt)
		return 0;
	if(ictx->n >= x->nxt){
		idx = ictx->n-x->nxt;
		ictx->n++;
		return valhead(&x->val[idx]);
	}
	idx = ictx->n++;
	return valhead(&x->key[idx]);
}

static void
freetab(Head *hd)
{
	u32 i;
	Tab *tab;
	Tabx *x;
	Tabidx *tk, *pk;

	tab = (Tab*)hd;
	x = tab->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			pk = tk;
			tk = tk->link;
			free(pk);
		}
	}
	freetabx(x);
}

static Tabidx*
_tabget(Tab *tab, Val *keyv, Tabidx ***prev)
{
	Tabidx **p, *tk;
	Val *kv;
	u32 idx;
	Tabx *x;
	unsigned kind;
	Hashop *op;

	x = tab->x;
	kind = keyv->qkind;
	op = &hashop[kind];
	idx = op->hash(keyv)&(x->sz-1);
	p = &x->idx[idx];
	tk = *p;
	while(tk){
		kv = &x->key[tk->idx];
		if(kv->qkind == kind && op->eq(keyv, kv)){
			if(prev)
				*prev = p;
			return tk;
		}
		p = &tk->link;
		tk = *p;
	}
	return 0;
}

static Val*
tabget(Tab *tab, Val *keyv)
{
	Tabidx *tk;
	tk = _tabget(tab, keyv, 0);
	if(tk)
		return &tab->x->val[tk->idx];
	return 0;
}

/* double hash table size and re-hash entries */
static void
tabexpand(VM *vm, Tab *tab)
{
	Tabidx *tk, *nxt;
	u32 i, m, idx;
	Tabx *x, *nx;
	Val *kv;

	x = tab->x;
	nx = mktabx(x->sz*2);
	m = 0;
	for(i = 0; i < x->sz && m < tab->cnt; i++){
		tk = x->idx[i];
		while(tk){
			nxt = tk->link;
			kv = &x->key[tk->idx];
			idx = hashop[kv->qkind].hash(kv)&(nx->sz-1);
			tk->link = nx->idx[idx];
			nx->idx[idx] = tk;
			nx->key[m] = x->key[tk->idx];
			nx->val[m] = x->val[tk->idx];
			tk->idx = m;
			m++;
			tk = nxt;
		}
	}
	nx->nxt = tab->cnt;

	/* fresh garbage reference to pre-expanded state of table.
	   this preserves a reference to the pre-expand storage
	   (tab->x) in case gc (via itertab) is concurrently (i.e., in
	   this epoch) marking it.  we exploit the property that
	   objects always survive the epoch of their creation. */
	x->sz = 0;		/* so freetab does not free nx's Tabkeys */
	_mktab(x);

	/* FIXME: it seems snapshot-at-beginning property, plus above
	   referenced property of objects, together ensure that any
	   object added to expanded table is safe from collection. */
	tab->x = nx;
}

static void
tabput(VM *vm, Tab *tab, Val *keyv, Val *val)
{
	Tabidx *tk;
	u32 idx;
	Tabx *x;

	x = tab->x;
	tk = _tabget(tab, keyv, 0);
	if(tk){
		if(vm->gcrun)
			addroot(&stores, valhead(&x->val[tk->idx]));
		x->val[tk->idx] = *val;
		return;
	}

	if(x->nxt >= x->lim){
		tabexpand(vm, tab);
		x = tab->x;
	}

	tk = xmalloc(sizeof(Tabidx));

	/* FIXME: snapshot-at-beginning seems to imply that it does
	   not matter whether gc can see these new values in this
	   epoch. right? */
	tk->idx = x->nxt;
	x->key[tk->idx] = *keyv;
	x->val[tk->idx] = *val;
	idx = hashop[keyv->qkind].hash(keyv)&(x->sz-1);
	tk->link = x->idx[idx];
	x->idx[idx] = tk;
	x->nxt++;
	tab->cnt++;
}

static void
tabdel(VM *vm, Tab *tab, Val *keyv)
{
	Tabidx *tk, **ptk;
	Tabx *x;
	
	x = tab->x;
	tk = _tabget(tab, keyv, &ptk);
	if(tk == 0)
		return;
	if(vm->gcrun){
		addroot(&stores, valhead(&x->key[tk->idx]));
		addroot(&stores, valhead(&x->val[tk->idx]));
	}
	x->key[tk->idx] = Xundef;
	x->val[tk->idx] = Xundef;
	*ptk = tk->link;
	free(tk);
	tab->cnt--;
}

/* create a new vector of len 2N, where N is number of elements in TAB.
   elements 0..N-1 are the keys of TAB; N..2N-1 are the associated vals. */
static Vec*
tabenum(Tab *tab)
{
	Vec *vec;
	Tabidx *tk;
	u32 i;;
	Imm m;
	Tabx *x;

	x = tab->x;
	vec = mkvec(2*tab->cnt);
	m = 0;
	for(i = 0; i < x->sz && m < tab->cnt; i++){
		tk = x->idx[i];
		while(tk){
			_vecset(vec, m, &x->key[tk->idx]);
			_vecset(vec, m+tab->cnt, &x->val[tk->idx]);
			m++;
			tk = tk->link;
		}
	}
	return vec;
}

static Vec*
tabenumkeys(Tab *tab)
{
	Vec *vec;
	Tabidx *tk;
	u32 i;;
	Imm m;
	Tabx *x;

	x = tab->x;
	vec = mkvec(tab->cnt);
	m = 0;
	for(i = 0; i < x->sz && m < tab->cnt; i++){
		tk = x->idx[i];
		while(tk){
			_vecset(vec, m, &x->key[tk->idx]);
			m++;
			tk = tk->link;
		}
	}
	return vec;
}

static Vec*
tabenumvals(Tab *tab)
{
	Vec *vec;
	Tabidx *tk;
	u32 i;;
	Imm m;
	Tabx *x;

	x = tab->x;
	vec = mkvec(tab->cnt);
	m = 0;
	for(i = 0; i < x->sz && m < tab->cnt; i++){
		tk = x->idx[i];
		while(tk){
			_vecset(vec, m, &x->val[tk->idx]);
			m++;
			tk = tk->link;
		}
	}
	return vec;
}

static Xtypename*
mkxtn()
{
	Xtypename *xtn;
	xtn = (Xtypename*)halloc(&heap[Qxtn]);
	return xtn;
}

static Head*
iterxtn(Head *hd, Ictx *ictx)
{
	Xtypename *xtn;

	xtn = (Xtypename*)hd;
	switch(xtn->xtkind){
	case Tbase:
		return 0;
	case Tstruct:
	case Tunion:
	case Tenum:
		if(ictx->n++ > 0)
			return 0;
		else
			return (Head*)xtn->tag;
	case Tptr:
		if(ictx->n++ > 0)
			return 0;
		else
			return (Head*)xtn->link;
	case Tarr:
		switch(ictx->n++){
		case 0:
			return valhead(&xtn->cnt);
		case 1:
			return (Head*)xtn->link;
		default:
			return 0;
		}
	case Tfun:
		switch(ictx->n++){
		case 0:
			return (Head*)xtn->link;
		case 1:
			return (Head*)xtn->param;
		default:
			return 0;
		}
		break;
	case Ttypedef:
		if(ictx->n++ > 0)
			return 0;
		else
			return (Head*)xtn->tid;
		break;
	default:
		fatal("bug");
	}
	return 0;
}

static char* _fmtxtn(Xtypename *xtn, char *o);
static char* _fmtdecl(Xtypename *xtn, Str *is);

static char*
fmtplist(Vec *param)
{
	char **ds, *buf, *bp;
	Imm i, n;
	unsigned long m;
	Xtypename *xtn;
	Vec *pvec;
	Val *id;
	
	n = param->len;
	if(n == 0)
		return xstrdup("");

	ds = xmalloc(n*sizeof(char**));
	m = 1;			/* null */
	for(i = 0; i < n; i++){
		pvec = valvec(vecref(param, i));
		xtn = valxtn(vecref(pvec, Typepos));
		id = vecref(pvec, 1);
		if(id->qkind == Qstr)
			ds[i] = _fmtdecl(xtn, valstr(id));
		else
			ds[i] = _fmtxtn(xtn, xstrdup(""));
		m += strlen(ds[i]);
		if(i < n-1)
			m += 2;	/* comma, space */
	}
	buf = xmalloc(m);
	bp = buf;
	for(i = 0; i < n; i++){
		strcpy(bp, ds[i]);
		bp += strlen(ds[i]);
		free(ds[i]);
		if(i < n-1){
			strcpy(bp, ", ");
			bp += 2;
		}
	}
	*bp = 0;
	free(ds);
	return buf;
}

static char*
_fmtxtn(Xtypename *xtn, char *o)
{
	char *buf, *w, *pl;
	unsigned m;
	Str *s;

	switch(xtn->xtkind){
	case Tbase:
		m = strlen(basename[xtn->basename])+strlen(o)+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s%s", basename[xtn->basename], o);
		free(o);
		return buf;
	case Ttypedef:
		s = xtn->tid;
		m = s->len+strlen(o)+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%.*s%s", s->len, s->s, o);
		free(o);
		return buf;
	case Tstruct:
	case Tunion:
		w = xtn->xtkind == Tstruct ? "struct" : "union";
		if(xtn->tag){
			s = xtn->tag;
			m = strlen(w)+1+s->len+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "%s %.*s%s", w, s->len, s->s, o);
		}else{
			m = strlen(w)+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "%s%s", w, o);
		}
		free(o);
		return buf;
	case Tenum:
		if(xtn->tag){
			s = xtn->tag;
			m = 4+1+s->len+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "enum %.*s%s", s->len, s->s, o);
		}else{
			m = 4+strlen(o)+1;
			buf = xmalloc(m);
			snprintf(buf, m, "enum%s", o);
		}
		free(o);
		return buf;
	case Tptr:
		m = 2+strlen(o)+1+1;
		buf = xmalloc(m);
		if(xtn->link->xtkind == Tfun || xtn->link->xtkind == Tarr)
			snprintf(buf, m, "(*%s)", o);
		else
			snprintf(buf, m, "*%s", o);
		free(o);
		return _fmtxtn(xtn->link, buf);
	case Tarr:
		m = strlen(o)+2+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s[]", o);
		free(o);
		return _fmtxtn(xtn->link, buf);
	case Tfun:
		pl = fmtplist(xtn->param);
		m = strlen(o)+1+strlen(pl)+1+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s(%s)", o, pl);
		free(o);
		free(pl);
		return _fmtxtn(xtn->link, buf);
	default:
		fatal("bug");
	}
	return NULL;

}

static char*
_fmtdecl(Xtypename *xtn, Str *id)
{
	char *o;
	o = xmalloc(1+id->len+1);
	o[0] = ' ';
	memcpy(o+1, id->s, id->len);
	return _fmtxtn(xtn, o);
}

static Str*
fmtdecl(Xtypename *xtn, Str *id)
{
	char *s;
	Str *str;
	s = _fmtdecl(xtn, id);
	str = mkstr0(s);
	free(s);
	return str;
}

static Str*
fmtxtn(Xtypename *xtn)
{
	char *s;
	Str *str;
	s = _fmtxtn(xtn, xstrdup(""));
	str = mkstr0(s);
	free(s);
	return str;
}

static As*
mkas()
{
	As *as;
	as = (As*)halloc(&heap[Qas]);
	return as;
}

static Head*
iteras(Head *hd, Ictx *ictx)
{
	As *as;
	as = (As*)hd;
	switch(ictx->n++){
	case 0:
		return (Head*)as->dispatch;
	default:
		return 0;
	}
}

static Dom*
mkdom()
{
	Dom *dom;
	dom = (Dom*)halloc(&heap[Qdom]);
	return dom;
}

static Head*
iterdom(Head *hd, Ictx *ictx)
{
	Dom *dom;
	dom = (Dom*)hd;
	switch(ictx->n++){
	case 0:
		return (Head*)dom->as;
	case 1:
		return (Head*)dom->ns;
	default:
		return 0;
	}
}

static Ns*
mkns()
{
	Ns *ns;
	ns = (Ns*)halloc(&heap[Qns]);
	return ns;
}

static Head*
iterns(Head *hd, Ictx *ictx)
{
	Ns *ns;
	ns = (Ns*)hd;
	switch(ictx->n++){
	case 0:
		return (Head*)ns->sym;
	case 1:
		return (Head*)ns->type;
	case 2:
		return (Head*)ns->looksym;
	case 3:
		return (Head*)ns->looktype;
	default:
		return 0;
	}
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
	box = (Box*)halloc(&heap[Qbox]);
	box->v = *boxed;
	vp->qkind = Qbox;
	vp->u.box = box;
}

static void
mkvalas(As *as, Val *vp)
{
	vp->qkind = Qas;
	vp->u.as = as;
}

static void
mkvaldom(Dom *dom, Val *vp)
{
	vp->qkind = Qdom;
	vp->u.dom = dom;
}

static void
mkvalns(Ns *ns, Val *vp)
{
	vp->qkind = Qns;
	vp->u.ns = ns;
}

static void
mkvalpair(Val *car, Val *cdr, Val *vp)
{
	Pair *pair;
	pair = (Pair*)halloc(&heap[Qpair]);
	pair->car = *car;
	pair->cdr = *cdr;
	vp->qkind = Qpair;
	vp->u.pair = pair;
}

static void
mkvalstr(Str *str, Val *vp)
{
	vp->qkind = Qstr;
	vp->u.str = str;
}

static void
mkvaltab(Tab *tab, Val *vp)
{
	vp->qkind = Qtab;
	vp->u.tab = tab;
}

static void
mkvalvec(Vec *vec, Val *vp)
{
	vp->qkind = Qvec;
	vp->u.vec = vec;
}

static void
mkvaltype(Type *type, Val *vp)
{
	vp->qkind = Qtype;
	vp->u.type = type;
}

static void
mkvalrange(Cval *beg, Cval *len, Val *vp)
{
	Range *r;

	r = (Range*)halloc(&heap[Qrange]);
	r->beg = *beg;
	r->len = *len;
	vp->qkind = Qrange;
	vp->u.range = r;
}

static void
mkvalxtn(Xtypename *xtn, Val *vp)
{
	vp->qkind = Qxtn;
	vp->u.xtn = xtn;
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

static As*
valas(Val *v)
{
	if(v->qkind != Qas)
		fatal("valas on non-addrspace");
	return v->u.as;
}

static Dom*
valdom(Val *v)
{
	if(v->qkind != Qdom)
		fatal("valdom on non-domain");
	return v->u.dom;
}

static Ns*
valns(Val *v)
{
	if(v->qkind != Qns)
		fatal("valns on non-namespace");
	return v->u.ns;
}

static Pair*
valpair(Val *v)
{
	if(v->qkind != Qpair)
		fatal("valpair on non-pair");
	return v->u.pair;
}

static Range*
valrange(Val *v)
{
	if(v->qkind != Qrange)
		fatal("valrange on non-range");
	return v->u.range;
}

static Str*
valstr(Val *v)
{
	if(v->qkind != Qstr)
		fatal("valstr on non-string");
	return v->u.str;
}

static Tab*
valtab(Val *v)
{
	if(v->qkind != Qtab)
		fatal("valtab on non-table");
	return v->u.tab;
}

static Vec*
valvec(Val *v)
{
	if(v->qkind != Qvec)
		fatal("valvec on non-vector");
	return v->u.vec;
}

static Xtypename*
valxtn(Val *v)
{
	if(v->qkind != Qxtn)
		fatal("valxtn on non-typename");
	return v->u.xtn;
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

static Type*
valtype(Val *v)
{
	if(v->qkind != Qtype)
		fatal("valtype on non-type");
	return v->u.type;
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
printsrc(FILE *out, Closure *cl, Imm pc)
{
	Code *code;
	
	code = cl->code;
	if(cl->cfn || cl->ccl){
		fprintf(out, "%s\t\t(builtin %s)\n", cl->id,
		       cl->cfn ? "function" : "closure");
		return;
	}

	while(1){
		if(code->labels[pc] && code->labels[pc]->src){
			fprintf(out, "%s\t\t%s:%u\n", cl->id,
			       code->labels[pc]->src->filename,
			       code->labels[pc]->src->line);
			return;
		}
		if(pc == 0)
			break;
		pc--;
	}
	fprintf(out, "%s\t\t(no source information)\n", cl->id);
}

static void
fvmbacktrace(FILE *out, VM *vm)
{
	Imm pc, fp, narg;
	Closure *cl;

	pc = vm->pc-1;		/* vm loop increments pc after fetch */
	if(vm->pc == 0)
		fprintf(out, "vmerr: pc is 0!\n");
	fp = vm->fp;
	cl = vm->clx;
	while(fp != 0){
		if(!strcmp(cl->id, "$halt"))
			fprintf(out, "\t-- vmcall --\n");
		else
			printsrc(out, cl, pc);
		narg = valimm(&vm->stack[fp]);
		pc = valimm(&vm->stack[fp+narg+1]);
		pc--; /* pc was insn following call */
		cl = valcl(&vm->stack[fp+narg+2]);
		fp = valimm(&vm->stack[fp+narg+3]);
	}
}

static void
vmbacktrace(VM *vm)
{
	fvmbacktrace(stderr, vm);
}

static void
vmerr(VM *vm, char *fmt, ...)
{
	va_list args;
	FILE *out = stdout;

	va_start(args, fmt);
	fprintf(out, "error: ");
	vfprintf(out, fmt, args);
	fprintf(out, "\n");
	va_end(args);
	fflush(out);

	fvmbacktrace(out, vm);

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
		vp->u.str = (Str*)halloc(&heap[Qstr]);
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
putcvalrand(VM *vm, Cval *cv, Operand *r)
{
	Val v;

	if(r->okind != Oloc)
		fatal("bad destination");
	mkvalcval2(cv, &v);
	putval(vm, &v, &r->u.loc);
}

static Imm
str2imm(Type *t, Str *s)
{
	if(t->kind != Tbase && t->kind != Tptr)
		fatal("str2imm on non-scalar type");
	if(t->kind == Tptr){
		return *(u32*)s;
	}

	switch(t->base){
	case Vchar:
		return *(s8*)s;
	case Vshort:
		return *(s16*)s;
	case Vint:
		return *(s32*)s;
	case Vlong:
		return *(s32*)s;
	case Vvlong:
		return *(s64*)s;
	case Vuchar:
		return *(u8*)s;
	case Vushort:
		return *(u16*)s;
	case Vuint:
		return *(u32*)s;
	case Vulong:
		return *(u32*)s;
	case Vuvlong:
		return *(u64*)s;
	default:
		fatal("missing case in str2imm");
	}	
}

static Str*
imm2str(Type *t, Imm imm)
{
	Str *str;
	char *s;

	if(t->kind != Tbase && t->kind != Tptr)
		fatal("str2imm on non-scalar type");

	if(t->kind == Tptr){
		str = mkstrn(sizeof(u32));
		s = str->s;
		*(u32*)s = (u32)imm;
		return str;
	}


	switch(t->base){
	case Vchar:
		str = mkstrn(sizeof(s8));
		s = str->s;
		*(s8*)s = (s8)imm;
		return str;
	case Vshort:
		str = mkstrn(sizeof(s16));
		s = str->s;
		*(s16*)s = (s16)imm;
		return str;
	case Vint:
		str = mkstrn(sizeof(s32));
		s = str->s;
		*(s32*)s = (s32)imm;
		return str;
	case Vlong:
		str = mkstrn(sizeof(s32));
		s = str->s;
		*(s32*)s = (s32)imm;
		return str;
	case Vvlong:
		str = mkstrn(sizeof(s64));
		s = str->s;
		*(s64*)s = (s64)imm;
		return str;
	case Vuchar:
		str = mkstrn(sizeof(u8));
		s = str->s;
		*(u8*)s = (u8)imm;
		return str;
	case Vushort:
		str = mkstrn(sizeof(u16));
		s = str->s;
		*(u16*)s = (u16)imm;
		return str;
	case Vuint:
		str = mkstrn(sizeof(u32));
		s = str->s;
		*(u32*)s = (u32)imm;
		return str;
	case Vulong:
		str = mkstrn(sizeof(u32));
		s = str->s;
		*(u32*)s = (u32)imm;
		return str;
	case Vuvlong:
		str = mkstrn(sizeof(u64));
		s = str->s;
		*(u64*)s = (u64)imm;
		return str;
	default:
		fatal("missing case in imm2str");
	}	
}

static void
printval(Val *val)
{
	Cval *cv;
	Closure *cl;
	Pair *pair;
	Range *r;
	Str *str;
	char *o;
	Val bv;
	Tab *tab;
	Type *t;
	Vec *vec;
	Xtypename *xtn;

	if(val == 0){
		printf("(no value)");
		return;
	}

	switch(val->qkind){
	case Qcval:
		cv = valcval(val);
 		printf("<cval %" PRIu64 ">", cv->val);
		break;
	case Qcl:
		cl = valcl(val);
		if(cl->fp)
			printf("<continuation %p>", cl);
		else
			printf("<closure %s>", cl->id);
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
		valboxed(val, &bv);
		printval(&bv);
		printf(">");
		break;
	case Qas:
		printf("<as %p>", valas(val));
		break;
	case Qdom:
		printf("<dom %p>", valdom(val));
		break;
	case Qns:
		printf("<ns %p>", valns(val));
		break;
	case Qpair:
		pair = valpair(val);
		printf("<pair %p>", pair);
		break;
	case Qrange:
		r = valrange(val);
 		printf("<range %" PRIu64 " %" PRIu64 ">",
 		       r->beg.val, r->len.val);
		break;
	case Qstr:
		str = valstr(val);
		printf("%.*s", (int)str->len, str->s);
		break;
	case Qtab:
		tab = valtab(val);
		printf("<table %p>", tab);
		break;
	case Qtype:
		t = valtype(val);
		o = fmttype(t, xstrdup(""));
		printf("<type %s>", o);
		free(o);
		break;
	case Qvec:
		vec = valvec(val);
		printf("<vector %p>", vec);
		break;
	case Qxtn:
		xtn = valxtn(val);
		printf("<typename %p>", xtn);
		break;
	default:
		printf("<unprintable type %d>", val->qkind);
		break;
	}
}

static void
xcallc(VM *vm)
{
	Imm argc;
	Val *argv;

	if(vm->clx->cfn == 0 && vm->clx->ccl == 0)
		vmerr(vm, "bad closure for builtin call");

	vm->ac = Xnil;
	argc = valimm(&vm->stack[vm->fp]);
	argv = &vm->stack[vm->fp+1];
	if(vm->clx->cfn)
		vm->clx->cfn(vm, argc, argv, &vm->ac);
	else
		vm->clx->ccl(vm, argc, argv, vm->clx->display, &vm->ac);
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
xrbeg(VM *vm, Operand *op, Operand *dst)
{
	Val v;
	Range *r;
	getvalrand(vm, op, &v);
	if(v.qkind != Qrange)
		vmerr(vm, "rbeg on non-range");
	r = valrange(&v);
	putcvalrand(vm, &r->beg, dst);
}

static void
xrlen(VM *vm, Operand *op, Operand *dst)
{
	Val v;
	Range *r;
	getvalrand(vm, op, &v);
	if(v.qkind != Qrange)
		vmerr(vm, "rlen on non-range");
	r = valrange(&v);
	putcvalrand(vm, &r->len, dst);
}

static void
xrange(VM *vm, Operand *beg, Operand *len, Operand *dst)
{
	Val begv, lenv, rv;

	getvalrand(vm, beg, &begv);
	getvalrand(vm, len, &lenv);
	if(begv.qkind != Qcval || lenv.qkind != Qcval)
		vmerr(vm, "range on non-cval");
	mkvalrange(&begv.u.cval, &lenv.u.cval, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xcval(VM *vm, Operand *type, Operand *str, Operand *dst)
{
	Val typev, strv, rv;
	Imm imm;
	Type *t;

	getvalrand(vm, type, &typev);
	getvalrand(vm, str, &strv);
	t = valtype(&typev);
	imm = str2imm(t, valstr(&strv));
	mkvalcval(t, imm, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xslices(VM *vm, Operand *str, Operand *beg, Operand *end, Operand *dst)
{
	Val strv, begv, endv, rv;
	Cval *b, *e;
	Str *r, *s;

	getvalrand(vm, str, &strv);
	getvalrand(vm, beg, &begv);
	getvalrand(vm, end, &endv);
	s = valstr(&strv);
	b = valcval(&begv);
	e = valcval(&endv);
	if(b->val > s->len)
		vmerr(vm, "string slice out of bounds");
	if(e->val > s->len)
		vmerr(vm, "string slice out of bounds");
	if(b->val > e->val)
		vmerr(vm, "string slice out of bounds");
	r = strslice(s, b->val, e->val);
	mkvalstr(r, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xlens(VM *vm, Operand *str, Operand *dst)
{
	Val strv, rv;
	Str *s;
	getvalrand(vm, str, &strv);
	s = valstr(&strv);
	mkvalcval(0, s->len, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xstr(VM *vm, Operand *cval, Operand *dst)
{
	Val cvalv, rv;
	Str *str;
	Cval *cv;
	
	getvalrand(vm, cval, &cvalv);
	cv = valcval(&cvalv);
	str = mkstrn(cv->val);
	mkvalstr(str, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xlenl(VM *vm, Operand *l, Operand *dst)
{
	Val lv, rv;
	Imm len;
	getvalrand(vm, l, &lv);
	if(listlen(&lv, &len) == 0)
		vmerr(vm, "length on non-list");
	mkvalcval(0, len, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xvecnil(VM *vm, Operand *cval, Operand *dst)
{
	Val cvalv, rv;
	Vec *vec;
	Cval *cv;

	getvalrand(vm, cval, &cvalv);
	cv = valcval(&cvalv);
	vec = mkvecnil(cv->val);
	mkvalvec(vec, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xlenv(VM *vm, Operand *vec, Operand *dst)
{
	Val vecv, rv;
	Vec *v;
	getvalrand(vm, vec, &vecv);
	v = valvec(&vecv);
	mkvalcval(0, v->len, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xvecref(VM *vm, Operand *vec, Operand *idx, Operand *dst)
{
	Val vecv, idxv;
	Vec *v;
	Cval *cv;

	getvalrand(vm, vec, &vecv);
	getvalrand(vm, idx, &idxv);
	v = valvec(&vecv);
	cv = valcval(&idxv);
	/* FIXME: check sign of cv */
	if(cv->val >= v->len)
		vmerr(vm, "vector reference out of bounds");
	putvalrand(vm, vecref(v, cv->val), dst);
}

static void
xvecset(VM *vm, Operand *vec, Operand *idx, Operand *val)
{
	Val vecv, idxv, valv;
	Vec *v;
	Cval *cv;

	getvalrand(vm, vec, &vecv);
	getvalrand(vm, idx, &idxv);
	getvalrand(vm, val, &valv);
	v = valvec(&vecv);
	cv = valcval(&idxv);
	/* FIXME: check sign of cv */
	if(cv->val >= v->len)
		vmerr(vm, "vector set out of bounds");
	vecset(vm, v, cv->val, &valv);
}

static void
xtab(VM *vm, Operand *dst)
{
	Val rv;
	Tab *tab;

	tab = mktab();
	mkvaltab(tab, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xtabdel(VM *vm, Operand *tab, Operand *key)
{
	Val tabv, keyv;
	Tab *t;

	getvalrand(vm, tab, &tabv);
	getvalrand(vm, key, &keyv);
	t = valtab(&tabv);
	tabdel(vm, t, &keyv);
}

static void
xtabenum(VM *vm, Operand *tab, Operand *dst)
{
	Val tabv, rv;
	Tab *t;
	Vec *v;

	getvalrand(vm, tab, &tabv);
	t = valtab(&tabv);
	v = tabenum(t);
	mkvalvec(v, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xtabget(VM *vm, Operand *tab, Operand *key, Operand *dst)
{
	Val tabv, keyv, *rv;
	Tab *t;

	getvalrand(vm, tab, &tabv);
	getvalrand(vm, key, &keyv);
	t = valtab(&tabv);
	rv = tabget(t, &keyv);
	if(rv)
		putvalrand(vm, rv, dst);
	else
		putvalrand(vm, &Xnil, dst);
}

static void
xtabput(VM *vm, Operand *tab, Operand *key, Operand *val)
{
	Val tabv, keyv, valv;
	Tab *t;

	getvalrand(vm, tab, &tabv);
	getvalrand(vm, key, &keyv);
	getvalrand(vm, val, &valv);
	t = valtab(&tabv);
	tabput(vm, t, &keyv, &valv);
}

static void
xxcast(VM *vm, Operand *type, Operand *cval, Operand *dst)
{
	Val typev, cvalv, rv;
	Cval *cv;
	Type *t;

	getvalrand(vm, type, &typev);
	getvalrand(vm, cval, &cvalv);
	t = valtype(&typev);
	cv = valcval(&cvalv);
	mkvalcval(t, cv->val, &rv); /* FIXME: sanity, representation change */
	putvalrand(vm, &rv, dst);
}

static void
xnull(VM *vm, Operand *dst)
{
	putvalrand(vm, &Xnulllist, dst);
}

static void
xiscl(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qcl)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xiscval(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qcval)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisnull(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qnulllist)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xispair(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qpair)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisas(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qas)
		mkvalimm(1, &rv);
	else

		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisdom(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qdom)
		mkvalimm(1, &rv);
	else

		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisns(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qns)
		mkvalimm(1, &rv);
	else

		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisrange(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qrange)
		mkvalimm(1, &rv);
	else

		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisstr(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qstr)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xistab(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qtab)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xistn(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qxtn)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xistype(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qtype)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xisvec(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == Qvec)
		mkvalimm(1, &rv);
	else
		mkvalimm(0, &rv);
	putvalrand(vm, &rv, dst);
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

static void
xvvec(VM *vm, Operand *op, Operand *dst)
{
	Val v, *vp;
	Imm sp, n, i;
	Val rv;
	Vec *vec;

	getvalrand(vm, op, &v);
	sp = valimm(&v);
	vp = &vm->stack[sp];
	n = valimm(vp);
	vec = mkvec(n);
	for(i = 0; i < n; i++)
		_vecset(vec, i, &vm->stack[sp+i+1]);
	mkvalvec(vec, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xencode(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	Str *str;
	Cval *cv;

	getvalrand(vm, op, &v);
	if(v.qkind != Qcval)
		vmerr(vm, "bad operand to encode");
	cv = valcval(&v);
	str = imm2str(cv->type, cv->val);
	mkvalstr(str, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xsizeof(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	Type *t;
	Imm imm;

	getvalrand(vm, op, &v);
	if(v.qkind != Qtype && v.qkind != Qcval)
		vmerr(vm, "bad operand to sizeof");
	if(v.qkind == Qcval)
		vmerr(vm, "sizeof cvalues not implemented");
	t = valtype(&v);
	imm = typesize(vm, t);
	mkvalcval(0, imm, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xas(VM *vm, Operand *dispatch, Operand *dst)
{
	Val dv, rv;
	Closure *cl;
	As *as;

	getvalrand(vm, dispatch, &dv);
	if(dv.qkind != Qcl)
		vmerr(vm, "mkas on non-procedure");
	cl = valcl(&dv);
	as = mkas();
	as->dispatch = cl;
	mkvalas(as, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xdom(VM *vm, Operand *nso, Operand *aso, Operand *dst)
{
	Val av, nv, rv;
	As *as;
	Ns *ns;
	Dom *dom;

	getvalrand(vm, nso, &nv);
	if(nv.qkind != Qns)
		vmerr(vm, "mkdom on non-namespace");
	ns = valns(&nv);
	getvalrand(vm, aso, &av);
	if(av.qkind != Qns)
		vmerr(vm, "mkdom on non-addrspace");
	as = valas(&av);
	dom = mkdom();
	dom->ns = ns;
	dom->as = as;
	mkvalas(as, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xdomas(VM *vm, Operand *domo, Operand *dst)
{
	Val dv, rv;
	Dom *dom;
	As *as;

	getvalrand(vm, domo, &dv);
	if(dv.qkind != Qdom)
		vmerr(vm, "domas on non-domain");
	dom = valdom(&dv);
	as = dom->as;
	mkvalas(as, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xdomns(VM *vm, Operand *domo, Operand *dst)
{
	Val dv, rv;
	Dom *dom;
	Ns *ns;

	getvalrand(vm, domo, &dv);
	if(dv.qkind != Qdom)
		vmerr(vm, "domns on non-domain");
	dom = valdom(&dv);
	ns = dom->ns;
	mkvalns(ns, &rv);
	putvalrand(vm, &rv, dst);
}

/* enumsym for namespaces constructed by @names */
static void
enumsym(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Ns *ns;

	if(argc != 0)
		vmerr(vm, "wrong number of arguments to enumsym");

	ns = valns(&disp[0]);
	mkvaltab(ns->sym, rv);
}

/* enumtype for namespaces constructed by @names */
static void
enumtype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Ns *ns;

	if(argc != 0)
		vmerr(vm, "wrong number of arguments to enumtype");

	ns = valns(&disp[0]);
	mkvaltab(ns->type, rv);
}

/* looksym for namespaces constructed by @names */
static void
looksym(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Ns *ns;
	Val *vp;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to looksym");
	if(argv->qkind != Qstr)
		vmerr(vm, "argument 1 to looksym must be a string");

	ns = valns(&disp[0]);
	vp = tabget(ns->sym, argv);
	if(vp)
		*rv = *vp;
}

/* looktype for namespaces constructed by @names */
static void
looktype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Ns *ns;
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to looktype");
	if(argv->qkind != Qxtn)
		vmerr(vm, "argument 1 to looktype must be a typename");

	ns = valns(&disp[0]);
	xtn = valxtn(&argv[0]);
	fatal("i'm not done");	/* recursive type resolve */
}

typedef
struct NSctx {
	Tab *otype, *osym;	/* bindings passed to @names */
	Tab *rawtype, *rawsym;	/* @names declarations */
	Tab *type, *sym;	/* resulting bindings */
} NSctx;

static Xtypename* resolvetypename(VM *vm, Xtypename *xtn, NSctx *ctx);

static Xtypename*
resolvetid(VM *vm, Val *xtnv, NSctx *ctx)
{
	Val *rv, v;
	Xtypename *xtn, *new;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, xtnv);
	if(rv)
		return valxtn(rv);

	/* do we have an unprocessed definition for the type? */
	rv = tabget(ctx->rawtype, xtnv);
	if(rv){
		xtn = valxtn(xtnv);
		new = mkxtn();
		new->xtkind = Ttypedef;
		new->tid = xtn->tid;

		/* bind before recursive resolve call to stop cycles */
		mkvalxtn(new, &v);
		tabput(vm, ctx->type, xtnv, &v);

		new->link = resolvetypename(vm, valxtn(rv), ctx);
		return new;
	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, xtnv);
	if(rv){
		tabput(vm, ctx->type, xtnv, rv);
		return valxtn(rv);
	}

	/* FIXME: should warn if only need ptr to type */
	xtn = valxtn(xtnv);
	vmerr(vm, "undefined type %.*s", xtn->tid->len, xtn->tid->s);
}

static Xtypename*
resolvetag(VM *vm, Val *xtnv, NSctx *ctx)
{
	Val *rv, v, *vp, *sz;
	Xtypename *xtn, *new, *tmp;
	Vec *vec, *fld, *fv;
	Imm i;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, xtnv);
	if(rv)
		return valxtn(rv);

	/* do we have an unprocessed definition for the type? */
	rv = tabget(ctx->rawtype, xtnv);
	if(rv){
		vec = valvec(rv);
		xtn = valxtn(vecref(vec, Typepos));
		fld = valvec(vecref(vec, Idpos));
		sz = vecref(vec, 2);

		new = mkxtn();
		new->xtkind = xtn->xtkind;
		new->tag = xtn->tag;
		new->field = mkvec(fld->len);
		new->sz = *sz;

		/* bind before recursive resolve call to stop cycles */
		mkvalxtn(new, &v);
		tabput(vm, ctx->type, xtnv, &v);

		for(i = 0; i < fld->len; i++){
			vec = valvec(vecref(fld, i));
			vp = vecref(vec, Typepos);
			tmp = valxtn(vp);
			tmp = resolvetypename(vm, tmp, ctx);
			fv = mkvec(3);
			
			mkvalxtn(tmp, &v);
			_vecset(fv, Typepos, &v);		/* type */
			_vecset(fv, Idpos, vecref(vec, 1));	/* id */
			_vecset(fv, 2, vecref(vec, 2));		/* offset */
			
			mkvalvec(fv, &v);
			_vecset(new->field, i, &v);
		}
		return new;
	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, xtnv);
	if(rv){
		tabput(vm, ctx->type, xtnv, rv);
		return valxtn(rv);
	}

	/* FIXME: warn if only need ptr to type */
	xtn = valxtn(xtnv);
	vmerr(vm, "undefined type %s %.*s",
	      (xtn->xtkind == Tstruct ? "struct"
	       : (xtn->xtkind == Tunion ? "union" : "enum")),
	      xtn->tag->len, xtn->tag->s);
}

static Xtypename*
resolvebase(VM *vm, Val *xtnv, NSctx *ctx)
{
	Val *rv;
	Xtypename *xtn;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, xtnv);
	if(rv)
		return valxtn(rv);

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, xtnv);
	if(rv){
		tabput(vm, ctx->type, xtnv, rv);
		return valxtn(rv);
	}
	
	xtn = valxtn(xtnv);
	vmerr(vm, "undefined base type %s", basename[xtn->basename]);
}

static Xtypename*
resolvetypename(VM *vm, Xtypename *xtn, NSctx *ctx)
{
	Vec *vec;
	Val v;
	Xtypename *tmp;
	Imm i;

	switch(xtn->xtkind){
	case Tbase:
		mkvalxtn(xtn, &v);
		return resolvebase(vm, &v, ctx);
	case Ttypedef:
		mkvalxtn(xtn, &v);
		xtn->link = resolvetid(vm, &v, ctx);
		return xtn;
	case Tstruct:
	case Tunion:
		mkvalxtn(xtn, &v);
		return resolvetag(vm, &v, ctx);
	case Tarr:
	case Tptr:
		xtn->link = resolvetypename(vm, xtn->link, ctx);
		return xtn;
	case Tfun:
		xtn->link = resolvetypename(vm, xtn->link, ctx);
		for(i = 0; i < xtn->param->len; i++){
			vec = valvec(vecref(xtn->param, i));
			tmp = valxtn(vecref(vec, Typepos));
			tmp = resolvetypename(vm, tmp, ctx);
			mkvalxtn(tmp, &v);
			vecset(vm, vec, Typepos, &v); 
		}
		return xtn;
	default:
		fatal("bug");
	}
}

static Ns*
mknstab(Tab *type, Tab *sym)
{
	Val v;
	Ns *ns;
	
	ns = mkns();
	ns->type = type;
	ns->sym = sym;
	mkvalns(ns, &v);
	ns->enumsym = mkccl("enumsym", enumsym, 1, &v);
	ns->enumtype = mkccl("enumtype", enumtype, 1, &v);
	ns->looksym = mkccl("looksym", looksym, 1, &v);
	ns->looktype = mkccl("looktype", looktype, 1, &v);
	return ns;
}

static void
xns(VM *vm, Operand *invec, Operand *dst)
{
	Val v, *idv, *vecv, *vp;
	Vec *vec;
	Tabx *x;
	Tabidx *tk;
	Xtypename *xtn;
	NSctx ctx;
	u32 i;
	Ns *ons, *ns;

	getvalrand(vm, invec, &v);
	vec = valvec(&v);
	if(vec->len != 3)
		vmerr(vm, "bad vector to ns");
	vp = vecref(vec, 0);
	ons = valns(vp);
	vp = vecref(vec, 1);
	ctx.rawtype = valtab(vp);
	vp = vecref(vec, 2);
	ctx.rawsym = valtab(vp);

	ctx.type = mktab();
	ctx.sym = mktab();

	ctx.otype = valtab(dovm(vm, ons->enumtype, 0, 0));
	ctx.osym = valtab(dovm(vm, ons->enumsym, 0, 0));

	x = ctx.rawtype->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			xtn = valxtn(&x->key[tk->idx]);
			resolvetypename(vm, xtn, &ctx);
			tk = tk->link;
		}
	}

	x = ctx.rawsym->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			/* id -> [ xtn, id, off ] */
			idv = &x->key[tk->idx];
			vecv = &x->val[tk->idx];
			vec = valvec(vecv);
			xtn = valxtn(vecref(vec, Typepos));
			xtn = resolvetypename(vm, xtn, &ctx);
			mkvalxtn(xtn, &v);
			vecset(vm, vec, Typepos, &v); /* reuse source vec */
			tabput(vm, ctx.sym, idv, vecv);
			tk = tk->link;
		}
	}

	/* inherit sym and type definitions not shadowed by @names */
	x = ctx.otype->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			vp = &x->key[tk->idx];
			if(!tabget(ctx.type, vp))
				tabput(vm, ctx.type, vp, &x->val[tk->idx]);
			tk = tk->link;
		}
	}
	x = ctx.osym->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			vp = &x->key[tk->idx];
			if(!tabget(ctx.sym, vp))
				tabput(vm, ctx.sym, vp, &x->val[tk->idx]);
			tk = tk->link;
		}
	}

	ns = mknstab(ctx.type, ctx.sym);
	mkvalns(ns, &v);
	putvalrand(vm, &v, dst);
}

static void
xnsesym(VM *vm, Operand *nso, Operand *dst)
{
	Val nv, rv;
	Ns *ns;

	getvalrand(vm, nso, &nv);
	if(nv.qkind != Qns)
		vmerr(vm, "nsesym on non-namespace");
	ns = valns(&nv);
	mkvalcl(ns->enumsym, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xnsetype(VM *vm, Operand *nso, Operand *dst)
{
	Val nv, rv;
	Ns *ns;

	getvalrand(vm, nso, &nv);
	if(nv.qkind != Qns)
		vmerr(vm, "nsesym on non-namespace");
	ns = valns(&nv);
	mkvalcl(ns->enumtype, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xnslsym(VM *vm, Operand *nso, Operand *dst)
{
	Val nv, rv;
	Ns *ns;

	getvalrand(vm, nso, &nv);
	if(nv.qkind != Qns)
		vmerr(vm, "nslsym on non-namespace");
	ns = valns(&nv);
	mkvalcl(ns->looksym, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xnsltype(VM *vm, Operand *nso, Operand *dst)
{
	Val nv, rv;
	Ns *ns;

	getvalrand(vm, nso, &nv);
	if(nv.qkind != Qns)
		vmerr(vm, "nslsym on non-namespace");
	ns = valns(&nv);
	mkvalcl(ns->looktype, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xtn(VM *vm, u8 bits, Operand *op1, Operand *op2, Operand *dst)
{
	Xtypename *xtn;
	Val v, rv;

	xtn = mkxtn();
	xtn->xtkind = TBITSTYPE(bits);
	switch(xtn->xtkind){
	case Tbase:
		xtn->basename = TBITSBASE(bits);
		break;
	case Tstruct:
	case Tunion:
	case Tenum:
		getvalrand(vm, op1, &v);
		xtn->tag = valstr(&v);
		break;
	case Ttypedef:
		getvalrand(vm, op1, &v);
		xtn->tid = valstr(&v);
		break;
	case Tptr:
		getvalrand(vm, op1, &v);
		xtn->link = valxtn(&v);
		break;
	case Tarr:
		getvalrand(vm, op1, &v);
		xtn->link = valxtn(&v);
		getvalrand(vm, op2, &v);
		xtn->cnt = v;
		break;
	case Tfun:
		getvalrand(vm, op1, &v);
		xtn->link = valxtn(&v);
		getvalrand(vm, op2, &v);
		xtn->param = valvec(&v);
		break;
	default:
		fatal("bug");
	}

	mkvalxtn(xtn, &rv);
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

int
waserror(VM *vm)
{
	return setjmp(vm->esc);
}

Val*
dovm(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	static int once;
	static Val haltv;
	static Closure *halt;
	Insn *i;
	Val val;
	Imm m, narg, onarg;

	if(!once){
		once = 1;
		gotab[Iadd]	= &&Iadd;
		gotab[Iand]	= &&Iand;
		gotab[Ias]	= &&Ias;
		gotab[Ibox]	= &&Ibox;
		gotab[Ibox0]	= &&Ibox0;
		gotab[Icall]	= &&Icall;
		gotab[Icallc]	= &&Icallc;
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
		gotab[Icval] 	= &&Icval;
		gotab[Iding] 	= &&Iding;
		gotab[Idiv] 	= &&Idiv;
		gotab[Idom]	= &&Idom;
		gotab[Idomas]	= &&Idomas;
		gotab[Idomns]	= &&Idomns;
		gotab[Iencode]	= &&Iencode;
		gotab[Iframe] 	= &&Iframe;
		gotab[Igc] 	= &&Igc;
		gotab[Ihalt] 	= &&Ihalt;
		gotab[Iinv] 	= &&Iinv;
		gotab[Iiscl] 	= &&Iiscl;
		gotab[Iiscval] 	= &&Iiscval;
		gotab[Iisas]	= &&Iisas;
		gotab[Iisdom]	= &&Iisdom;
		gotab[Iisns]	= &&Iisns;
		gotab[Iisnull] 	= &&Iisnull;
		gotab[Iispair] 	= &&Iispair;
		gotab[Iisrange]	= &&Iisrange;
		gotab[Iisstr] 	= &&Iisstr;
		gotab[Iistab] 	= &&Iistab;
		gotab[Iistn] 	= &&Iistn;
		gotab[Iistype] 	= &&Iistype;
		gotab[Iisvec] 	= &&Iisvec;
		gotab[Ijmp] 	= &&Ijmp;
		gotab[Ijnz] 	= &&Ijnz;
		gotab[Ijz] 	= &&Ijz;
		gotab[Ikg] 	= &&Ikg;
		gotab[Ikp] 	= &&Ikp;
		gotab[Ilenl]	= &&Ilenl;
		gotab[Ilens]	= &&Ilens;
		gotab[Ilenv]	= &&Ilenv;
		gotab[Imod] 	= &&Imod;
		gotab[Imov] 	= &&Imov;
		gotab[Imul] 	= &&Imul;
		gotab[Ineg] 	= &&Ineg;
		gotab[Inot] 	= &&Inot;
		gotab[Ins]	= &&Ins;
		gotab[Insesym]	= &&Insesym;
		gotab[Insetype]	= &&Insetype;
		gotab[Inslsym]	= &&Inslsym;
		gotab[Insltype]	= &&Insltype;
		gotab[Inull] 	= &&Inull;
		gotab[Ior] 	= &&Ior;
		gotab[Inop] 	= &&Inop;
		gotab[Ipanic] 	= &&Ipanic;
		gotab[Iprint] 	= &&Iprint;
		gotab[Ipush] 	= &&Ipush;
		gotab[Irange] 	= &&Irange;
		gotab[Irbeg]	= &&Irbeg;
		gotab[Iret] 	= &&Iret;
		gotab[Irlen]	= &&Irlen;
		gotab[Ishl] 	= &&Ishl;
		gotab[Ishr] 	= &&Ishr;
		gotab[Isizeof]	= &&Isizeof;
		gotab[Islices]	= &&Islices;
		gotab[Istr]	= &&Istr;
		gotab[Isub] 	= &&Isub;
		gotab[Itab]	= &&Itab;
		gotab[Itabdel]	= &&Itabdel;
		gotab[Itabenum]	= &&Itabenum;
		gotab[Itabget]	= &&Itabget;
		gotab[Itabput]	= &&Itabput;
		gotab[Itn]	= &&Itn;
		gotab[Ivec] 	= &&Ivec;
		gotab[Ivecref] 	= &&Ivecref;
		gotab[Ivecset] 	= &&Ivecset;
		gotab[Ivlist] 	= &&Ivlist;
		gotab[Ivvec] 	= &&Ivvec;
		gotab[Ixcast] 	= &&Ixcast;
		gotab[Ixor] 	= &&Ixor;

		if(!envlookup(vm->topenv, "halt", &haltv))
			fatal("broken vm");
		halt = valcl(&haltv);
	}

	/* for recursive entry, store current context */
	mkvalimm(vm->fp, &val);
	vmpush(vm, &val);	/* fp */
	vmpush(vm, &vm->cl);	/* cl */
	mkvalimm(vm->pc, &val);
	vmpush(vm, &val);	/* pc */
	mkvalimm(0, &val);
	vmpush(vm, &val);	/* narg */
	vm->fp = vm->sp;

	/* push frame for halt thunk */
	mkvalimm(vm->fp, &val);
	vmpush(vm, &val);	/* fp */
	vmpush(vm, &haltv);	/* cl */
	mkvalimm(halt->entry, &val);
	vmpush(vm, &val);	/* pc */
	for(m = argc; m > 0; m--)
		vmpush(vm, &argv[m-1]);
	mkvalimm(argc, &val);
	vmpush(vm, &val);	/* narg */
	vm->fp = vm->sp;

	/* switch to cl */
	mkvalcl(cl, &vm->cl);
	vmsetcl(vm, cl);
	vm->pc = vm->clx->entry;

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
	Icallc:
		xcallc(vm);
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
	Ipanic:
		fatal("vm panic");
	Ihalt:
		/* Ihalt is exactly like Iret... */
		vm->sp = vm->fp+valimm(&vm->stack[vm->fp])+1; /* narg+1 */
		vm->fp = valimm(&vm->stack[vm->sp+2]);
		vm->cl = vm->stack[vm->sp+1];
		vmsetcl(vm, valcl(&vm->cl));
		vm->pc = valimm(&vm->stack[vm->sp]);
		vmpop(vm, 3);

		/* ...except that it returns from dovm */
		return &vm->ac;
	Iret:
		vm->sp = vm->fp+valimm(&vm->stack[vm->fp])+1; /* narg+1 */
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
	Icar:
		xcar(vm, &i->op1, &i->dst);
		continue;
	Icdr:
		xcdr(vm, &i->op1, &i->dst);
		continue;
	Icons:
		xcons(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Irbeg:
		xrbeg(vm, &i->op1, &i->dst);
		continue;
	Irlen:
		xrlen(vm, &i->op1, &i->dst);
		continue;
	Irange:
		xrange(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Icval:
		xcval(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Istr:
		xstr(vm, &i->op1, &i->dst);
		continue;
	Islices:
		xslices(vm, &i->op1, &i->op2, &i->op3, &i->dst);
		continue;
	Ilens:
		xlens(vm, &i->op1, &i->dst);
		continue;
	Ilenl:
		xlenl(vm, &i->op1, &i->dst);
		continue;
	Ivec:
		xvecnil(vm, &i->op1, &i->dst);
		continue;
	Ivecref:
		xvecref(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Ivecset:
		xvecset(vm, &i->op1, &i->op2, &i->op3);
		continue;
	Itab:
		xtab(vm, &i->dst);
		continue;
	Itabdel:
		xtabdel(vm, &i->op1, &i->op2);
		continue;
	Itabenum:
		xtabenum(vm, &i->op1, &i->dst);
		continue;
	Itabget:
		xtabget(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Itabput:
		xtabput(vm, &i->op1, &i->op2, &i->op3);
		continue;
	Ilenv:
		xlenv(vm, &i->op1, &i->dst);
		continue;
	Ixcast:
		xxcast(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Inull:
		xnull(vm, &i->dst);
		continue;
	Iiscval:
		xiscval(vm, &i->op1, &i->dst);
		continue;
	Iiscl:
		xiscl(vm, &i->op1, &i->dst);
		continue;
	Iisas:
		xisas(vm, &i->op1, &i->dst);
		continue;
	Iisdom:
		xisdom(vm, &i->op1, &i->dst);
		continue;
	Iisns:
		xisns(vm, &i->op1, &i->dst);
		continue;
	Iisnull:
		xisnull(vm, &i->op1, &i->dst);
		continue;
	Iispair:
		xispair(vm, &i->op1, &i->dst);
		continue;
	Iisrange:
		xisrange(vm, &i->op1, &i->dst);
		continue;
	Iisstr:
		xisstr(vm, &i->op1, &i->dst);
		continue;
	Iistab:
		xistab(vm, &i->op1, &i->dst);
		continue;
	Iistn:
		xistn(vm, &i->op1, &i->dst);
		continue;
	Iistype:
		xistype(vm, &i->op1, &i->dst);
		continue;
	Iisvec:
		xisvec(vm, &i->op1, &i->dst);
		continue;
	Ivlist:
		xvlist(vm, &i->op1, &i->dst);
		continue;
	Ivvec:
		xvvec(vm, &i->op1, &i->dst);
		continue;
	Iencode:
		xencode(vm, &i->op1, &i->dst);
		continue;
	Isizeof:
		xsizeof(vm, &i->op1, &i->dst);
		continue;
	Itn:
		xtn(vm, i->bits, &i->op1, &i->op2, &i->dst);
		continue;
	Ias:
		xas(vm, &i->op1, &i->dst);
		continue;
	Idom:
		xdom(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Idomas:
		xdomas(vm, &i->op1, &i->dst);
		continue;
	Idomns:
		xdomns(vm, &i->op1, &i->dst);
		continue;
	Ins:
		xns(vm, &i->op1, &i->dst);
		continue;
	Inslsym:
		xnslsym(vm, &i->op1, &i->dst);
		continue;
	Insltype:
		xnsltype(vm, &i->op1, &i->dst);
		continue;
	Insesym:
		xnsesym(vm, &i->op1, &i->dst);
		continue;
	Insetype:
		xnsetype(vm, &i->op1, &i->dst);
		continue;
	}
}

static void
builtinfn(Env *env, char *name, Closure *cl)
{
	Val val;
	mkvalcl(cl, &val);
	envbind(env, name, &val);
}

static void
builtinstr(Env *env, char *name, char *s)
{
	Val val;
	mkvalstr(mkstr0(s), &val);
	envbind(env, name, &val);
}

static void
builtinns(Env *env, char *name, Ns *ns)
{
	Val val;
	mkvalns(ns, &val);
	envbind(env, name, &val);
}

static void
testfoo(VM *vm, Imm argc, Val *argv, Val *rv)
{
	unsigned i;

	printf("you called foo with %lld args!\n", argc);
	for(i = 0; i < argc; i++){
		printf("argv[%d] = ", i);
		printval(&argv[i]);
		printf("\n");
	}
	mkvalstr(mkstr0("returned!"), rv);
}

static void
testbin(VM *vm, Imm argc, Val *argv, Val *rv)
{
	unsigned i;
	Closure *cl;

	printf("you called it with %lld args!\n", argc);
	for(i = 0; i < argc; i++){
		printf("argv[%d] = ", i);
		printval(&argv[i]);
		printf("\n");
	}

	cl = mkcfn("testfoo", testfoo);
	rv = dovm(vm, cl, argc, argv);
	printval(rv);
	printf("returned from dovm\n");
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
	mkvalcval(0, tod, rv);
}

static void
l1_randseed(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val *arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to randseed");
	arg0 = &argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	srand((unsigned int)cv->val);
}

static void
l1_rand(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val *arg0;
	Imm r;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to randseed");
	arg0 = &argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm, "operand 1 to randseed must be an integer");

	cv = valcval(arg0);
	if(cv->val > RAND_MAX)
		vmerr(vm, "operand to rand exceeds RAND_MAX (%d)", RAND_MAX);
	if(cv->val == 0)
		vmerr(vm, "operand to rand must be positive");
	
	r = rand();
	r %= cv->val;
	mkvalcval(0, r, rv);
}

static void
l1_nslooksym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nslooksym");
	arg0 = &argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nslooksym must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	mkvalcl(ns->looksym, rv);
}

static void
l1_nslooktype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nslooktype");
	arg0 = &argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nslooktype must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	mkvalcl(ns->looktype, rv);
}

static void
l1_nsenumsym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsenumsym");
	arg0 = &argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nsenumsym must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	mkvalcl(ns->enumsym, rv);
}

static void
l1_nsenumtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsenumtype");
	arg0 = &argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nsenumtype must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	mkvalcl(ns->enumtype, rv);
}

static void
l1_printf(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *vp;
	Cval *cv;
	Str *fmts, *as;
	char *fmt, *efmt;
	char ch;
	FILE *fp = stdout;
	Xtypename *xtn;
	Vec *dvec;

	if(argc < 1)
		vmerr(vm, "wrong number of arguments to printf");
	vp = &argv[0];
	if(vp->qkind != Qstr)
		vmerr(vm, "operand 1 to printf must be a string");
	fmts = valstr(vp);
	fmt = fmts->s;
	efmt = fmt+fmts->len;
	argc--;
	vp = &argv[1];
	while(1){
		while(fmt < efmt && (ch = *fmt++) != '%')
			fputc(ch, fp);
		if(fmt >= efmt)
			return;
		ch = *fmt++;
		if(ch == '%'){
			fputc('%', fp);
			continue;
		}
		if(argc == 0)
			vmerr(vm, "format string needs more arguments");
		switch(ch){
		case 'a':
			printval(vp);
			break;
		case 'd':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			fprintf(fp, "%" PRIu64, cv->val);
			break;
		case 'x':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			fprintf(fp, "%" PRIx64, cv->val);
			break;
		case 'o':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			fprintf(fp, "%" PRIo64, cv->val);
			break;
		case 's':
			if(vp->qkind != Qstr)
				goto badarg;
			as = valstr(vp);
			fprintf(fp, "%.*s", as->len, as->s);
			break;
		case 't':
			if(vp->qkind == Qxtn)
				as = fmtxtn(valxtn(vp));
			else if(vp->qkind == Qvec){
				dvec = valvec(vp);
				if(dvec->len < 2)
					goto badarg;
				vp = vecref(dvec, Typepos);
				if(vp->qkind != Qxtn)
					goto badarg;
				xtn = valxtn(vp);
				vp = vecref(dvec, Idpos);
				if(vp->qkind == Qnil)
					as = fmtxtn(xtn);
				else if(vp->qkind == Qstr)
					as = fmtdecl(xtn, valstr(vp));
				else
					goto badarg;
			}
			fprintf(fp, "%.*s", as->len, as->s);
			break;
		default:
			vmerr(vm, "unrecognized format conversion: %%%c", ch);
		}
		argc--;
		vp++;
	}
	/* not reached */
badarg:
	vmerr(vm, "wrong type to %%%c conversion", ch);
}

static void
l1_vmbacktrace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to backtrace");
	vmbacktrace(vm);
}

static void
l1_tabkeys(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tabkeys");
	arg0 = &argv[0];
	if(arg0->qkind != Qtab)
		vmerr(vm, "operand 1 to tabkeys must be a table");
	mkvalvec(tabenumkeys(valtab(arg0)), rv);
}

static void
l1_tabvals(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tabvals");
	arg0 = &argv[0];
	if(arg0->qkind != Qtab)
		vmerr(vm, "operand 1 to tabvals must be a table");
	mkvalvec(tabenumvals(valtab(arg0)), rv);
}

static void
l1_cracktype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val xv;
	Xtypename *xtn;
	Vec *vec;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cracktype");
	xtn = valxtn(&argv[0]);
	switch(xtn->xtkind){
	case Tbase:
		vec = mkvec(2);
		mkvalstr(mkstr0(basename[xtn->basename]), &xv);
		_vecset(vec, 0, &xv);
		mkvalstr(mkstr0(repname[xtn->rep]), &xv);
		_vecset(vec, 1, &xv);
		break;
	case Tstruct:
	case Tunion:
		vec = mkvec(4);
		switch(xtn->xtkind){
		case Tstruct:
			mkvalstr(mkstr0("struct"), &xv);
			break;
		case Tunion:
			mkvalstr(mkstr0("union"), &xv);
			break;
		}
		_vecset(vec, 0, &xv);
		mkvalstr(xtn->tag, &xv);
		_vecset(vec, 1, &xv);
		mkvalvec(xtn->field, &xv);
		_vecset(vec, 2, &xv);
		_vecset(vec, 3, &xtn->sz);
		break;
	case Tenum:
		fatal("incomplete support for enums");
		break;
	case Tptr:
		vec = mkvec(1);
		mkvalxtn(xtn->link, &xv);
		_vecset(vec, 0, &xv);
		break;
	case Tarr:
		vec = mkvec(2);
		mkvalxtn(xtn->link, &xv);
		_vecset(vec, 0, &xv);
		_vecset(vec, 1, &xtn->cnt); /* nil or cval */
		break;
	case Tfun:
		vec = mkvec(2);
		mkvalxtn(xtn->link, &xv);
		_vecset(vec, 0, &xv);
		mkvalvec(xtn->param, &xv);
		_vecset(vec, 1, &xv);
		break;
	case Ttypedef:
		vec = mkvec(2);
		mkvalxtn(xtn->link, &xv);
		_vecset(vec, 0, &xv);
		mkvalstr(xtn->tid, &xv);
		_vecset(vec, 1, &xv);
		break;
	default:
		fatal("bug");
	}
	mkvalvec(vec, rv);
}

static void
dotypepredicate(VM *vm, Imm argc, Val *argv, Val *rv, char *id, unsigned kind)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", id);
	xtn = valxtn(&argv[0]);
	if(xtn->xtkind == kind)
		mkvalcval(0, 1, rv);
	else
		mkvalcval(0, 0, rv);
}

static void
l1_isbase(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isbase", Tbase);
}

static void
l1_isstruct(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isstruct", Tstruct);
}

static void
l1_isunion(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isunion", Tunion);
}

static void
l1_isenum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isenum", Tenum);
}

static void
l1_isptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isptr", Tptr);
}

static void
l1_isarray(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isarray", Tarr);
}

static void
l1_isfunc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isfunc", Tfun);
}

static void
l1_istypedef(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "istypedef", Ttypedef);
}

static void
l1_isnil(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isnil");
	if(argv->qkind == Qnil)
		mkvalcval(0, 1, rv);
	else
		mkvalcval(0, 0, rv);
}

/* FIXME: this shouldn't need a VM */
static void
addbasedef(VM *vm, Tab *type, unsigned name, unsigned rep)
{
	Xtypename *k, *v;
	Val kv, vv;
	k = mkxtn();
	k->xtkind = Tbase;
	k->basename = name;
	v = mkxtn();
	v->xtkind = Tbase;
	v->basename = name;
	v->rep = rep;
	mkvalxtn(k, &kv);
	mkvalxtn(v, &vv);
	tabput(vm, type, &kv, &vv);
}

static Ns*
mkc32lens(VM *vm)
{
	Tab *type;

	type = mktab();
	addbasedef(vm, type, Vchar,	Rs8le);
	addbasedef(vm, type, Vshort,	Rs16le);
	addbasedef(vm, type, Vint,	Rs32le);
	addbasedef(vm, type, Vlong,	Rs32le);
	addbasedef(vm, type, Vvlong,	Rs64le);
	addbasedef(vm, type, Vuchar,	Ru8le);
	addbasedef(vm, type, Vushort,	Ru16le);
	addbasedef(vm, type, Vuint,	Ru32le);
	addbasedef(vm, type, Vulong,	Ru32le);
	addbasedef(vm, type, Vuvlong,	Ru64le);
	addbasedef(vm, type, Vfloat,	Rundef);
	addbasedef(vm, type, Vdouble,	Rundef);
	addbasedef(vm, type, Vlongdouble, 	Rundef);
	addbasedef(vm, type, Vvoid,	Rundef);
	addbasedef(vm, type, Vptr,	Ru32le);

	return mknstab(type, mktab());
}

static VM *vms[Maxvms];

VM*
mkvm(Env *env)
{
	VM *vm, **vmp;

	vm = xmalloc(sizeof(VM));
	vm->fp = 0;
	vm->sp = Maxstk;
	vm->ac = Xundef;
	vm->topenv = env;
	mkvalcl(panicthunk(), &vm->cl);
	
	builtinfn(env, "testbin", mkcfn("testbin", testbin));
	builtinfn(env, "gc", gcthunk());
	builtinfn(env, "ding", dingthunk());
	builtinfn(env, "print", printthunk());
	builtinfn(env, "halt", haltthunk());
	builtinfn(env, "callcc", callcc());
	builtinfn(env, "car", carthunk());
	builtinfn(env, "cdr", cdrthunk());
	builtinfn(env, "cons", consthunk());
	builtinfn(env, "mkas", mkasthunk());
	builtinfn(env, "mkdom", mkdomthunk());
	builtinfn(env, "domas", domasthunk());
	builtinfn(env, "domns", domnsthunk());
	builtinfn(env, "mkns", mknsthunk());
	builtinfn(env, "rangebeg", rangebegthunk());
	builtinfn(env, "rangelen", rangelenthunk());
	builtinfn(env, "range", rangethunk());
	builtinfn(env, "null", nullthunk());
	builtinfn(env, "iscvalue", iscvaluethunk());
	builtinfn(env, "isas", isasthunk());
	builtinfn(env, "isdom", isdomthunk());
	builtinfn(env, "isns", isnsthunk());
	builtinfn(env, "isnull", isnullthunk());
	builtinfn(env, "ispair", ispairthunk());
	builtinfn(env, "isprocedure", isprocedurethunk());
	builtinfn(env, "isrange", israngethunk());
	builtinfn(env, "isstring", isstringthunk());
	builtinfn(env, "istable", istablethunk());
	builtinfn(env, "istypename", istnthunk());
	builtinfn(env, "istype", istypethunk());
	builtinfn(env, "isvector", isvectorthunk());
	builtinfn(env, "string", stringthunk());
	builtinfn(env, "strlen", strlenthunk());
	builtinfn(env, "substr", substrthunk());
	builtinfn(env, "table", tablethunk());
	builtinfn(env, "tabinsert", tabinsertthunk());
	builtinfn(env, "tabdelete", tabdeletethunk());
	builtinfn(env, "tabenum", tabenumthunk());
	builtinfn(env, "tablook", tablookthunk());
	builtinfn(env, "mkvec", mkvecthunk());
	builtinfn(env, "vector", vectorthunk());
	builtinfn(env, "veclen", veclenthunk());
	builtinfn(env, "vecref", vecrefthunk());
	builtinfn(env, "vecset", vecsetthunk());

	builtinfn(env, "nslooksym", mkcfn("nslooksym", l1_nslooksym));
	builtinfn(env, "nslooktype", mkcfn("nslooktype", l1_nslooktype));
	builtinfn(env, "nsenumsym", mkcfn("nsenumsym", l1_nsenumsym));
	builtinfn(env, "nsenumtype", mkcfn("nsenumtype", l1_nsenumtype));
	builtinfn(env, "gettimeofday", mkcfn("gettimeofday", l1_gettimeofday));
	builtinfn(env, "randseed", mkcfn("randseed", l1_randseed));
	builtinfn(env, "rand", mkcfn("rand", l1_rand));
	builtinfn(env, "printf", mkcfn("printf", l1_printf));
	builtinfn(env, "tabkeys", mkcfn("tabkeys", l1_tabkeys));
	builtinfn(env, "tabvals", mkcfn("tabvals", l1_tabvals));
	builtinfn(env, "vmbacktrace", mkcfn("vmbacktrace", l1_vmbacktrace));

	builtinfn(env, "cracktype", mkcfn("cracktype", l1_cracktype));
	builtinfn(env, "isbase", mkcfn("isbase", l1_isbase));
	builtinfn(env, "isstruct", mkcfn("isstruct", l1_isstruct));
	builtinfn(env, "isunion", mkcfn("isunion", l1_isunion));
	builtinfn(env, "isenum", mkcfn("isenum", l1_isenum));
	builtinfn(env, "isptr", mkcfn("isptr", l1_isptr));
	builtinfn(env, "isarray", mkcfn("isarray", l1_isarray));
	builtinfn(env, "isfunc", mkcfn("isfunc", l1_isfunc));
	builtinfn(env, "istypedef", mkcfn("istypedef", l1_istypedef));

	builtinfn(env, "isnil", mkcfn("isnil", l1_isnil));

	builtinstr(env, "$get", "get");
	builtinstr(env, "$put", "put");
	builtinstr(env, "$looksym", "looksym");
	builtinstr(env, "$looktype", "looktype");

	builtinns(env, "c32le", mkc32lens(vm));

	vmp = vms;
	while(*vmp){
		vmp++;
		if(vmp > vms+Maxvms)
			fatal("too many vms");
	}
	*vmp = vm;

	concurrentgc(vm);

	return vm;
}

void
freevm(VM *vm)
{
	VM **vmp;
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp == vm){
			*vmp = 0;
			break;
		}
		vmp++;
	}
	gckill(vm);
	free(vm);
}

static void
vmfaulthook()
{
	VM **vmp;
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp){
			fprintf(stderr, "fault context of vm %p:\n", *vmp);
			fvmbacktrace(stderr, *vmp);
			fprintf(stderr, "\n");
		}
		vmp++;
	}
}

void
initvm()
{
	Xundef.qkind = Qundef;
	Xnil.qkind = Qnil;
	Xnulllist.qkind = Qnulllist;
	kcode = contcode();
	cccode = callccode();
	setfaulthook(vmfaulthook);
}

void
finivm()
{
	unsigned i;
	Heap *hp;

	/* run two epochs without mutator to collect all objects;
	   then gcreset to free rootsets */
	gc(0);
	gc(0);
	gcreset();

	freecode((Head*)kcode);
	freecode((Head*)cccode);

	for(i = 0; i < Qnkind; i++){
		hp = &heap[i];
		if(hp->id)
			freeheap(hp);
	}
	setfaulthook(0);
}
