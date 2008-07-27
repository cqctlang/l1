#include "sys.h"
#include "util.h"
#include "l1.h"
#include "code.h"

#define HEAPPROF 0
#define HEAPDEBUG 0

typedef
enum {
	Qundef = 0,
	Qnil,
	Qnulllist,
	Qas,
	Qbox,
	Qcl,
	Qcode,
	Qcval,
	Qdom,
	Qns,
	Qpair,
	Qrange,
	Qstr,
	Qtab,
	Qvec,
	Qxtn,
	Qnkind
} Qkind;

static char *qname[Qnkind] = {
	[Qundef]=	"undefined",
	[Qnil]=		"nil",
	[Qnulllist]=	"null",
	[Qas]=		"address space",
	[Qbox]=		"box",
	[Qcl]=		"closure",
	[Qcode]=	"code",
	[Qcval]=	"cvalue",
	[Qdom]=		"domain",
	[Qns]=		"name space",
	[Qpair]=	"pair",
	[Qrange]=	"range",
	[Qstr]=		"string",
	[Qtab]=		"table",
	[Qvec]=		"vector",
	[Qxtn]=		"ctype",
};

typedef
enum Rkind {
	/* type representations */ 
	Rundef,
	Ru08le,
	Ru16le,
	Ru32le,
	Ru64le,
	Rs08le,
	Rs16le,
	Rs32le,
	Rs64le,
	Ru08be,
	Ru16be,
	Ru32be,
	Ru64be,
	Rs08be,
	Rs16be,
	Rs32be,
	Rs64be,
	Rnrep, 
} Rkind;

static char* repname[Rnrep+1] = {
	[Ru08le]=	"@u8le",
	[Ru16le]=	"@u16le",
	[Ru32le]=	"@u32le",
	[Ru64le]=	"@u64le",
	[Rs08le]=	"@s8le", 
	[Rs16le]=	"@s16le",
	[Rs32le]=	"@s32le",
	[Rs64le]=	"@s64le",
	[Ru08be]=	"@u8be", 
	[Ru16be]=	"@u16be",
	[Ru32be]=	"@u32be",
	[Ru64be]=	"@u64be",
	[Rs08be]=	"@s8be", 
	[Rs16be]=	"@s16be",
	[Rs32be]=	"@s32be",
	[Rs64be]=	"@s64be",
};

static Imm repsize[Rnrep+1] = {
	[Ru08le]=	1,
	[Ru16le]=	2,
	[Ru32le]=	4,
	[Ru64le]=	8,
	[Rs08le]=	1,
	[Rs16le]=	2,
	[Rs32le]=	4,
	[Rs64le]=	8,
	[Ru08be]=	1,
	[Ru16be]=	2,
	[Ru32be]=	4,
	[Ru64be]=	8,
	[Rs08be]=	1,
	[Rs16be]=	2,
	[Rs32be]=	4,
	[Rs64be]=	8,
};

static unsigned isunsigned[Vnbase] = {
	[Vuchar] = 1,
	[Vushort] = 1,
	[Vuint] = 1,
	[Vulong] = 1,
	[Vuvlong] = 1,
};

static unsigned isbigendian[Rnrep] = {
	[Ru08be]=	1,
	[Ru16be]=	1,
	[Ru32be]=	1,
	[Ru64be]=	1,
	[Rs08be]=	1,
	[Rs16be]=	1,
	[Rs32be]=	1,
	[Rs64be]=	1,
};

enum {
	Tabinitsize=1024,	/* power of 2 */
	Typepos=0,
	Idpos=1,
	Offpos=2,
	Maxvms=1024,
	Errinitdepth=128,	/* initial max error stack */
};

struct Heap {
	char *id;
	unsigned sz;
	void (*free1)(Head *hd);
	Head* (*iter)(Head *hd, Ictx *ictx);
	Head *alloc, *swept, *sweep, *free;
	unsigned long nalloc, nfree, nha;    /* statistics */
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
static Head *GCiterdone;

#define GCCOLOR(i) ((i)%3)
enum {
	GCfree = 3,
	GCrate = 1000,
	GCinitprot = 128,	/* initial # of GC-protected lists in VM */
};

static unsigned long long nextgctick = GCrate;

typedef struct As As;
typedef struct Box Box;
typedef struct Cval Cval;
typedef struct Dom Dom;
typedef struct Ns Ns;
typedef struct Pair Pair;
typedef struct Range Range;
typedef struct Str Str;
typedef struct Tab Tab;
typedef struct Vec Vec;
typedef struct Xtypename Xtypename;

struct Val {
	Qkind qkind;
	union {
		Head *hd;
		As *as;
		Box *box;
		Closure *cl;
		Cval *cval;
		Dom *dom;
		Ns *ns;
		Pair *pair;
		Range *range;
		Str *str;
		Tab *tab;
		Vec *vec;
		Xtypename *xtn;
	} u;
};

struct Env {
	HT *ht;
};

typedef void (Cfn)(VM *vm, Imm argc, Val *argv, Val *rv);
typedef void (Ccl)(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv);

struct Cval {
	Head hd;
	Dom *dom;
	Xtypename *type;
	Imm val;
};

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

	/* cached base type definition */
	Xtypename *base[Vnallbase];

	/* data for instances created by @names */
	/* FIXME: push into closure defining interface */
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
	Cval *beg;
	Cval *len;
};

typedef
enum Skind {
	Sperm,			/* don't free */
	Smalloc,		/* free with free() */
	Smmap,			/* free with munmap() */
} Skind;

struct Str {
	Head hd;
	Skind skind;
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
	Tkind tkind;		/* = Tbase, Tstruct, ... */
	Cbase basename;		/* base (FIXME: rename cbase) */
	Rkind rep;		/* base, ptr, enum; = Ru08le ... */
	Str *tid;		/* typedef */
	Str *tag;		/* struct, union, enum */
	Val cnt;		/* arr */
	Val sz;			/* struct, union, bitfield */
	Val bit0;		/* bitfield */
	Xtypename *link;	/* ptr, arr, func, bitfield, enum, const */
	Vec *field;		/* struct, union */
	Vec *param;		/* abstract declarators for func */
	Vec *konst;		/* constants for enum */
};

typedef
struct Err {
	jmp_buf esc;
	unsigned pdepth;	/* vm->pdepth when error label set */
} Err;

struct VM {
	Val stack[Maxstk];
	Dom *litdom;
	Ns *litns;
	Xtypename **litbase;	/* always points to litns->base */
	Str *sget, *sput, *smap;/* cached dispatch operands */
	Root **prot;		/* stack of lists of GC-protected objects */
	unsigned pdepth, pmax;	/* # live and max prot lists  */
	Env *top;
	Imm sp, fp, pc;
	Closure *clx;
	Insn *ibuf;
	Val ac, cl;
	unsigned char gcpause, gcrun;
	int cm, cgc;
	pthread_t t;
	Err *err;		/* stack of error labels */
	unsigned edepth, emax;	/* # live and max error labels */
};

static void freeval(Val *val);
static void vmsetcl(VM *vm, Closure *cl);
static void gcprotpush(VM *vm);
static void gcprotpop(VM *vm);
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
static Xtypename* chasetype(Xtypename *xtn);
static Xtypename* dolooktype(VM *vm, Xtypename *xtn, Ns *ns);
static Xtypename* mkvoidxtn();
static Xtypename* mkbasextn(Cbase name, Rkind rep);
static Xtypename* mkptrxtn(Xtypename *t, Rkind rep);
static Xtypename* mkundefxtn(Xtypename *t);
static Str* fmtxtn(Xtypename *xtn);
static void checkarg(VM *vm, char *fn, Val *argv, unsigned arg, Qkind qkind);
static int isstrcval(Cval *cv);
static Str* stringof(VM *vm, Cval *cv);
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

static void freecl(Head*);
static void freestr(Head*);
static void freetab(Head*);
static void freevec(Head*);

static Head *iteras(Head*, Ictx*);
static Head *iterbox(Head*, Ictx*);
static Head *itercl(Head*, Ictx*);
static Head *itercval(Head*, Ictx*);
static Head *iterdom(Head*, Ictx*);
static Head *iterns(Head*, Ictx*);
static Head *iterpair(Head*, Ictx*);
static Head *iterrange(Head*, Ictx*);
static Head *itertab(Head*, Ictx*);
static Head *itervec(Head*, Ictx*);
static Head *iterxtn(Head*, Ictx*);

static Heap heap[Qnkind] = {
	[Qas]	= { "as", sizeof(As), 0, iteras },
	[Qbox]	= { "box", sizeof(Box),	0, iterbox },
	[Qcval] = { "cval", sizeof(Cval), 0, itercval },
	[Qcl]	= { "closure", sizeof(Closure), freecl, itercl },
	[Qcode]	= { "code", sizeof(Code), freecode, 0 },
	[Qdom]	= { "domain", sizeof(Dom), 0, iterdom },
	[Qns]	= { "ns", sizeof(Ns), 0, iterns },
	[Qpair]	= { "pair", sizeof(Pair), 0, iterpair },
	[Qrange] = { "range", sizeof(Range), 0, iterrange },
	[Qstr]	= { "string", sizeof(Str), freestr, 0 },
	[Qtab]	= { "table", sizeof(Tab), freetab, itertab },
	[Qvec]	= { "vector", sizeof(Vec), freevec, itervec },
	[Qxtn]	= { "typename", sizeof(Xtypename), 0, iterxtn },
};

static u32 nohash(Val*);
static u32 hashcval(Val*);
static u32 hashptr(Val*);
static u32 hashconst(Val*);
static u32 hashrange(Val*);
static u32 hashstr(Val*);
static u32 hashxtn(Val*);

static int eqcval(Val*, Val*);
static int eqptr(Val*, Val*);
static int eqtrue(Val*, Val*);
static int eqrange(Val*, Val*);
static int eqstrv(Val*, Val*);
static int eqxtn(Val*, Val*);

typedef struct Hashop {
	u32 (*hash)(Val*);
	int (*eq)(Val*, Val*);
} Hashop;

static Hashop hashop[Qnkind] = {
	[Qundef] = { nohash, 0 },
	[Qnil]	= { hashconst, eqtrue },
	[Qnulllist] = { hashconst, eqtrue },
	[Qas]	= { hashptr, eqptr },
	[Qbox]	= { nohash, 0 },
	[Qcl]	= { hashptr, eqptr },
	[Qcode]	= { nohash, 0 },
	[Qcval]	= { hashcval, eqcval },
	[Qdom]	= { hashptr, eqptr },
	[Qns]	= { hashptr, eqptr },
	[Qpair]	= { hashptr, eqptr },
	[Qrange] =  { hashrange, eqrange },
	[Qstr]	= { hashstr, eqstrv },
	[Qtab]	= { hashptr, eqptr },
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

static void
heapstat(char *s)
{
	unsigned i;
	Heap *hp;

	printf("\n                              %s (epoch %lu)\n", s, gcepoch);
	printf("%-10s %10s %10s %10s\n",
	       "heap", "nalloc", "nfree", "nha");
	printf("--------------------------------"
	       "---------------------------------\n");
	for(i = 0; i < Qnkind; i++){
		hp = &heap[i];
		if(hp->id)
			printf("%-10s %10lu %10lu %10lu\n",
			       hp->id,
			       hp->nalloc, hp->nfree, hp->nha);
	}
}

static unsigned long
hlen(Head *h)
{
	unsigned n;
	while(h){
		n++;
		h = h->link;
	}
	return n;
}

// FIXME: remove sanity checks
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
		heap->nfree--;
	}else if(heap->swept){
		heap->free = (Head*)read_and_clear(&heap->swept);
		heap->nfree += hlen(heap->free);
		goto retry;
	}else{
		ap = heap->alloc;
		fp = 0;
		for(m = 0; m < AllocBatch; m++){
			o = xmalloc(heap->sz);
//			VALGRIND_MAKE_MEM_NOACCESS(o->data,
//						   heap->sz-sizeof(Head));
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
		heap->nalloc += AllocBatch;
		heap->nfree += AllocBatch;
		goto retry;
	}
	
	o->color = GCCOLOR(gcepoch);
	heap->nha++;
	if(HEAPDEBUG)
		printf("alloc %p (%s)\n", o, heap->id);

	// FIXME: only object types that do not initialize all fields
	// (e.g., Xtypename) need to be cleared.  Perhaps add a bit
	// to heap to select clearing.
//	VALGRIND_MAKE_MEM_UNDEFINED(o->data, heap->sz-sizeof(Head));
	memset(o->data, 0, heap->sz-sizeof(Head));
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
			if(HEAPDEBUG)
				printf("collect %s %p\n", heap->id, p); 
			if(p->state != 0 || p->inrootset)
				fatal("sweep heap (%s) %p bad state %d",
				      heap->id, p, p->state);
			p->link = heap->sweep;
			heap->sweep = p;
			p->state = -1;
			p->color = GCfree;
//			VALGRIND_MAKE_MEM_NOACCESS(p->data,
//						   heap->sz-sizeof(Head));
//			if(heap->swept == 0){
//				heap->swept = heap->sweep;
//				heap->sweep = 0;
//			}
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
//		if(p->color != GCfree)
//			printf("freeing heap (%s) with live data\n", heap->id);
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

static void
freerootlist(Root *r)
{
	Root *nxt;
	while(r){
		nxt = r->link;
		freeroot(r);
		r = nxt;
	}
}

/* called on roots by marker.  called on stores by mutator. */
static void
addroot(Rootset *rs, Head *h)
{
	Root *r;
	int x;

	if(h == 0)
		return;

	if(HEAPDEBUG)
		printf("addroot %s %p %d %d\n",
		       rs == &roots ? "roots" : "stores",
		       h, h->inrootset, h->state);

	if(h->inrootset)
		return;

	/* test if already on a rootlist */
	x = h->state;
	if(x > 2 || x < 0)
		fatal("addroot %p (%s) bad state %d", h, h->heap->id, x);
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
	if(HEAPDEBUG)
		printf("rmroot %p %d %d\n", h, h->inrootset, h->state);
	h->inrootset = 0;
	x = h->state;
	if(x > 2 || x <= 0)
		fatal("remove root %p (%s) bad state %d", h, h->heap->id, x);
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
		if(c == GCiterdone)
			break;
		if(c && c->color != color)
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
	Root *r;

	/* never collect these things */
	addroot(&roots, (Head*)kcode); 
	addroot(&roots, (Head*)cccode); 

	if(vm == 0)
		return;

	for(m = vm->sp; m < Maxstk; m++)
		addroot(&roots, valhead(&vm->stack[m]));
	hforeach(vm->top->ht, bindingroot, 0);
	addroot(&roots, valhead(&vm->ac));
	addroot(&roots, valhead(&vm->cl));

	
	addroot(&roots, (Head*)vm->litdom); /* assume vm->litns in litdom */
	addroot(&roots, (Head*)vm->sget);
	addroot(&roots, (Head*)vm->sput);
	addroot(&roots, (Head*)vm->smap);

	/* current GC-protected objects */
	for(m = 0; m < vm->pdepth; m++){
		r = vm->prot[m];
		while(r){
			addroot(&roots, r->hd);
			r = r->link;
		}
	}
}

static void
rootsetreset(Rootset *rs)
{
	freerootlist(rs->roots);
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
	if(HEAPPROF) heapstat("start");
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
		if(HEAPPROF) heapstat("end");
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
typesize(VM *vm, Xtypename *xtn)
{
	Cval *cv;
	Str *es;
	switch(xtn->tkind){
	case Tvoid:
		vmerr(vm, "attempt to compute size of void type");
	case Tbase:
		return repsize[xtn->rep];
	case Ttypedef:
		return typesize(vm, xtn->link);
	case Tstruct:
	case Tunion:
		cv = valcval(&xtn->sz);
		return cv->val;
	case Tenum:
		return typesize(vm, xtn->link);
	case Tptr:
		return repsize[xtn->rep];
	case Tarr:
		if(xtn->cnt.qkind != Qcval)
			vmerr(vm,
			      "attempt to compute size of unspecified array");
		cv = valcval(&xtn->cnt);
		return cv->val*typesize(vm, xtn->link);
	case Tfun:
		vmerr(vm, "attempt to compute size of function type");
	case Tbitfield:
		vmerr(vm, "attempt to compute size of bitfield"); 
	case Tundef:
		es = fmtxtn(xtn->link);
		vmerr(vm, "attempt to compute size of undefined type: %.*s",
		      es->len, es->s);
	case Tconst:
		vmerr(vm, "shouldn't this be impossible?");
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
		return GCiterdone;
	ictx->n = 1;
	return valhead(&box->v);
}

static Head*
itercl(Head *hd, Ictx *ictx)
{
	Closure *cl;
	cl = (Closure*)hd;
	if(ictx->n > cl->dlen)
		return GCiterdone;
	if(ictx->n == cl->dlen){
		ictx->n++;
		return (Head*)cl->code;
	}
	return valhead(&cl->display[ictx->n++]);
}

static Head*
itercval(Head *hd, Ictx *ictx)
{
	Cval *cval;
	cval = (Cval*)hd;

	switch(ictx->n++){
	case 0:
		return (Head*)cval->dom;
	case 1:
		return (Head*)cval->type;
	default:
		return GCiterdone;
	}
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
		return GCiterdone;
	}
}

static Head*
iterrange(Head *hd, Ictx *ictx)
{
	Range *range;
	range = (Range*)hd;

	switch(ictx->n++){
	case 0:
		return (Head*)range->beg;
	case 1:
		return (Head*)range->len;
	default:
		return GCiterdone;
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
hashconst(Val *val)
{
	switch(val->qkind){
	case Qnil:
		return hashptr32shift(&Xnil);
	case Qnulllist:
		return hashptr32shift(&Xnulllist);
	default:
		fatal("bug");
	}
}

static int
eqtrue(Val *a, Val *b)
{
	return 1;
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
	return hash6432shift(r->beg->val)^hash6432shift(r->len->val);
}

static int
eqrange(Val *a, Val *b)
{
	Range *ra, *rb;
	ra = valrange(a);
	rb = valrange(b);
	return ra->beg->val==rb->beg->val && ra->len->val==rb->len->val;
}

static u32
hashstr(Val *val)
{
	Str *s;
	s = valstr(val);
	return shash(s->s, s->len);
}

static int
eqstrc(Str *a, char *b)
{
	if(a->len != strlen(b))
		return 0;
	return memcmp(a->s, b, a->len) ? 0 : 1;
}

static int
eqstr(Str *a, Str *b)
{
	if(a->len != b->len)
		return 0;
	return memcmp(a->s, b->s, a->len) ? 0 : 1;
}

static int
eqstrv(Val *a, Val *b)
{
	Str *sa, *sb;
	sa = valstr(a);
	sb = valstr(b);
	return eqstr(sa, sb);
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
	switch(xtn->tkind){
	case Tvoid:
		return hash6432shift(xtn->tkind);
	case Tbase:
		return hash6432shift(xtn->basename);
	case Ttypedef:
		mkvalstr(xtn->tid, &v);
		return hashstr(&v)<<xtn->tkind;
	case Tstruct:
	case Tunion:
	case Tenum:
		mkvalstr(xtn->tag, &v);
		return hashstr(&v)<<xtn->tkind;
	case Tundef:
	case Tptr:
		mkvalxtn(xtn->link, &v);
		return hashxtn(&v)<<xtn->tkind;
	case Tarr:
		mkvalxtn(xtn->link, &v);
		x = hashxtn(&v)<<xtn->tkind;
		if(xtn->cnt.qkind == Qcval)
			x ^= hashcval(&xtn->cnt);
		return x;
	case Tfun:
		mkvalxtn(xtn->link, &v);
		x = hashxtn(&v)<<xtn->tkind;
		for(m = 0; m < xtn->param->len; m++){
			x <<= 1;
			vec = valvec(vecref(xtn->param, m));
			x ^= hashxtn(vecref(vec, Typepos));
		}
		return x;
	case Tbitfield:
		mkvalxtn(xtn->link, &v);
		x = hashxtn(&v)<<xtn->tkind;
		x ^= hashcval(&xtn->sz);
		return x;
	case Tconst:
		mkvalxtn(xtn->link, &v);
		return hashxtn(&v)<<xtn->tkind;
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

	if(xa->tkind != xb->tkind)
		return 0;

	switch(xa->tkind){
	case Tvoid:
		return 1;
	case Tbase:
		return xa->basename == xb->basename;
	case Ttypedef:
		return eqstr(xa->tid, xb->tid);
	case Tstruct:
	case Tunion:
	case Tenum:
		return eqstr(xa->tag, xb->tag);
	case Tundef:
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
	case Tbitfield:
		if(!eqcval(&xa->sz, &xb->sz))
			return 0;
		mkvalxtn(xa->link, &va);
		mkvalxtn(xb->link, &vb);
		return eqxtn(&va, &vb);
	case Tconst:
		mkvalxtn(xa->link, &va);
		mkvalxtn(xb->link, &vb);
		return eqxtn(&va, &vb);
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
	str->skind = Smalloc;
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
	str->skind = Smalloc;
	return str;
}

static Str*
mkstrk(char *s, unsigned long len, Skind skind)
{
	Str *str;
	str = (Str*)halloc(&heap[Qstr]);
	str->s = s;
	str->len = len;
	str->skind = skind;
	return str;
}

static Str*
mkstrn(unsigned long len)
{
	Str *str;
	str = (Str*)halloc(&heap[Qstr]);
	str->len = len;
	str->s = xmalloc(str->len);
	str->skind = Smalloc;
	return str;
}

static char*
str2cstr(Str *str)
{
	char *s;
	s = xmalloc(str->len+1);
	memcpy(s, str->s, str->len);
	return s;
}

static void
strinit(Str *str, Lits *lits)
{
	str->len = lits->len;
	str->s = xmalloc(str->len);
	str->skind = Smalloc;
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
	switch(str->skind){
	case Smmap:
		munmap(str->s, str->len);
		break;
	case Smalloc:
		free(str->s);
		break;
	case Sperm:
		break;
	}
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

static Vec*
veccopy(Vec *old)
{
	Vec *new;
	new = mkvec(old->len);
	memcpy(new->vec, old->vec, old->len*sizeof(Val));
	return new;
}

static Head*
itervec(Head *hd, Ictx *ictx)
{
	Vec *vec;
	vec = (Vec*)hd;
	if(ictx->n >= vec->len)
		return GCiterdone;
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
	u32 idx, nxt;
	Tabx *x;

	tab = (Tab*)hd;

	if(ictx->n == 0)
		ictx->x = x = tab->x;
	else
		x = ictx->x;
		
	nxt = x->nxt;		/* mutator may update nxt */
	if(ictx->n >= 2*nxt)
		return GCiterdone;
	if(ictx->n >= nxt){
		idx = ictx->n-nxt;
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
	u32 i;
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
	switch(xtn->tkind){
	case Tvoid:
	case Tbase:
		return GCiterdone;
	case Tstruct:
	case Tunion:
		switch(ictx->n++){
		case 0:
			return (Head*)xtn->tag;
		case 1:
			return (Head*)xtn->field;
		case 2:
			return valhead(&xtn->sz);
		default:
			return GCiterdone;
		}
	case Tenum:
		switch(ictx->n++){
		case 0:
			return (Head*)xtn->tag;
		case 1:
			return (Head*)xtn->link;
		case 2:
			return (Head*)xtn->konst;
		default:
			return GCiterdone;
		}
	case Tundef:
	case Tptr:
		if(ictx->n++ > 0)
			return GCiterdone;
		else
			return (Head*)xtn->link;
	case Tarr:
		switch(ictx->n++){
		case 0:
			return valhead(&xtn->cnt);
		case 1:
			return (Head*)xtn->link;
		default:
			return GCiterdone;
		}
	case Tfun:
		switch(ictx->n++){
		case 0:
			return (Head*)xtn->link;
		case 1:
			return (Head*)xtn->param;
		default:
			return GCiterdone;
		}
		break;
	case Ttypedef:
		switch(ictx->n++){
		case 0:
			return (Head*)xtn->link;
		case 1:
			return (Head*)xtn->tid;
		default:
			return GCiterdone;
		}
	case Tbitfield:
		switch(ictx->n++){
		case 0:
			return valhead(&xtn->sz);
		case 1:
			return valhead(&xtn->bit0);
		case 2:
			return (Head*)xtn->link;
		default:
			return GCiterdone;
		}
	case Tconst:
		switch(ictx->n++){
		case 0:
			return (Head*)xtn->link;
		default:
			return GCiterdone;
		}
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
	unsigned m, leno;
	Str *s;
	Cval *cv;

	leno = strlen(o);
	switch(xtn->tkind){
	case Tvoid:
		m = 4+1+leno+1;
		buf = xmalloc(m);
		if(leno)
			snprintf(buf, m, "void %s", o);
		else
			snprintf(buf, m, "void");
		free(o);
		return buf;
	case Tbase:
		m = strlen(basename[xtn->basename])+1+leno+1;
		buf = xmalloc(m);
		if(leno)
			snprintf(buf, m, "%s %s", basename[xtn->basename], o);
		else
			snprintf(buf, m, "%s", basename[xtn->basename]);
		free(o);
		return buf;
	case Ttypedef:
		s = xtn->tid;
		m = s->len+1+leno+1;
		buf = xmalloc(m);
		if(leno)
			snprintf(buf, m, "%.*s %s", s->len, s->s, o);
		else
			snprintf(buf, m, "%.*s", s->len, s->s);
		free(o);
		return buf;
	case Tstruct:
	case Tunion:
	case Tenum:
		w = tkindstr[xtn->tkind];
		if(xtn->tag){
			s = xtn->tag;
			m = strlen(w)+1+s->len+1+leno+1;
			buf = xmalloc(m);
			if(leno)
				snprintf(buf, m, "%s %.*s %s",
					 w, s->len, s->s, o);
			else
				snprintf(buf, m, "%s %.*s", w, s->len, s->s);
		}else{
			m = strlen(w)+1+leno+1;
			buf = xmalloc(m);
			if(leno)
				snprintf(buf, m, "%s %s", w, o);
			else
				snprintf(buf, m, "%s", w);
		}
		free(o);
		return buf;
	case Tundef:
		m = leno+1+strlen("/*UNDEFINED*/")+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s /*UNDEFINED*/", o);
		return _fmtxtn(xtn->link, buf);
	case Tptr:
		m = 2+leno+1+1;
		buf = xmalloc(m);
		if(xtn->link->tkind == Tfun || xtn->link->tkind == Tarr)
			snprintf(buf, m, "(*%s)", o);
		else
			snprintf(buf, m, "*%s", o);
		free(o);
		return _fmtxtn(xtn->link, buf);
	case Tarr:
		m = leno+1+10+1+1;	/* assume max 10 digit size */
		buf = xmalloc(m);
		if(xtn->cnt.qkind == Qnil)
			snprintf(buf, m, "%s[]", o);
		else{
			cv = valcval(&xtn->cnt);
			snprintf(buf, m, "%s[%" PRIu64 "]", o, cv->val);
		}
		free(o);
		return _fmtxtn(xtn->link, buf);
	case Tfun:
		pl = fmtplist(xtn->param);
		m = leno+1+strlen(pl)+1+1;
		buf = xmalloc(m);
		snprintf(buf, m, "%s(%s)", o, pl);
		free(o);
		free(pl);
		return _fmtxtn(xtn->link, buf);
	case Tconst:
	case Tbitfield:
		/* this is questionable...maybe we want more accurate printed
		   representation of these objects, especially bitfields */
		return _fmtxtn(xtn->link, o);
	default:
		fatal("bug");
	}
	return NULL;

}

static char*
_fmtdecl(Xtypename *xtn, Str *id)
{
	char *o;
	o = xmalloc(id->len+1);
	memcpy(o, id->s, id->len);
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
		return GCiterdone;
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
		return GCiterdone;
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
	unsigned n;

	ns = (Ns*)hd;
	n = ictx->n++;
	switch(n){
	case 0:
		return (Head*)ns->sym;
	case 1:
		return (Head*)ns->type;
	case 2:
		return (Head*)ns->looksym;
	case 3:
		return (Head*)ns->looktype;
	case 4:
		return (Head*)ns->enumtype;
	case 5:
		return (Head*)ns->enumsym;
	}
	if(n >= 5+Vnbase) /* assume elements at+above nbase are aliases */
		return GCiterdone;
	return (Head*)ns->base[n-5];
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

	v = hget(env->ht, id, strlen(id));
	if(!v){
		v = xmalloc(sizeof(Val));
		hput(env->ht, xstrdup(id), strlen(id), v);
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
	free(id);
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

static Cval*
mkcval(Dom *dom, Xtypename *type, Imm val)
{
	Cval *cv;
	cv = (Cval*)halloc(&heap[Qcval]);
	cv->dom = dom;
	cv->type = type;
	cv->val = val;
	return cv;
}

static void
mkvalcval(Dom *dom, Xtypename *t, Imm imm, Val *vp)
{
	vp->qkind = Qcval;
	vp->u.cval = mkcval(dom, t, imm);
}

/* the difference between mkvalimm and mkvalcval is usage:
   the former is for VM literals, the latter for other values */
static void
mkvalimm(Dom *dom, Xtypename *t, Imm imm, Val *vp)
{
	mkvalcval(dom, t, imm, vp);
}

static void
mkvalcval2(Cval *cv, Val *vp)
{
	vp->qkind = Qcval;
	vp->u.cval = cv;
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

static Range *
mkrange(Cval *beg, Cval *len)
{
	Range *r;
	r = (Range*)halloc(&heap[Qrange]);
	r->beg = beg;
	r->len = len;
	return r;
}

static void
mkvalrange(Cval *beg, Cval *len, Val *vp)
{
	Range *r;
	r = mkrange(beg, len);
	vp->qkind = Qrange;
	vp->u.range = r;
}

static void
mkvalrange2(Range *r, Val *vp)
{
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
	return v->u.cval->val;
}

static Cval*
valcval(Val *v)
{
	if(v->qkind != Qcval)
		fatal("valcval on non-cval");
	return v->u.cval;
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
	return v->u.box->v.u.cval;
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
		fprintf(out, "%20s\t(builtin %s)\n", cl->id,
			cl->cfn ? "function" : "closure");
		return;
	}

	while(1){
		if(code->labels[pc] && code->labels[pc]->src){
			fprintf(out, "%20s\t(%s:%u)\n", cl->id,
				code->labels[pc]->src->filename,
				code->labels[pc]->src->line);
			return;
		}
		if(pc == 0)
			break;
		pc--;
	}
	fprintf(out, "%20s\t(no source information)\n", cl->id);
}

static void
fvmbacktrace(FILE *out, VM *vm)
{
	Imm pc, fp, narg;
	Closure *cl;

	pc = vm->pc-1;		/* vm loop increments pc after fetch */
	fp = vm->fp;
	cl = vm->clx;
	while(fp != 0){
//		if(!strcmp(cl->id, "$halt"))
//			fprintf(out, "\t-- vmcall --\n");
//		else
//			printsrc(out, cl, pc);
		if(strcmp(cl->id, "$halt"))
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
	nexterror(vm);
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
			mkvalimm(vm->litdom, vm->litbase[Vuint], vm->sp, vp);
			return;
		case Rfp:
			mkvalimm(vm->litdom, vm->litbase[Vuint], vm->fp, vp);
			return;
		case Rpc:
			mkvalimm(vm->litdom, vm->litbase[Vuint], vm->pc, vp);
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
			vmerr(vm, "reference to undefined variable: %s",
			      topvecid(loc->idx, vm->clx->code->topvec));
		*vp = *p;
		return;
	default:
		fatal("bug");
	}
}

static Cval*
getcval(VM *vm, Location *loc)
{
	Val *p;

	switch(loc->kind){
	case Lreg:
		switch(loc->idx){
		case Rac:
			return valcval(&vm->ac);
		case Rsp:
			return mkcval(vm->litdom, vm->litbase[Vint], vm->sp);
		case Rfp:
			return mkcval(vm->litdom, vm->litbase[Vint], vm->fp);
		case Rpc:
			return mkcval(vm->litdom, vm->litbase[Vint], vm->pc);
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
			vmerr(vm, "reference to undefined variable: %s",
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
	case Oliti:
		mkvalcval(vm->litdom, vm->litbase[r->u.liti.base],
			  r->u.liti.val, vp);
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
getcvalrand(VM *vm, Operand *r)
{
	switch(r->okind){
	case Oloc:
		return getcval(vm, &r->u.loc);
	case Oliti:
		return mkcval(vm->litdom, vm->litbase[r->u.liti.base],
			      r->u.liti.val);
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
getbeint(char *s, unsigned nb)
{
	unsigned i;
	Imm w;
	w = 0;
	for(i = 0; i < nb; i++){
		w <<= 8;
		w |= (s[i]&0xff);
	}
	return w;
}

static Imm
str2imm(Xtypename *xtn, Str *str)
{
	char *s;

	xtn = chasetype(xtn);
	if(xtn->tkind != Tbase && xtn->tkind != Tptr)
		fatal("str2imm on non-scalar type");

	s = str->s;
	switch(xtn->rep){
	case Rs08le:
		return *(s8*)s;
	case Rs16le:
		return *(s16*)s;
	case Rs32le:
		return *(s32*)s;
	case Rs64le:
		return *(s64*)s;
	case Ru08le:
		return *(u8*)s;
	case Ru16le:
		return *(u16*)s;
	case Ru32le:
		return *(u32*)s;
	case Ru64le:
		return *(u64*)s;
	case Ru08be:
		return (u8)getbeint(s, 1);
	case Ru16be:
		return (u16)getbeint(s, 2);
	case Ru32be:
		return (u32)getbeint(s, 4);
	case Ru64be:
		return (u64)getbeint(s, 8);
	case Rs08be:
		return (s8)getbeint(s, 1);
	case Rs16be:
		return (s16)getbeint(s, 2);
	case Rs32be:
		return (s32)getbeint(s, 4);
	case Rs64be:
		return (s64)getbeint(s, 8);
	default:
		fatal("bug");
	}	
}

static void
putbeint(char *p, Imm w, unsigned nb)
{
	int i;
	for(i = nb-1; i >= 0; i--){
		p[i] = w&0xff;
		w >>= 8;
	}
}

static Str*
imm2str(Xtypename *xtn, Imm imm)
{
	Str *str;
	char *s;

	xtn = chasetype(xtn);
	if(xtn->tkind != Tbase && xtn->tkind != Tptr)
		fatal("imm2str on non-scalar type");

	switch(xtn->rep){
	case Rs08le:
		str = mkstrn(sizeof(s8));
		s = str->s;
		*(s8*)s = (s8)imm;
		return str;
	case Rs16le:
		str = mkstrn(sizeof(s16));
		s = str->s;
		*(s16*)s = (s16)imm;
		return str;
	case Rs32le:
		str = mkstrn(sizeof(s32));
		s = str->s;
		*(s32*)s = (s32)imm;
		return str;
	case Rs64le:
		str = mkstrn(sizeof(s64));
		s = str->s;
		*(s64*)s = (s64)imm;
		return str;
	case Ru08le:
		str = mkstrn(sizeof(u8));
		s = str->s;
		*(u8*)s = (u8)imm;
		return str;
	case Ru16le:
		str = mkstrn(sizeof(u16));
		s = str->s;
		*(u16*)s = (u16)imm;
		return str;
	case Ru32le:
		str = mkstrn(sizeof(u32));
		s = str->s;
		*(u32*)s = (u32)imm;
		return str;
	case Ru64le:
		str = mkstrn(sizeof(u64));
		s = str->s;
		*(u64*)s = (u64)imm;
		return str;
	case Rs08be:
		str = mkstrn(sizeof(s8));
		s = str->s;
		putbeint(s, imm, 1);
		return str;
	case Rs16be:
		str = mkstrn(sizeof(s16));
		s = str->s;
		putbeint(s, imm, 2);
		return str;
	case Rs32be:
		str = mkstrn(sizeof(s32));
		s = str->s;
		putbeint(s, imm, 4);
		return str;
	case Rs64be:
		str = mkstrn(sizeof(s64));
		s = str->s;
		putbeint(s, imm, 8);
		return str;
	case Ru08be:
		str = mkstrn(sizeof(u8));
		s = str->s;
		putbeint(s, imm, 1);
		return str;
	case Ru16be:
		str = mkstrn(sizeof(u16));
		s = str->s;
		putbeint(s, imm, 2);
		return str;
	case Ru32be:
		str = mkstrn(sizeof(u32));
		s = str->s;
		putbeint(s, imm, 4);
		return str;
	case Ru64be:
		str = mkstrn(sizeof(u64));
		s = str->s;
		putbeint(s, imm, 8);
		return str;
	default:
		fatal("bug");
	}	
}

/* transform representation of VAL used for type OLD to type NEW */
static Imm
rerep(Imm val, Xtypename *old, Xtypename *new)
{
	/* FIXME: non-trivial cases are : real <-> int,
	   real <-> alternate real
	   integer truncation
	   (so div and shr work)
	*/
 	return val;
}

static Cval*
typecast(VM *vm, Xtypename *xtn, Cval *cv)
{
	return mkcval(cv->dom, xtn, rerep(cv->val, cv->type, xtn));
}

static Cval*
domcast(VM *vm, Dom *dom, Cval *cv)
{
	Xtypename *xtn;
	Str *es;

	/* FIXME: do we really want to lookup the type in the new domain? */
	if(dom == vm->litdom)
		xtn = dolooktype(vm, chasetype(cv->type), dom->ns);
	else
		xtn = dolooktype(vm, cv->type, dom->ns);
	if(xtn == 0){
		es = fmtxtn(cv->type);
		vmerr(vm, "cast to domain that does not define %.*s",
		      es->len, es->s);
	}
	return mkcval(dom, xtn, rerep(cv->val, cv->type, xtn));
}

static void
dompromote(VM *vm, ikind op, Cval *op1, Cval *op2, Cval **rv1, Cval **rv2)
{
	Xtypename *b1, *b2;
	static char *domerr
		= "attempt to combine cvalues of incompatible domains"; 

	if(op1->dom == op2->dom)
		goto out;

	if(op1->dom == vm->litdom)
		op1 = domcast(vm, op2->dom, op1);
	else if(op2->dom == vm->litdom)
		op2 = domcast(vm, op1->dom, op2);
	else{
		b1 = chasetype(op1->type);
		b2 = chasetype(op2->type);
		if(b1->tkind != Tptr && b2->tkind != Tptr){
			op1 = domcast(vm, vm->litdom, op1);
			op2 = domcast(vm, vm->litdom, op2);
		}else if(b1->tkind == Tptr && b2->tkind == Tptr)
			vmerr(vm, domerr);
		else if(b1->tkind == Tptr){
			if(op == Iadd || op == Isub)
				op2 = domcast(vm, op1->dom, op2);
			else
				vmerr(vm, domerr);
		}else /* b2->tkind == Tptr */ {
			if(op == Iadd || op == Isub)
				op1 = domcast(vm, op2->dom, op1);
			else
				vmerr(vm, domerr);
		}
	}
out:
	*rv1 = op1;
	*rv2 = op2;
}

static Cval*
intpromote(VM *vm, Cval *cv)
{
	Xtypename *base;

	base = chasetype(cv->type);
	if(base->tkind != Tbase)
		return cv;
	switch(base->basename){
	case Vuchar:
	case Vushort:
	case Vchar:
	case Vshort:
		return typecast(vm, cv->dom->ns->base[Vint], cv);
	default:
		break;
	}
	return cv;
}

static void
usualconvs(VM *vm, Cval *op1, Cval *op2, Cval **rv1, Cval **rv2)
{
	/* FIXME: why assign ranks to types below int promotion? */
	static unsigned rank[Vnbase] = {
		[Vchar] = 0,
		[Vuchar] = 0,
		[Vshort] = 1,
		[Vushort] = 1,
		[Vint] = 2,
		[Vuint] = 2,
		[Vlong] = 3,
		[Vulong] = 3,
		[Vvlong] = 4,
		[Vuvlong] = 4,
	};
	static unsigned uvariant[Vnbase] = {
		[Vchar] = Vuchar,
		[Vshort] = Vushort,
		[Vint] = Vuint,
		[Vlong] = Vulong,
		[Vvlong] = Vuvlong,
	};
	Cbase c1, c2, nc;
	Xtypename *b1, *b2, *nxtn;

	op1 = intpromote(vm, op1);
	op2 = intpromote(vm, op2);

	b1 = chasetype(op1->type);
	b2 = chasetype(op2->type);
	if(b1->tkind != Tbase || b2->tkind != Tbase){
		/* presumably one of them is a Tptr; nothing else is sane. */
		*rv1 = op1;
		*rv2 = op2;
		return;
	}

	c1 = b1->basename;
	c2 = b2->basename;
	if(c1 == c2){
		*rv1 = op1;
		*rv2 = op2;
		return;
	}

	if((isunsigned[c1] && isunsigned[c2])
	   || (!isunsigned[c1] && !isunsigned[c2])){
		if(rank[c1] < rank[c2])
			nc = c2;
		else
			nc = c1;
	}else if(isunsigned[c1] && rank[c1] >= rank[c2])
		nc = c1;
	else if(isunsigned[c2] && rank[c2] >= rank[c1])
		nc = c2;
	else if(!isunsigned[c1] && repsize[b1->rep] > repsize[b2->rep])
		nc = c1;
	else if(!isunsigned[c2] && repsize[b2->rep] > repsize[b1->rep])
		nc = c2;
	else if(!isunsigned[c1])
		nc = uvariant[c1];
	else
		nc = uvariant[c2];

	nxtn = op1->dom->ns->base[nc];          /* op2 has the same domain */
	if(c1 != nc)
		op1 = typecast(vm, nxtn, op1);
	if(c2 != nc)
		op2 = typecast(vm, nxtn, op2);
	*rv1 = op1;
	*rv2 = op2;
}

static void
printval(Val *val)
{
	Cval *cv;
	Closure *cl;
	Pair *pair;
	Range *r;
	Str *str;
	Val bv;
	Tab *tab;
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
		else if(cl->dlen > 0)
			printf("<closure %s>", cl->id);
		else
			printf("<procedure %s>", cl->id);
		break;
	case Qundef:
//		printf("<undefined>");
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
 		       r->beg->val, r->len->val);
		break;
	case Qstr:
		str = valstr(val);
		printf("%.*s", (int)str->len, str->s);
		break;
	case Qtab:
		tab = valtab(val);
		printf("<table %p>", tab);
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

void
printvmac(VM *vm)
{
	if(vm->ac.qkind != Qnil){
		printval(&vm->ac);
		printf("\n");
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

static int
Strcmp(Str *s1, Str *s2)
{
	unsigned char *p1, *p2;
	u32 l1, l2;

	p1 = (unsigned char*)s1->s;
	p2 = (unsigned char*)s2->s;
	l1 = s1->len;
	l2 = s2->len;
	while(l1 && l2){
		if(*p1 < *p2)
			return -1;
		else if(*p1 > *p2)
			return 1;
		p1++;
		p2++;
		l1--;
		l2--;
	}
	if(l1)
		return 1;
	else if(l2)
		return -1;
	else
		return 0;
}

static Imm
binopstr(VM *vm, ikind op, Str *s1, Str *s2)
{
	int x;

	switch(op){
	case Icmpeq:
		return eqstr(s1, s2);
	case Icmpneq:
		return !eqstr(s1, s2);
	case Icmpgt:
		x = Strcmp(s1, s2);
		return x > 0 ? 1 : 0;
	case Icmpge:
		x = Strcmp(s1, s2);
		return x >= 0 ? 1 : 0;
	case Icmplt:
		x = Strcmp(s1, s2);
		return x < 0 ? 1 : 0;
	case Icmple:
		x = Strcmp(s1, s2);
		return x <= 0 ? 1 : 0;
	default:
		vmerr(vm, "attempt to apply %s to string operands", opstr[op]);
	}
}

static void
xunop(VM *vm, ikind op, Operand *op1, Operand *dst)
{
	Val rv;
	Cval *cv, *cvr;
	Imm imm, nv;
	
	cv = getcvalrand(vm, op1);
	cv = intpromote(vm, cv);
	imm = cv->val;

	switch(op){
	case Ineg:
		nv = -imm;
		cvr = mkcval(cv->dom, cv->type, nv);
		break;
	case Iinv:
		nv = ~imm;
		cvr = mkcval(cv->dom, cv->type, nv);
		break;
	case Inot:
		if(imm)
			nv = 0;
		else
			nv = 1;
		cvr = mkcval(vm->litdom, vm->litbase[Vint], nv);
		break;
	default:
		fatal("unknown unary operator %d", op);
	}
	mkvalcval2(cvr, &rv);
	putvalrand(vm, &rv, dst);
}

static Imm
truncimm(Imm v, Rkind rep)
{
	switch(rep){
	case Rs08be:
	case Rs08le:
		return (s8)v;
	case Rs16be:
	case Rs16le:
		return (s16)v;
	case Rs32be:
	case Rs32le:
		return (s32)v;
	case Rs64be:
	case Rs64le:
		return (s64)v;
	case Ru08be:
	case Ru08le:
		return (u8)v;
	case Ru16be:
	case Ru16le:
		return (u16)v;
	case Ru32be:
	case Ru32le:
		return (u32)v;
	case Ru64be:
	case Ru64le:
		return (u64)v;
	default:
		fatal("bug");
	}
}

static Cval*
xcvalptralu(VM *vm, ikind op, Cval *op1, Cval *op2,
	    Xtypename *t1, Xtypename *t2)
{
	Xtypename *sub;
	Cval *ptr;
	Imm sz, osz, n;

	if(op != Iadd && op != Isub)
		vmerr(vm, "attempt to apply %s to pointer operand", opstr[op]);

	if(t1->tkind == Tptr && t2->tkind == Tptr){
		if(op != Isub)
			vmerr(vm, "attempt to apply %s to pointer operands",
			      opstr[op]);
		sub = chasetype(t1->link);
		/* special case: sizeof(void)==1 for pointer arith */
		if(sub->tkind == Tvoid)
			sz = 1;
		else
			sz = typesize(vm, sub);
		sub = chasetype(t2->link);
		if(sub->tkind == Tvoid)
			osz = 1;
		else
			osz = typesize(vm, sub);
		if(sz != osz)
			vmerr(vm, "attempt to subtract pointers to "
			      "objects of different sizes");
		if(sz == 0)
			vmerr(vm, "attempt to subtract pointers to "
			      "zero-sized objects");
		n = (op1->val-op2->val)/sz;
		n = truncimm(n, t1->rep);
		/* FIXME: define ptrdiff_t? */
		return mkcval(vm->litdom, vm->litbase[Vlong], n);
	}

	/* exactly one operand is a pointer */

	if(t1->tkind == Tptr){
		sub = chasetype(t1->link);
		ptr = op1;
		n = op2->val;
	}else{
		sub = chasetype(t2->link);
		ptr = op2;
		n = op1->val;
	}

	/* special case: sizeof(void)==1 for pointer arith */
	if(sub->tkind == Tvoid)
		sz = 1;
	else
		sz = typesize(vm, sub);
	/* FIXME: what about undefined sub types? */
	if(op == Iadd)
		n = ptr->val+n*sz;
	else
		n = ptr->val-n*sz;
	n = truncimm(n, ptr->type->rep);
	return mkcval(ptr->dom, ptr->type, n);
}

static Cval*
xcvalalu(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	Imm i1, i2, rv;
	Xtypename *t1, *t2;

	dompromote(vm, op, op1, op2, &op1, &op2);
	usualconvs(vm, op1, op2, &op1, &op2);
	t1 = chasetype(op1->type);
	t2 = chasetype(op2->type);
	if(t1->tkind == Tptr || t2->tkind == Tptr)
		return xcvalptralu(vm, op, op1, op2, t1, t2);

	i1 = op1->val;
	i2 = op2->val;
	
	switch(op){
	case Iadd:
		rv = i1+i2;
		break;
	case Idiv:
		if(i2 == 0)
			vmerr(vm, "divide by zero");
		rv = i1/i2;
		break;
	case Imod:
		if(i2 == 0)
			vmerr(vm, "divide by zero");
		rv = i1%i2;
		break;
	case Imul:
		rv = i1*i2;
		break;
	case Isub:
		rv = i1-i2;
		break;
	case Iand:
		rv = i1&i2;
		break;
	case Ixor:
		rv = i1^i2;
		break;
	case Ior:
		rv = i1|i2;
		break;
	default:
		fatal("bug");
	}		

	rv = truncimm(rv, t1->rep);
	return mkcval(op1->dom, op1->type, rv);
}

static Cval*
xcvalshift(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	Imm i1, i2, rv;

	/* no need to rationalize domains */
	op1 = intpromote(vm, op1);
	op2 = intpromote(vm, op2);
	i1 = op1->val;
	i2 = op2->val;

	/* following C99:
	   - (both) if op2 is negative or >= width of op1,
	            the result is undefined;
	   - (<<)   if op1 is signed and op1*2^op2 is not representable in
	            typeof(op1), the result is undefined;
	   - (>>)   if op1 is signed and negative, the result is
                    implementation-defined; gcc's >> performs sign extension
	*/
	switch(op){
	case Ishl:
		rv = i1<<i2;
		break;
	case Ishr:
		rv = i1>>i2;
		break;
	default:
		fatal("bug");
	}
	return mkcval(op1->dom, op1->type, rv);
}

static Cval*
xcvalcmp(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	Imm i1, i2, rv;

	dompromote(vm, op, op1, op2, &op1, &op2);
	usualconvs(vm, op1, op2, &op1, &op2);
	i1 = op1->val;
	i2 = op2->val;

	/* We're intentionally relaxed about whether one operand is
	   pointer so that expressions like (p == 0x<addr>) can be
	   written without cast clutter. */

	if(isunsigned[op1->type->basename])
		switch(op){
		case Icmpeq:
			rv = i1==i2;
			break;
		case Icmpneq:
			rv = i1!=i2;
			break;
		case Icmpgt:
			rv = i1>i2;
			break;
		case Icmpge:
			rv = i1>=i2;
			break;
		case Icmplt:
			rv = i1<i2;
			break;
		case Icmple:
			rv = i1<=i2;
			break;
		default:
			fatal("bug");
		}
	else
		switch(op){
		case Icmpeq:
			rv = (s64)i1==(s64)i2;
			break;
		case Icmpneq:
			rv = (s64)i1!=(s64)i2;
			break;
		case Icmpgt:
			rv = (s64)i1>(s64)i2;
			break;
		case Icmpge:
			rv = (s64)i1>=(s64)i2;
			break;
		case Icmplt:
			rv = (s64)i1<(s64)i2;
			break;
		case Icmple:
			rv = (s64)i1<=(s64)i2;
			break;
		default:
			fatal("bug");
		}
	return mkcval(vm->litdom, vm->litbase[Vint], rv);
}

static void
xbinop(VM *vm, ikind op, Operand *op1, Operand *op2, Operand *dst)
{
	Val v1, v2, rv;
	Cval *cv1, *cv2, *cvr;
	Str *s1, *s2;
	Imm nv;

	getvalrand(vm, op1, &v1);
	getvalrand(vm, op2, &v2);

	if(v1.qkind == Qcval && v2.qkind == Qcval){
		cv1 = valcval(&v1);
		cv2 = valcval(&v2);
		switch(op){
		case Iadd:
		case Iand:
		case Idiv:
		case Imod:
		case Imul:
		case Ior:
		case Isub:
		case Ixor:
			cvr = xcvalalu(vm, op, cv1, cv2);
			break;
		case Ishl:
		case Ishr:
			cvr = xcvalshift(vm, op, cv1, cv2);
			break;
		case Icmplt:
		case Icmple:
		case Icmpgt:
		case Icmpge:
		case Icmpeq:
		case Icmpneq:
			cvr = xcvalcmp(vm, op, cv1, cv2);
			break;
		default:
			fatal("bug");
		}
		mkvalcval2(cvr, &rv);
		putvalrand(vm, &rv, dst);
		return;
	}

	if(v1.qkind == Qstr && v2.qkind == Qstr){
		s1 = valstr(&v1);
		s2 = valstr(&v2);
dostr:
		nv = binopstr(vm, op, s1, s2);
		mkvalcval(vm->litdom, vm->litbase[Vint], nv, &rv);
		putvalrand(vm, &rv, dst);
		return;
	}

	if(v1.qkind == Qstr && v2.qkind == Qcval){
		cv2 = valcval(&v2);
		if(isstrcval(cv2)){
			s1 = valstr(&v1);
			s2 = stringof(vm, cv2);
			goto dostr;
		}
		/* fall through */
	}

	if(v2.qkind == Qstr && v1.qkind == Qcval){
		cv1 = valcval(&v1);
		if(isstrcval(cv1)){
			s1 = stringof(vm, cv1);
			s2 = valstr(&v2);
			goto dostr;
		}
		/* fall through */
	}

	/* all other types support only == and != */
	if(op != Icmpeq && op != Icmpneq)
		vmerr(vm, "incompatible operands for binary %s", opstr[op]);

	if(v1.qkind != v2.qkind)
		nv = 0;
	else
		nv = hashop[v1.qkind].eq(&v1, &v2);
	if(op == Icmpneq)
		nv = !nv;
	mkvalcval(vm->litdom, vm->litbase[Vint], nv, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xclo(VM *vm, Operand *dl, Ctl *label, Operand *dst)
{
	Closure *cl;
	Val rv;
	Cval *cv;
	Imm m, len;

	/* dl is number of values to copy from stack into display */
	/* label points to instruction in current closure's code */
	/* captured variables are in display order on stack,
	   from low to high stack address */

	cv = getcvalrand(vm, dl);
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
	Cval *cv;
	cv = getcvalrand(vm, src);
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
	putcvalrand(vm, r->beg, dst);
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
	putcvalrand(vm, r->len, dst);
}

static void
xrange(VM *vm, Operand *beg, Operand *len, Operand *dst)
{
	Val begv, lenv, rv;

	getvalrand(vm, beg, &begv);
	getvalrand(vm, len, &lenv);
	if(begv.qkind != Qcval || lenv.qkind != Qcval)
		vmerr(vm, "range on non-cval");
	mkvalrange(begv.u.cval, lenv.u.cval, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xref(VM *vm, Operand *dom, Operand *type, Operand *cval, Operand *dst)
{
	Val domv, typev, cvalv, rv;
	Xtypename *t, *b, *pt;
	Dom *d;
	Cval *cv;
	Imm imm;

	getvalrand(vm, type, &typev);
	getvalrand(vm, cval, &cvalv);
	getvalrand(vm, dom, &domv);
	d = valdom(&domv);
	t = valxtn(&typev);
	cv = valcval(&cvalv);

	b = chasetype(t);
	switch(b->tkind){
	case Tvoid:
	case Tbase:
	case Tundef:
	case Tptr:
	case Tstruct:
	case Tunion:
	case Tfun:
	case Tenum:
		/* construct pointer */
		pt = mkptrxtn(t, d->ns->base[Vptr]->rep);
		imm = truncimm(cv->val, pt->rep);
		mkvalcval(d, pt, imm, &rv);
		break;
	case Tarr:
		/* construct pointer to first element */
		pt = mkptrxtn(t->link, d->ns->base[Vptr]->rep);
		imm = cv->val;
		imm = truncimm(imm, pt->rep);
		mkvalcval(d, pt, imm, &rv);
		break;
	case Tbitfield:
		vmerr(vm, "attempt to construct pointer to a bitfield");
		break;
	case Tconst:
		vmerr(vm, "attempt to use enumeration constant as location");
	default:
		fatal("bug");
	}
	putvalrand(vm, &rv, dst);
}

static int
dobitfieldgeom(Cval *addr, Xtypename *b, BFgeom *bfg)
{
	Cval *bit0, *bs;
	Xtypename *bb;
		
	bit0 = valcval(&b->bit0);
	bs = valcval(&b->sz);
	bfg->bp = 8*addr->val+bit0->val; /* FIXME: overflow bug */
	bfg->bs = bs->val;
	bb = chasetype(b->link);
	bfg->isbe = isbigendian[bb->rep];
	return bitfieldgeom(bfg);
}

static void
xcval(VM *vm, Operand *dom, Operand *type, Operand *cval, Operand *dst)
{
	Val domv, typev, cvalv, rv, *p, argv[2];
	Imm imm;
	Dom *d;
	Xtypename *t, *b, *pt;
	Cval *cv, *len;
	Str *s, *es;
	BFgeom bfg;

	getvalrand(vm, type, &typev);
	getvalrand(vm, cval, &cvalv);
	getvalrand(vm, dom, &domv);
	d = valdom(&domv);
	t = valxtn(&typev);
	cv = valcval(&cvalv);

	b = chasetype(t);
	switch(b->tkind){
	case Tbitfield:
		if(b->link->tkind == Tundef){
			es = fmtxtn(b->link->link);
			vmerr(vm, "attempt to read object of undefined type: "
			      "%.*s", es->len, es->s);
		}
		if(0 > dobitfieldgeom(cv, b, &bfg))
			vmerr(vm, "invalid bitfield access");
		mkvalstr(vm->sget, &argv[0]);
		mkvalrange(mkcval(vm->litdom, vm->litbase[Vptr], bfg.addr),
			   mkcval(vm->litdom, vm->litbase[Vptr], bfg.cnt),
			   &argv[1]);
		p = dovm(vm, d->as->dispatch, 2, argv);
		s = valstr(p);
		imm = bitfieldget(s->s, &bfg);
		mkvalcval(d, b->link, imm, &rv);
		break;
	case Tconst:
		mkvalcval(d, b->link, cv->val, &rv);
		break;
	case Tbase:
	case Tptr:
	case Tenum:
		len = mkcval(vm->litdom, vm->litbase[Vptr], typesize(vm, t));
		mkvalstr(vm->sget, &argv[0]);
		mkvalrange(cv, len, &argv[1]);
		p = dovm(vm, d->as->dispatch, 2, argv);
		imm = str2imm(t, valstr(p));
		mkvalcval(d, t, imm, &rv);
		break;
	case Tarr:
		/* construct pointer to first element */
		pt = mkptrxtn(t->link, d->ns->base[Vptr]->rep);
		imm = truncimm(cv->val, pt->rep);
		mkvalcval(d, pt, imm, &rv);
		break;
	case Tvoid:
	case Tfun:
	case Tstruct:
	case Tunion:
		vmerr(vm,
		      "attempt to read %s-valued object from address space",
		      tkindstr[b->tkind]);
	case Tundef:
		es = fmtxtn(b->link);
		vmerr(vm, "attempt to read object of undefined type: %.*s",
		      es->len, es->s);
	default:
		fatal("bug");
	}
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
	mkvalcval(vm->litdom, vm->litbase[Vuint], s->len, &rv);
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
	mkvalcval(vm->litdom, vm->litbase[Vuint], len, &rv);
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
	mkvalcval(vm->litdom, vm->litbase[Vuint], v->len, &rv);
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
xxcast(VM *vm, Operand *typeordom, Operand *cval, Operand *dst)
{
	Val typeordomv, cvalv, rv;
	Cval *cv;
	Dom *d;
	Xtypename *t;

	getvalrand(vm, cval, &cvalv);
	if(cvalv.qkind != Qcval)
		vmerr(vm,
		      "operand 2 to extended cast operator must be a cvalue");
	cv = valcval(&cvalv);

	getvalrand(vm, typeordom, &typeordomv);
	if(typeordomv.qkind == Qxtn){
		t = valxtn(&typeordomv);
		mkvalcval2(typecast(vm, t, cv), &rv);
	}else if(typeordomv.qkind == Qdom){
		d = valdom(&typeordomv);
		mkvalcval2(domcast(vm, d, cv), &rv);
	}else
		vmerr(vm,
		      "operand 2 to extended cast operator must be "
		      "a domain or ctype");
	putvalrand(vm, &rv, dst);
}

static void
xnull(VM *vm, Operand *dst)
{
	putvalrand(vm, &Xnulllist, dst);
}

static void
xis(VM *vm, Operand *op, unsigned kind, Operand *dst)
{
	Val v, rv;
	getvalrand(vm, op, &v);
	if(v.qkind == kind)
		mkvalimm(vm->litdom, vm->litbase[Vuint], 1, &rv);
	else
		mkvalimm(vm->litdom, vm->litbase[Vuint], 0, &rv);
	putvalrand(vm, &rv, dst);
}

static void
xiscl(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qcl, dst);
}

static void
xiscval(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qcval, dst);
}

static void
xisnull(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qnulllist, dst);
}

static void
xispair(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qpair, dst);
}

static void
xisas(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qas, dst);
}

static void
xisdom(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qdom, dst);
}

static void
xisns(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qns, dst);
}

static void
xisrange(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qrange, dst);
}

static void
xisstr(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qstr, dst);
}

static void
xistab(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qtab, dst);
}

static void
xistn(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qxtn, dst);
}

static void
xisvec(VM *vm, Operand *op, Operand *dst)
{
	xis(vm, op, Qvec, dst);
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
	Imm imm;
	Xtypename *xtn;
	Cval *cv;

	getvalrand(vm, op, &v);
	if(v.qkind == Qcval){
		cv = valcval(&v);
		xtn = cv->type;
	}else if(v.qkind == Qxtn)
		xtn = valxtn(&v);
	else
		vmerr(vm, "operand 1 to sizeof must be a type or cvalue");
	imm = typesize(vm, xtn);
	mkvalcval(vm->litdom, vm->litbase[Vuint], imm, &rv);
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
	if(av.qkind != Qas)
		vmerr(vm, "mkdom on non-addrspace");
	as = valas(&av);
	dom = mkdom();
	dom->ns = ns;
	dom->as = as;
	mkvaldom(dom, &rv);
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
		vmerr(vm, "domns on non-domain (is %d)", dv.qkind);
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

/* dispatch for abstract address spaces */
static void
nodispatch(VM *vm, Imm argc, Val *argv, Val *rv)
{
	vmerr(vm, "attempt to access abstract address space");
}

static As*
mkabas()
{
	As *as;
	as = mkas();
	as->dispatch = mkcfn("nodispatch", nodispatch);
	return as;
}

static void
masdispatch(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s, *cmd, *dat;
	Range *r;
	Vec *v;
	Cval *beg, *end;
	Val val;

	s = valstr(&disp[0]);
	if(argc < 1)
		vmerr(vm,
		      "wrong number of arguments to address space dispatch");
	checkarg(vm, "masdispatch", argv, 0, Qstr);
	cmd = valstr(&argv[0]);
	if(eqstrc(cmd, "get")){
		if(argc != 2)
			vmerr(vm, "wrong number of arguments to get");
		checkarg(vm, "masdispatch", argv, 1, Qrange);
		r = valrange(&argv[1]);
		beg = r->beg;
		end = xcvalalu(vm, Iadd, beg, r->len);
		if(beg->val > s->len)
			vmerr(vm, "out of bounds string access");
		if(end->val > s->len)
			vmerr(vm, "out of bounds string access");
		if(beg->val > end->val)
			vmerr(vm, "out of bounds string access");
		dat = strslice(s, beg->val, end->val);
		mkvalstr(dat, rv);
	}else if(eqstrc(cmd, "put")){
		if(argc != 3)
			vmerr(vm, "wrong number of arguments to put");
		checkarg(vm, "masdispatch", argv, 1, Qrange);
		checkarg(vm, "masdispatch", argv, 2, Qstr);
		r = valrange(&argv[1]);
		dat = valstr(&argv[2]);
		beg = r->beg;
		end = xcvalalu(vm, Iadd, beg, r->len);
		if(beg->val > s->len)
			vmerr(vm, "out of bounds string access");
		if(end->val > s->len)
			vmerr(vm, "out of bounds string access");
		if(beg->val > end->val)
			vmerr(vm, "out of bounds string access");
		if(dat->len < r->len->val)
			vmerr(vm, "short put");
		/* FIXME: rationalize with l1_strput */
		memcpy(s->s+beg->val, dat->s, dat->len);
	}else if(eqstrc(cmd, "map")){
		if(argc != 1)
			vmerr(vm, "wrong number of arguments to map");
		v = mkvec(1);
		mkvalrange(mkcval(vm->litdom, vm->litbase[Vptr], 0),
			   mkcval(vm->litdom, vm->litbase[Vptr], s->len),
			   &val);
		_vecset(v, 0, &val);
		mkvalvec(v, rv);
	}else
		vmerr(vm, "undefined operation on address space: %.*s",
		      cmd->len, cmd->len);
}

static As*
mkmas(Str *s)
{
	Val v;
	As *as;
	mkvalstr(s, &v);
	as = mkas();
	as->dispatch = mkccl("masdispatch", masdispatch, 1, &v);
	return as;
}

static Xtypename*
chasetype(Xtypename *xtn)
{
	if(xtn->tkind == Ttypedef || xtn->tkind == Tenum)
		return chasetype(xtn->link);
	return xtn;
}

/* call looktype operator of NS on typename XTN */
static Xtypename*
dolooktype(VM *vm, Xtypename *xtn, Ns *ns)
{
	Val v, *rv;
	Xtypename *tmp, *xtmp, *new;
	Vec *vec;
	Imm i;
	Str *es;

	switch(xtn->tkind){
	case Tbase:
	case Ttypedef:
	case Tstruct:
	case Tunion:
	case Tenum:
		mkvalxtn(xtn, &v);
		rv = dovm(vm, ns->looktype, 1, &v);
		if(rv->qkind == Qnil)
			return 0;
		return valxtn(rv);
	case Tptr:
		new = gcprotect(vm, mkxtn());
		new->tkind = Tptr;
		new->link = dolooktype(vm, xtn->link, ns);
		tmp = ns->base[Vptr];
		new->rep = tmp->rep;
		return new;
	case Tarr:
		new = gcprotect(vm, mkxtn());
		new->tkind = Tarr;
		new->link = dolooktype(vm, xtn->link, ns);
		if(new->link == 0){
			es = fmtxtn(xtn->link);
			vmerr(vm, "name space does not define %.*s",
			      es->len, es->s);
		}
		new->cnt = xtn->cnt;
		return new;
	case Tfun:
		new = gcprotect(vm, mkxtn());
		new->tkind = Tfun;
		new->link = dolooktype(vm, xtn->link, ns);
		new->param = mkvec(xtn->param->len);
		for(i = 0; i < xtn->param->len; i++){
			vec = veccopy(valvec(vecref(xtn->param, i)));
			xtmp = valxtn(vecref(vec, Typepos));
			tmp = dolooktype(vm, xtmp, ns);
			if(tmp == 0){
				es = fmtxtn(xtmp);
				vmerr(vm, "name space does not define %.*s",
				      es->len, es->s);
			}
			mkvalxtn(tmp, &v);
			vecset(vm, vec, Typepos, &v); 
			mkvalvec(vec, &v);
			vecset(vm, new->param, i, &v);
		}
		return new;
	case Tundef:
		/* FIXME: do we want this? */
		return dolooktype(vm, xtn->link, ns);
	case Tconst:
	case Tbitfield:
	default:
		fatal("bug");
	}
}

static void
nscache1base(VM *vm, Ns *ns, Cbase cb)
{
	Val v;
	Xtypename *xtn;

	/* will be garbage; safe across dolooktype
	   because as an argument to looktype it will
	   be in vm roots */
	xtn = mkbasextn(cb, Rundef);
	mkvalxtn(xtn, &v);
	xtn = dolooktype(vm, xtn, ns);
	if(xtn == 0)
		vmerr(vm, "name space does not define %s",
		      basename[cb]);
	ns->base[cb] = xtn;
}

static void
nscachebase(VM *vm, Ns *ns)
{
	Cbase cb;
	for(cb = Vchar; cb < Vnbase; cb++)
		nscache1base(vm, ns, cb);
	nscache1base(vm, ns, Vptr);
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
	Val *vp;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to looktype");
	if(argv->qkind != Qxtn)
		vmerr(vm, "argument 1 to looktype must be a typename");

	ns = valns(&disp[0]);
	vp = tabget(ns->type, argv);
	if(vp)
		*rv = *vp;
}

typedef
struct NSctx {
	Tab *otype, *osym;	/* bindings passed to @names */
	Tab *rawtype, *rawsym;	/* @names declarations */
	Tab *type, *sym;	/* resulting bindings */
	Tab *undef;		/* undefined type names */
	Rkind ptrrep;		/* pointer representation */
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
		new->tkind = Ttypedef;
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

	tabput(vm, ctx->undef, xtnv, xtnv);
	xtn = valxtn(xtnv);
	return mkundefxtn(xtn);
}

static Xtypename*
resolvetag(VM *vm, Val *xtnv, NSctx *ctx)
{
	Val *rv, v, *vp, *sz;
	Xtypename *xtn, *new, *tmp;
	Vec *vec, *fld, *fv;
	Imm i;
	Str *es;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, xtnv);
	if(rv)
		return valxtn(rv);

	/* do we have an unprocessed definition for the type? */
	rv = tabget(ctx->rawtype, xtnv);
	if(rv){
		xtn = valxtn(rv);
		switch(xtn->tkind){
		case Tstruct:
		case Tunion:
			fld = xtn->field;
			sz = &xtn->sz;

			new = mkxtn();
			new->tkind = xtn->tkind;
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
				mkvalxtn(tmp, &v);
				fv = mkvec(3);
				_vecset(fv, Typepos, &v);
				_vecset(fv, Idpos, vecref(vec, 1));
				_vecset(fv, Offpos, vecref(vec, 2));
				mkvalvec(fv, &v);
				_vecset(new->field, i, &v);
			}
			return new;
		case Tenum:
			new = mkxtn();
			new->tkind = Tenum;
			new->tag = xtn->tag;
			new->konst = xtn->konst;

			mkvalxtn(new, &v);
			tabput(vm, ctx->type, xtnv, &v);
			new->link = resolvetypename(vm, xtn->link, ctx);
			return new;
		default:
			es = fmtxtn(xtn);
			vmerr(vm, "bad definition for tagged type: %.*s",
			      es->len, es->s);
		}

	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, xtnv);
	if(rv){
		tabput(vm, ctx->type, xtnv, rv);
		return valxtn(rv);
	}

	tabput(vm, ctx->undef, xtnv, xtnv);
	xtn = valxtn(xtnv);
	return mkundefxtn(xtn);
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
	Vec *vec, *pv;
	Val v;
	Xtypename *tmp;
	Imm i;
	Xtypename *new;

	switch(xtn->tkind){
	case Tvoid:
		return mkvoidxtn();
	case Tbase:
		mkvalxtn(xtn, &v);
		return resolvebase(vm, &v, ctx);
	case Ttypedef:
		mkvalxtn(xtn, &v);
//		new = mkxtn();
//		new->tkind = Ttypedef;
//		new->tid = xtn->tid;
//		new->link = resolvetid(vm, &v, ctx);
		new = resolvetid(vm, &v, ctx);
		return new;
	case Tenum:
	case Tstruct:
	case Tunion:
		mkvalxtn(xtn, &v);
		new = resolvetag(vm, &v, ctx);
		return new;
	case Tbitfield:
		new = mkxtn();
		new->tkind = xtn->tkind;
		new->bit0 = xtn->bit0;
		new->sz = xtn->sz;
		new->link = resolvetypename(vm, xtn->link, ctx);
		return new;
	case Tconst:
		new = mkxtn();
		new->tkind = xtn->tkind;
		new->link = resolvetypename(vm, xtn->link, ctx);
		return new;
	case Tptr:
		new = mkxtn();
		new->tkind = xtn->tkind;
		new->rep = ctx->ptrrep;
		new->link = resolvetypename(vm, xtn->link, ctx);
		return new;
	case Tarr:
		new = mkxtn();
		new->tkind = xtn->tkind;
		new->cnt = xtn->cnt;
		new->link = resolvetypename(vm, xtn->link, ctx);
		return new;
	case Tfun:
		new = mkxtn();
		new->tkind = Tfun;
		new->link = resolvetypename(vm, xtn->link, ctx);
		new->param = mkvec(xtn->param->len);
		for(i = 0; i < xtn->param->len; i++){
			vec = valvec(vecref(xtn->param, i));
			tmp = valxtn(vecref(vec, Typepos));
			tmp = resolvetypename(vm, tmp, ctx);
			pv = mkvec(2);
			mkvalxtn(tmp, &v);
			_vecset(pv, Typepos, &v);
			_vecset(pv, Idpos, vecref(vec, Idpos));
			mkvalvec(pv, &v);
			_vecset(new->param, i, &v);
		}
		return new;
	case Tundef:
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

static Ns*
dorawns(VM *vm, Ns *ons, Tab *rawtype, Tab *rawsym)
{
	Val v, *idv, *vecv, *vp;
	Vec *vec;
	Tabx *x;
	Tabidx *tk;
	Xtypename *xtn;
	NSctx ctx;
	u32 i;
	Ns *ns;
	Imm m;
	Str *as;

	ctx.rawtype = rawtype;
	ctx.rawsym = rawsym;

	ctx.type = mktab();
	ctx.sym = mktab();
	ctx.undef = mktab();

	ctx.otype = valtab(dovm(vm, ons->enumtype, 0, 0));
	ctx.osym = valtab(dovm(vm, ons->enumsym, 0, 0));

	/* get pointer representation from parent name space */
	xtn = mkxtn();		/* will be garbage */
	xtn->tkind = Tbase;
	xtn->basename = Vptr;
	mkvalxtn(xtn, &v);
	vp = tabget(ctx.otype, &v);
	if(vp == 0)
		vmerr(vm,
		      "derived name space must define pointer representation");
	xtn = valxtn(vp);
	ctx.ptrrep = xtn->rep;
	xtn = 0;

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
			if(xtn != 0){
				mkvalxtn(xtn, &v);
				vecset(vm, vec, Typepos, &v); /* reuse vec */
				tabput(vm, ctx.sym, idv, vecv);
			}
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

	vec = tabenum(ctx.undef);
	m = vec->len/2;
	if(m > 0){
		printf("warning: name space references undefined "
		       "type%s:\n", m > 1 ? "s" : "");
		while(m != 0){
			m--;
			vp = vecref(vec, m);
			xtn = valxtn(vp);
			as = fmtxtn(xtn);
			printf("\t%.*s\n", as->len, as->s);
		}
	}

	ns = mknstab(ctx.type, ctx.sym);
	nscachebase(vm, ns);
	return ns;
}

static void
xns(VM *vm, Operand *invec, Operand *dst)
{
	Val v, *vp;
	Vec *vec;
	Ns *ons, *ns;
	Tab *rawtype, *rawsym;

	getvalrand(vm, invec, &v);
	vec = valvec(&v);
	if(vec->len != 3)
		vmerr(vm, "bad vector to ns");
	vp = vecref(vec, 0);
	ons = valns(vp);
	vp = vecref(vec, 1);
	rawtype = valtab(vp);
	vp = vecref(vec, 2);
	rawsym = valtab(vp);

	ns = dorawns(vm, ons, rawtype, rawsym);
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

jmp_buf*
_pusherror(VM *vm)
{
	Err *ep;
	if(vm->edepth >= vm->emax){
		vm->err = xrealloc(vm->err, vm->emax*sizeof(Err),
				   2*vm->emax*sizeof(Err));
		vm->emax *= 2;
	}
	ep = &vm->err[vm->edepth++];
	ep->pdepth = vm->pdepth;
	return &ep->esc;
}

void
nexterror(VM *vm)
{
	Err *ep;
	if(vm->edepth == 0)
		fatal("bad error stack discipline");
	vm->edepth--;
	ep = &vm->err[vm->edepth];
	while(vm->pdepth > ep->pdepth)
		gcprotpop(vm);
	longjmp(ep->esc, 1);
}

void
poperror(VM *vm)
{
	if(vm->edepth == 0)
		fatal("bad error stack discipline");
	vm->edepth--;
}

static void
gcprotpush(VM *vm)
{
	if(vm->pdepth >= vm->pmax){
		vm->prot = xrealloc(vm->prot,
				    vm->pmax*sizeof(Root*),
				    2*vm->pmax*sizeof(Root*));
		vm->pmax *= 2;
	}
	vm->pdepth++;
}

static void
gcprotpop(VM *vm)
{
	freerootlist(vm->prot[vm->pdepth-1]);
	vm->prot[vm->pdepth-1] = 0;
	vm->pdepth--;
}

void*
gcprotect(VM *vm, void *obj)
{
	Root *r;

	r = newroot();
	r->hd = obj;
	r->link = vm->prot[vm->pdepth-1];
	vm->prot[vm->pdepth-1] = r;	
	return obj;
}

void
vmreset(VM *vm)
{
	while(vm->pdepth > 0)
		gcprotpop(vm);
	vm->edepth = 0;
	vm->fp = 0;
	vm->sp = Maxstk;
	vm->ac = Xundef;
	mkvalcl(panicthunk(), &vm->cl);
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
		gotab[Iref] 	= &&Iref;
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
		gotab[Ivec] 	= &&Ivec;
		gotab[Ivecref] 	= &&Ivecref;
		gotab[Ivecset] 	= &&Ivecset;
		gotab[Ivlist] 	= &&Ivlist;
		gotab[Ivvec] 	= &&Ivvec;
		gotab[Ixcast] 	= &&Ixcast;
		gotab[Ixor] 	= &&Ixor;

		if(!envlookup(vm->top, "halt", &haltv))
			fatal("broken vm");
		halt = valcl(&haltv);
	}

	gcprotpush(vm);

	/* for recursive entry, store current context */
	mkvalimm(vm->litdom, vm->litbase[Vuint], vm->fp, &val);
	vmpush(vm, &val);	/* fp */
	vmpush(vm, &vm->cl);	/* cl */
	mkvalimm(vm->litdom, vm->litbase[Vuint], vm->pc, &val);
	vmpush(vm, &val);	/* pc */
	mkvalimm(vm->litdom, vm->litbase[Vuint], 0, &val);
	vmpush(vm, &val);	/* narg */
	vm->fp = vm->sp;

	/* push frame for halt thunk */
	mkvalimm(vm->litdom, vm->litbase[Vuint], vm->fp, &val);
	vmpush(vm, &val);	/* fp */
	vmpush(vm, &haltv);	/* cl */
	mkvalimm(vm->litdom, vm->litbase[Vuint], halt->entry, &val);
	vmpush(vm, &val);	/* pc */
	for(m = argc; m > 0; m--)
		vmpush(vm, &argv[m-1]);
	mkvalimm(vm->litdom, vm->litbase[Vuint], argc, &val);
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
		mkvalimm(vm->litdom, vm->litbase[Vuint], vm->fp, &val);
		vmpush(vm, &val);
		vmpush(vm, &vm->cl);
		mkvalimm(vm->litdom, vm->litbase[Vuint],
			 i->dstlabel->insn, &val);
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
		gcprotpop(vm);
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
		xcval(vm, &i->op1, &i->op2, &i->op3, &i->dst);
		continue;
	Iref:
		xref(vm, &i->op1, &i->op2, &i->op3, &i->dst);
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
builtindom(Env *env, char *name, Dom *dom)
{
	Val val;
	mkvaldom(dom, &val);
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
checkarg(VM *vm, char *fn, Val *argv, unsigned arg, Qkind qkind)
{
	if(argv[arg].qkind != qkind)
		vmerr(vm, "operand %d to %s must be a %s",
		      arg+1, fn, qname[qkind]);
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
	mkvalcval(vm->litdom, vm->litbase[Vulong], tod, rv);
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
	mkvalcval(vm->litdom, vm->litbase[Vulong], r, rv);
}

static void
l1_asdispatch(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;
	Dom *dom;
	As *as;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to asdispatch");
	arg0 = &argv[0];
	if(arg0->qkind != Qas && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nslooksym must be an addrspace or domain");
	if(arg0->qkind == Qas)
		as = valas(arg0);
	else{
		dom = valdom(arg0);
		as = dom->as;
	}
	mkvalcl(as->dispatch, rv);
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
l1_nsptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsptr");
	arg0 = &argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nsptr must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	mkvalxtn(ns->base[Vptr], rv);
}

static void
l1_looktype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	Xtypename *xtn;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looktype");

	if(argv[0].qkind == Qns)
		ns = valns(&argv[0]);
	else if(argv[0].qkind == Qdom){
		dom = valdom(&argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to looktype must be a namespace or domain");
		      
	if(argv[1].qkind != Qxtn)
		vmerr(vm, "operand 1 to looktype must be a typename");
	xtn = valxtn(&argv[1]);

	xtn = dolooktype(vm, xtn, ns);
	if(xtn)
		mkvalxtn(xtn, rv);
}

static void
l1_domof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val *arg0;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to domof");
	arg0 = &argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm,
		      "operand 1 to domof must be a cvalue");
	cv = valcval(arg0);
	mkvaldom(cv->dom, rv);
}

static void
fprinticval(FILE *fp, unsigned char conv, Cval *cv)
{
	static char* fmttab[Rnrep][256] = {
	[Ru08le]['d'] = "%"PRId8,	[Ru08le]['i'] = "%"PRIi8,   
	[Ru08be]['d'] = "%"PRId8,	[Ru08be]['i'] = "%"PRIi8,   
	[Rs08le]['d'] = "%"PRId8,	[Rs08le]['i'] = "%"PRIi8,   
	[Rs08be]['d'] = "%"PRId8,	[Rs08be]['i'] = "%"PRIi8,   
	[Ru16le]['d'] = "%"PRId16,	[Ru16le]['i'] = "%"PRIi16,  
	[Ru16be]['d'] = "%"PRId16,	[Ru16be]['i'] = "%"PRIi16,  
	[Rs16le]['d'] = "%"PRId16,	[Rs16le]['i'] = "%"PRIi16,  
	[Rs16be]['d'] = "%"PRId16,	[Rs16be]['i'] = "%"PRIi16,  
	[Ru32le]['d'] = "%"PRId32,	[Ru32le]['i'] = "%"PRIi32,  
	[Ru32be]['d'] = "%"PRId32,	[Ru32be]['i'] = "%"PRIi32,  
	[Rs32le]['d'] = "%"PRId32,	[Rs32le]['i'] = "%"PRIi32,  
	[Rs32be]['d'] = "%"PRId32,	[Rs32be]['i'] = "%"PRIi32,  
	[Ru64le]['d'] = "%"PRId64,	[Ru64le]['i'] = "%"PRIi64,  
	[Ru64be]['d'] = "%"PRId64,	[Ru64be]['i'] = "%"PRIi64,  
	[Rs64le]['d'] = "%"PRId64,	[Rs64le]['i'] = "%"PRIi64,  
	[Rs64be]['d'] = "%"PRId64,	[Rs64be]['i'] = "%"PRIi64,  

	[Ru08le]['o'] = "%"PRIo8,	[Ru08le]['u'] = "%"PRIu8, 
	[Ru08be]['o'] = "%"PRIo8,	[Ru08be]['u'] = "%"PRIu8, 
	[Rs08le]['o'] = "%"PRIo8,	[Rs08le]['u'] = "%"PRIu8, 
	[Rs08be]['o'] = "%"PRIo8,	[Rs08be]['u'] = "%"PRIu8, 
	[Ru16le]['o'] = "%"PRIo16,	[Ru16le]['u'] = "%"PRIu16,
	[Ru16be]['o'] = "%"PRIo16,	[Ru16be]['u'] = "%"PRIu16,
	[Rs16le]['o'] = "%"PRIo16,	[Rs16le]['u'] = "%"PRIu16,
	[Rs16be]['o'] = "%"PRIo16,	[Rs16be]['u'] = "%"PRIu16,
	[Ru32le]['o'] = "%"PRIo32,	[Ru32le]['u'] = "%"PRIu32,
	[Ru32be]['o'] = "%"PRIo32,	[Ru32be]['u'] = "%"PRIu32,
	[Rs32le]['o'] = "%"PRIo32,	[Rs32le]['u'] = "%"PRIu32,
	[Rs32be]['o'] = "%"PRIo32,	[Rs32be]['u'] = "%"PRIu32,
	[Ru64le]['o'] = "%"PRIo64,	[Ru64le]['u'] = "%"PRIu64,
	[Ru64be]['o'] = "%"PRIo64,	[Ru64be]['u'] = "%"PRIu64,
	[Rs64le]['o'] = "%"PRIo64,	[Rs64le]['u'] = "%"PRIu64,
	[Rs64be]['o'] = "%"PRIo64,	[Rs64be]['u'] = "%"PRIu64,

	[Ru08le]['x'] = "%"PRIx8,	[Ru08le]['X'] = "%"PRIX8, 
	[Ru08be]['x'] = "%"PRIx8,	[Ru08be]['X'] = "%"PRIX8, 
	[Rs08le]['x'] = "%"PRIx8,	[Rs08le]['X'] = "%"PRIX8, 
	[Rs08be]['x'] = "%"PRIx8,	[Rs08be]['X'] = "%"PRIX8, 
	[Ru16le]['x'] = "%"PRIx16,	[Ru16le]['X'] = "%"PRIX16,
	[Ru16be]['x'] = "%"PRIx16,	[Ru16be]['X'] = "%"PRIX16,
	[Rs16le]['x'] = "%"PRIx16,	[Rs16le]['X'] = "%"PRIX16,
	[Rs16be]['x'] = "%"PRIx16,	[Rs16be]['X'] = "%"PRIX16,
	[Ru32le]['x'] = "%"PRIx32,	[Ru32le]['X'] = "%"PRIX32,
	[Ru32be]['x'] = "%"PRIx32,	[Ru32be]['X'] = "%"PRIX32,
	[Rs32le]['x'] = "%"PRIx32,	[Rs32le]['X'] = "%"PRIX32,
	[Rs32be]['x'] = "%"PRIx32,	[Rs32be]['X'] = "%"PRIX32,
	[Ru64le]['x'] = "%"PRIx64,	[Ru64le]['X'] = "%"PRIX64,
	[Ru64be]['x'] = "%"PRIx64,	[Ru64be]['X'] = "%"PRIX64,
	[Rs64le]['x'] = "%"PRIx64,	[Rs64le]['X'] = "%"PRIX64,
	[Rs64be]['x'] = "%"PRIx64,	[Rs64be]['X'] = "%"PRIX64,	};

	char *fmt;
	Xtypename *t;

	t = chasetype(cv->type);
	fmt = fmttab[t->rep][conv];
	switch(t->rep){
	case Ru08le:
	case Ru08be:
		fprintf(fp, fmt, (u8)cv->val);
		break;
	case Rs08le:
	case Rs08be:
		fprintf(fp, fmt, (s8)cv->val);
		break;
	case Ru16le:
	case Ru16be:
		fprintf(fp, fmt, (u16)cv->val);
		break;
	case Rs16le:
	case Rs16be:
		fprintf(fp, fmt, (s16)cv->val);
		break;
	case Ru32le:
	case Ru32be:
		fprintf(fp, fmt, (u32)cv->val);
		break;
	case Rs32le:
	case Rs32be:
		fprintf(fp, fmt, (s32)cv->val);
		break;
	case Ru64le:
	case Ru64be:
		fprintf(fp, fmt, (u64)cv->val);
		break;
	case Rs64le:
	case Rs64be:
		fprintf(fp, fmt, (s64)cv->val);
		break;
	default:
		fatal("bug");
	}
}

static int
isstrcval(Cval *cv)
{
	Xtypename *t;
	t = chasetype(cv->type);
	if(t->tkind != Tptr)
		return 0;
	t = chasetype(t->link);
	if(t->tkind != Tbase)
		return 0;
	if(t->basename != Vchar && t->basename != Vuchar)
		return 0;
	return 1;
}

/* assume CV has been vetted by isstrcval */
static Str*
stringof(VM *vm, Cval *cv)
{
	Str *s;
	char *buf, *q;
	Val *p, *rp, argv[2];
	Vec *v;
	Range *r;
	Imm l, m, n, o;
	static unsigned unit = 128;
	
	mkvalstr(vm->smap, &argv[0]);
	p = dovm(vm, cv->dom->as->dispatch, 1, argv);
	if(p->qkind != Qvec)
		vmerr(vm, "address space map returned invalid value");

	/* FIXME: turn this into function mapstab or somesuch */
	v = valvec(p);
	for(m = 0; m < v->len; m++){
		rp = vecref(v, m);
		if(rp->qkind != Qrange)
			vmerr(vm, "address space map returned invalid value");
		r = valrange(rp);
		/* FIXME: we assume compare w/o cast/rerep is okay */
		if(r->beg->val > cv->val)
			continue;
		if(r->beg->val+r->len->val <= cv->val)
			r = 0;
		break;
	}
	if(r == 0)
		vmerr(vm, "out-of-bounds address space access");

	l = 0;
	m = r->beg->val+r->len->val-cv->val;
	o = cv->val;
	buf = 0;
	while(m > 0){
		n = MIN(m, unit);
		if(buf == 0)
			buf = xmalloc(unit);
		else
			buf = xrealloc(buf, l, l+n);
		mkvalstr(vm->sget, &argv[0]);
		r = mkrange(mkcval(cv->dom, cv->dom->ns->base[Vptr], o),
			    mkcval(cv->dom, cv->dom->ns->base[Vptr], n));
		gcprotect(vm, r);
		mkvalrange2(r, &argv[1]);
		p = dovm(vm, cv->dom->as->dispatch, 2, argv);
		s = valstr(p);
		memcpy(buf+l, s->s, s->len);
		q = strnchr(buf+l, '\0', s->len);
		if(q){
			l += q-buf+l;
			break;
		}
		l += s->len;
		o += s->len;
		m -= s->len;
	}
	s = mkstr(buf, l);	/* FIXME: mkstr copies buf; should steal */
	free(buf);
	return s;
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
	unsigned char c;

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
		case 'c':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			c = cv->val;
			if(isgraph(c) || isspace(c))
				fprintf(fp, "%c", c);
			else
				fprintf(fp, "\\%.3o", c);
			break;
		case 'd':
		case 'i':
		case 'o':
		case 'u':
		case 'x':
		case 'X':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			fprinticval(fp, ch, cv);
			break;
		case 's':
			if(vp->qkind == Qstr)
				as = valstr(vp);
			else if(vp->qkind == Qcval){
				cv = valcval(vp);
				if(!isstrcval(cv))
					goto badarg;
				as = stringof(vm, cv);
			}else
				goto badarg;
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
			}else if(vp->qkind == Qcval){
				cv = valcval(vp);
				as = fmtxtn(cv->type);
			}else
				vmerr(vm, "bad operand to %%t");
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
dotypepredicate(VM *vm, Imm argc, Val *argv, Val *rv, char *id, unsigned kind)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", id);
	if(argv[0].qkind != Qxtn)
		vmerr(vm, "operand 1 to %s must be a ctype", id);
	xtn = valxtn(&argv[0]);
	if(xtn->tkind == kind)
		mkvalcval(vm->litdom, vm->litbase[Vint], 1, rv);
	else
		mkvalcval(vm->litdom, vm->litbase[Vint], 0, rv);
}

static void
l1_isvoid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isvoid", Tvoid);
}

static void
l1_isundeftype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isundeftype", Tundef);
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
l1_issu(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to issu");
	if(argv[0].qkind != Qxtn)
		vmerr(vm, "operand 1 to issu must be a ctype");
	xtn = valxtn(&argv[0]);
	if(xtn->tkind == Tstruct || xtn->tkind == Tunion)
		mkvalcval(vm->litdom, vm->litbase[Vint], 1, rv);
	else
		mkvalcval(vm->litdom, vm->litbase[Vint], 0, rv);
}

static void
l1_isenum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isenum", Tenum);
}

static void
l1_isenumconst(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isenumconst", Tconst);
}

static void
l1_isbitfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isbitfield", Tbitfield);
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
l1_baseid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to baseid");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Tbase)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	mkvalstr(mkstr0(basename[xtn->basename]), rv);
}

static void
l1_subtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	static char *err = 
		"operand 1 to subtype must be an "
		"array, pointer, enum, or enum constant";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to subtype");
	if(argv->qkind != Qxtn)
		vmerr(vm, err);
		      
	xtn = valxtn(argv);
	if(xtn->tkind != Tptr && xtn->tkind != Tarr
	   && xtn->tkind != Tenum && xtn->tkind != Tconst
	   && xtn->tkind != Tundef)
		vmerr(vm, err);

	mkvalxtn(xtn->link, rv);
}

static void
l1_rettype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to retttype");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to rettype must be a function ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Tfun)
		vmerr(vm, "operand 1 to rettype must be a function ctype");
	mkvalxtn(xtn->link, rv);
}

static void
l1_suekind(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suekind");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to suekind must be a tagged ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion
	   && xtn->tkind != Tenum)
		vmerr(vm, "operand 1 to suekind must be a tagged ctype");
	mkvalstr(mkstr0(tkindstr[xtn->tkind]), rv);
}

static void
l1_suetag(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suetag");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to suetag must be a tagged ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion
	   && xtn->tkind != Tenum)
		vmerr(vm, "operand 1 to sutag must be a tagged ctype");
	mkvalstr(xtn->tag, rv);
}

static void
l1_susize(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to susize");
	if(argv->qkind != Qxtn)
		vmerr(vm,
		      "operand 1 to susize must be a struct or union ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to susize must be a struct or union ctype");
	*rv = xtn->sz;
}

static void
l1_bitfieldwidth(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldwidth");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to bitfieldwidth "
		      "must be a bitfield ctype");
		      
	xtn = valxtn(argv);
	if(xtn->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldwidth "
		      "must be a bitfield ctype");
	*rv = xtn->sz;
}

static void
l1_bitfieldcontainer(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldcontainer");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to bitfieldcontainer "
		      "must be a bitfield ctype");
		      
	xtn = valxtn(argv);
	if(xtn->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldcontainer "
		      "must be a bitfield ctype");
	mkvalxtn(xtn->link, rv);
}


static void
l1_bitfieldpos(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldpos");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to bitfieldpos must be a bitfield ctype");
		      
	xtn = valxtn(argv);
	if(xtn->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldpos must be a bitfield ctype");
	*rv = xtn->bit0;
}

static void
l1_arraynelm(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to arraynelm");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to arraynelm must be an array ctype");
		      
	xtn = valxtn(argv);
	if(xtn->tkind != Tarr)
		vmerr(vm, "operand 1 to arraynelm must be an array ctype");
	*rv = xtn->cnt;
}

static void
l1_typedefid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to typedefid");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to typedefid must be a typedef ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Ttypedef)
		vmerr(vm, "operand 1 to typedefid must be a typedef ctype");
	mkvalstr(xtn->tid, rv);
}

static void
l1_typedeftype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to typedeftype");
	if(argv->qkind != Qxtn)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Ttypedef)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	mkvalxtn(xtn->link, rv);
}

static void
l1_params(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to params");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to params must be a function ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Tfun)
		vmerr(vm, "operand 1 to params must be a function ctype");
	mkvalvec(xtn->param, rv);
}

static void
l1_paramtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to paramtype must be a vector returned by params";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to paramtype");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 2)
		vmerr(vm, err);
	vp = &v->vec[Typepos];
	if(vp->qkind != Qxtn)
		vmerr(vm, err);
	*rv = *vp;
}

static void
l1_paramid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to paramid must be a vector returned by params";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to paramid");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 2)
		vmerr(vm, err);
	vp = &v->vec[Idpos];
	if(vp->qkind != Qstr && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = *vp;
}

static void
l1_fields(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fields");
	if(argv->qkind != Qxtn)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	xtn = valxtn(argv);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	mkvalvec(xtn->field, rv);
}

static void
l1_lookfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	Val *vp;
	Imm i;

	static char *err1
		= "operand 1 to lookfield must be a struct or union ctype";
	static char *err2
		= "operand 2 to lookfield must be a string";

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to lookfield");
	if(argv[0].qkind != Qxtn)
		vmerr(vm, err1);
	xtn = valxtn(&argv[0]);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	if(argv[1].qkind != Qstr)
		vmerr(vm, err2);
	for(i = 0; i < xtn->field->len; i++){
		vp = vecref(xtn->field, i);
		if(eqstrv(&argv[1], vecref(valvec(vp), Idpos))){
			*rv = *vp;
			return;
		}
	}
}

static void
l1_fieldtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to fieldtype must be a vector returned by fields";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fieldtype");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 3)
		vmerr(vm, err);
	vp = &v->vec[Typepos];
	if(vp->qkind != Qxtn)
		vmerr(vm, err);
	*rv = *vp;
}

static void
l1_fieldid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to fieldid must be a vector returned by fields";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fieldid");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 3)
		vmerr(vm, err);
	vp = &v->vec[Idpos];
	if(vp->qkind != Qstr && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = *vp;
}

static void
l1_fieldoff(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to fieldoff must be a vector returned by fields";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fieldoff");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 3)
		vmerr(vm, err);
	vp = &v->vec[Offpos];
	if(vp->qkind != Qcval && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = *vp;
}

static void
l1_enumconsts(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	static char *err
		= "operand 1 to enumconsts must be a defined enum ctype";
	if(argc != 1)
		vmerr(vm, err);
	if(argv[0].qkind != Qxtn)
		vmerr(vm, err);
	xtn = valxtn(&argv[0]);
	if(xtn->tkind != Tenum)
		vmerr(vm, err);
	if(xtn->konst == 0)
		vmerr(vm, err);
	mkvalvec(xtn->konst, rv);
}

static void
l1_symtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to symtype must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symtype");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 3)
		vmerr(vm, err);
	vp = &v->vec[Typepos];
	if(vp->qkind != Qxtn)
		vmerr(vm, err);
	*rv = *vp;
}

static void
l1_symid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to symid must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symid");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 3)
		vmerr(vm, err);
	vp = &v->vec[Idpos];
	if(vp->qkind != Qcval && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = *vp;
}

static void
l1_symval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val *vp;
	static char *err
		= "operand 1 to symval must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symval");
	if(argv->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv);
	if(v->len < 3)
		vmerr(vm, err);
	vp = &v->vec[Offpos];
	if(vp->qkind != Qcval && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = *vp;
}

/* the purpose of typeof on types is to strip the
   enum Tconst and bitfield Tbitfield status from
   lvalue expressions passed to typeof.
   for example: if A is a const of enum X in DOM,
   then we ensure that
   	typeof(dom`A) == typeof(x = dom`A) == enum X
   otherwise, typeof(dom`A) would be Tconst(enum X)

   FIXME:  perhaps this should be generalized
           so that typeof(dom`A) == typeof(x = dom`A).

   e.g.,
	currently, given
		int a[3];
	that
		typeof(a) is int[3]
*/
static void
l1_typeof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Xtypename *t;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to $typeof");
	if(argv->qkind == Qcval){
		cv = valcval(argv);
		t = cv->type;
	}else if(argv->qkind == Qxtn){
		t = valxtn(argv);
		if(t->tkind == Tbitfield || t->tkind == Tconst)
			t = t->link;
	}else
		vmerr(vm, "operand 1 to $typeof must be a cvalue or type");
	mkvalxtn(t, rv);
}

static void
l1_mkctype_void(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_void");
	mkvalxtn(mkvoidxtn(), rv);
}

static void
domkctype_base(Cbase name, Val *rv)
{
	Xtypename *xtn;
	xtn = mkbasextn(name, Rundef);
	mkvalxtn(xtn, rv);
}

static void
l1_mkctype_char(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_char");
	domkctype_base(Vchar, rv);
}

static void
l1_mkctype_short(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_short");
	domkctype_base(Vshort, rv);
}

static void
l1_mkctype_int(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_int");
	domkctype_base(Vint, rv);
}

static void
l1_mkctype_long(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_long");
	domkctype_base(Vlong, rv);
}

static void
l1_mkctype_vlong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_vlong");
	domkctype_base(Vvlong, rv);
}

static void
l1_mkctype_uchar(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uchar");
	domkctype_base(Vuchar, rv);
}

static void
l1_mkctype_ushort(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ushort");
	domkctype_base(Vushort, rv);
}

static void
l1_mkctype_uint(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uint");
	domkctype_base(Vuint, rv);
}

static void
l1_mkctype_ulong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ulong");
	domkctype_base(Vulong, rv);
}

static void
l1_mkctype_uvlong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uvlong");
	domkctype_base(Vuvlong, rv);
}

static void
l1_mkctype_float(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_float");
	domkctype_base(Vfloat, rv);
}

static void
l1_mkctype_double(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_double");
	domkctype_base(Vdouble, rv);
}

static void
l1_mkctype_ldouble(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ldouble");
	domkctype_base(Vlongdouble, rv);
}

static void
l1_mkctype_ptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkctype_ptr");
	if(argv->qkind != Qxtn)
		vmerr(vm, "operand 1 to mkctype_ptr must be a pointer ctype");
	xtn = valxtn(argv);
	xtn = mkptrxtn(xtn, Rundef);
	mkvalxtn(xtn, rv);	
}

static void
l1_mkctype_typedef(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *sub;
	Str *s;

	switch(argc){
	case 1:
		checkarg(vm, "mkctype_typedef", argv, 0, Qstr);
		s = valstr(&argv[0]);
		xtn = mkxtn();
		xtn->tkind = Ttypedef;
		xtn->tid = s;
		break;
	case 2:
		checkarg(vm, "mkctype_typedef", argv, 0, Qstr);
		checkarg(vm, "mkctype_typedef", argv, 1, Qxtn);
		s = valstr(&argv[0]);
		sub = valxtn(&argv[1]);
		xtn = mkxtn();
		xtn->tkind = Ttypedef;
		xtn->tid = s;
		xtn->link = sub;
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_typedef");
	}
	mkvalxtn(xtn, rv);	
}

static void
domkctype_su(VM *vm, char *fn, Tkind tkind, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	Str *s;
	Vec *f;

	switch(argc){
	case 1:
		/* TAG */
		checkarg(vm, fn, argv, 0, Qstr);
		s = valstr(argv);
		xtn = mkxtn();
		xtn->tkind = tkind;
		xtn->tag = s;
		break;
	case 3:
		/* TAG FIELDS SIZE */
		checkarg(vm, fn, argv, 0, Qstr);
		checkarg(vm, fn, argv, 1, Qvec);
		checkarg(vm, fn, argv, 2, Qcval);
		s = valstr(&argv[0]);
		f = valvec(&argv[1]);
		xtn = mkxtn();
		xtn->tkind = tkind;
		xtn->tag = s;
		xtn->field = f;
		xtn->sz = argv[2];
		break;
	default:
		vmerr(vm, "wrong number of arguments to %s", fn);
	}
	mkvalxtn(xtn, rv);
}

static void
l1_mkctype_struct(VM *vm, Imm argc, Val *argv, Val *rv)
{
	domkctype_su(vm, "mkctype_struct", Tstruct, argc, argv, rv);
}

static void
l1_mkctype_union(VM *vm, Imm argc, Val *argv, Val *rv)
{
	domkctype_su(vm, "mkctype_union", Tunion, argc, argv, rv);
}

static void
l1_mkctype_array(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *sub;

	switch(argc){
	case 1:
		/* TYPE */
		checkarg(vm, "mkctype_array", argv, 0, Qxtn);
		sub = valxtn(&argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tarr;
		xtn->link = sub;
		break;
	case 2:
		/* TYPE CNT */
		checkarg(vm, "mkctype_array", argv, 0, Qxtn);
		checkarg(vm, "mkctype_array", argv, 1, Qcval);
		sub = valxtn(&argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tarr;
		xtn->link = sub;
		xtn->cnt = argv[1];
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_array");
	}
	mkvalxtn(xtn, rv);
}

static void
l1_mkctype_fn(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *sub;
	Vec *p;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mkctype_fn");
	checkarg(vm, "mkctype_fn", argv, 0, Qxtn);
	checkarg(vm, "mkctype_fn", argv, 1, Qvec);
	sub = valxtn(&argv[0]);
	p = valvec(&argv[1]);
	xtn = mkxtn();
	xtn->tkind = Tfun;
	xtn->link = sub;
	xtn->param = p;
	mkvalxtn(xtn, rv);
}

static void
l1_mkctype_bitfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *sub;
	/* TYPE WIDTH POS */
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to mkctype_bitfield");
	checkarg(vm, "mkctype_bitfield", argv, 0, Qxtn);
	checkarg(vm, "mkctype_bitfield", argv, 1, Qcval);
	checkarg(vm, "mkctype_bitfield", argv, 2, Qcval);
	sub = valxtn(&argv[0]);
	xtn = mkxtn();
	xtn->tkind = Tbitfield;
	xtn->link = sub;
	xtn->sz = argv[1];
	xtn->bit0 = argv[2];
	mkvalxtn(xtn, rv);
}

static void
l1_mkctype_enum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *t;
	Str *s;
	Vec *c;

	switch(argc){
	case 1:
		/* TAG */
		checkarg(vm, "mkctype_enum", argv, 0, Qstr);
		s = valstr(&argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tenum;
		xtn->tag = s;
		break;
	case 3:
		/* TAG TYPE CONSTS */
		checkarg(vm, "mkctype_enum", argv, 0, Qstr);
		checkarg(vm, "mkctype_enum", argv, 1, Qxtn);
		checkarg(vm, "mkctype_enum", argv, 2, Qvec);
		s = valstr(&argv[0]);
		t = valxtn(&argv[1]);
		c = valvec(&argv[2]);
		xtn = mkxtn();
		xtn->tkind = Tenum;
		xtn->tag = s;
		xtn->link = t;
		xtn->konst = c;
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_enum");
	}
	mkvalxtn(xtn, rv);
}

static void
l1_mkctype_const(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkctype_const");
	checkarg(vm, "mkctype_const", argv, 0, Qxtn);
	t = valxtn(&argv[0]);
	xtn = mkxtn();
	xtn->tkind = Tconst;
	xtn->link = t;
	mkvalxtn(xtn, rv);
}

static void
l1_isnil(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isnil");
	if(argv->qkind == Qnil)
		mkvalcval(vm->litdom, vm->litbase[Vint], 1, rv);
	else
		mkvalcval(vm->litdom, vm->litbase[Vint], 0, rv);
}

static void
l1_error(VM *vm, Imm argc, Val *argv, Val *rv)
{
	/* FIXME: unified formatting */
	l1_printf(vm, argc, argv, rv);
	vmerr(vm, "error (see last message)");
}

static void
l1_strput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s, *t;
	Cval *off;
	Imm o;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to strput");
	if(argv[0].qkind != Qstr)
		vmerr(vm, "operand 1 to strput must be a string");
	if(argv[1].qkind != Qcval)
		vmerr(vm, "operand 2 to strput must be an offset");
	if(argv[2].qkind != Qstr)
		vmerr(vm, "operand 3 to strput must be a string");
	s = valstr(&argv[0]);
	off = valcval(&argv[1]);
	t = valstr(&argv[2]);
	o = off->val;		/* FIXME: use type */
	if(o >= s->len || o+t->len > s->len)
		vmerr(vm, "strput out of bounds");
	memcpy(s->s+o, t->s, t->len);
}

static void
l1_put(VM *vm, Imm argc, Val *iargv, Val *rv)
{
	Dom *d;
	Xtypename *t, *b;
	Cval *addr, *cv, *len;
	Val argv[3], *p;
	Str *bytes, *es;
	BFgeom bfg;
	Imm imm;
	Range *r;

	if(argc != 4)
		vmerr(vm, "wrong number of arguments to put");
	checkarg(vm, "put", iargv, 0, Qdom);
	checkarg(vm, "put", iargv, 1, Qcval);
	checkarg(vm, "put", iargv, 2, Qxtn);
	checkarg(vm, "put", iargv, 3, Qcval);
	d = valdom(&iargv[0]);
	addr = valcval(&iargv[1]);
	t = valxtn(&iargv[2]);
	cv = valcval(&iargv[3]);

	b = chasetype(t);
	switch(b->tkind){
	case Tbase:
	case Tptr:
	case Tenum:
		cv = typecast(vm, t, cv);
		gcprotect(vm, cv);
		bytes = imm2str(t, cv->val);
		len = mkcval(vm->litdom, vm->litbase[Vptr], typesize(vm, t));
		mkvalstr(vm->sput, &argv[0]);
		mkvalrange(addr, len, &argv[1]);
		mkvalstr(bytes, &argv[2]);
		dovm(vm, d->as->dispatch, 3, argv);
		mkvalcval2(cv, rv);
		break;
	case Tbitfield:
		if(b->link->tkind == Tundef){
			es = fmtxtn(b->link->link);
			vmerr(vm, "attempt to write object of undefined type: "
			      "%.*s", es->len, es->s);
		}
		if(0 > dobitfieldgeom(addr, b, &bfg))
			vmerr(vm, "invalid bitfield access");

		/* get contents of bitfield container */
		mkvalstr(vm->sget, &argv[0]);
		/* FIXME: why litbase[Vptr]; why not d->ns->base[Vptr] ? */
		r = mkrange(mkcval(vm->litdom, vm->litbase[Vptr], bfg.addr),
			    mkcval(vm->litdom, vm->litbase[Vptr], bfg.cnt));
		gcprotect(vm, r);
		mkvalrange2(r, &argv[1]);
		p = dovm(vm, d->as->dispatch, 2, argv);
		bytes = valstr(p);

		/* update bitfield container */
		imm = bitfieldput(bytes->s, &bfg, cv->val);

		/* put updated bitfield container */
		mkvalstr(vm->sput, &argv[0]);		
			/* re-use range */
		mkvalstr(bytes, &argv[2]);
		dovm(vm, d->as->dispatch, 3, argv);

		/* return value of bitfield (not container) */
		mkvalcval(d, b->link, imm, rv);
		break;
	case Tconst:
		vmerr(vm, "attempt to use enumeration constant as location");
		break;
	case Tundef:
		es = fmtxtn(b->link);
		vmerr(vm,
		      "attempt to write object of undefined type: %.*s",
		      es->len, es->s);
	default:
		vmerr(vm,
		      "attempt to write %s-valued object to address space",
		      tkindstr[b->tkind]);
	}
}

static void
l1_foreach(VM *vm, Imm argc, Val *iargv, Val *rv)
{
	Vec *k, *v;
	Tab *t;
	Closure *cl;
	Imm m;
	Val argv[2];

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to foreach");
	checkarg(vm, "foreach", iargv, 0, Qcl);
	if(iargv[1].qkind != Qvec && iargv[1].qkind != Qtab)
		vmerr(vm, "operand 1 to foreach must be a vector or table");
	cl = valcl(&iargv[0]);
	switch(iargv[1].qkind){
	case Qvec:
		v = valvec(&iargv[1]);
		for(m = 0; m < v->len; m++){
			argv[0] = *vecref(v, m);
			dovm(vm, cl, 1, argv);
		}
		break;
	case Qtab:
		t = valtab(&iargv[1]);
		k = tabenumkeys(t);
		v = tabenumvals(t);
		gcprotect(vm, k);
		gcprotect(vm, v);
		for(m = 0; m < v->len; m++){
			argv[0] = *vecref(k, m);
			argv[1] = *vecref(v, m);
			dovm(vm, cl, 2, argv);
		}
		break;
	default:
		fatal("bug");
	}
}

static void
l1_mapfile(VM *vm, Imm argc, Val *argv, Val *rv)
{
	int fd;
	Str *names, *map;
	char *p, *name;
	struct stat st;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mapfile");
	checkarg(vm, "mapfile", argv, 0, Qstr);
	names = valstr(&argv[0]);
	name = str2cstr(names);
	fd = open(name, O_RDONLY);
	free(name);
	if(0 > fd)
		vmerr(vm, "cannot open %.*s: %m", names->len, names->s);
	if(0 > fstat(fd, &st)){
		close(fd);
		vmerr(vm, "cannot open %.*s: %m", names->len, names->s);
	}
	p = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	if(p == MAP_FAILED)
		vmerr(vm, "cannot open %.*s: %m", names->len, names->s);
	map = mkstrk(p, st.st_size, Smmap);
	mkvalstr(map, rv);
}

static void
l1_enconsts(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ns *ns;
	Vec *v, *e;
	Dom *d;
	Imm m;
	Cval *a, *cv;
	Xtypename *t;
	Val val;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to enconsts");
	checkarg(vm, "enconsts", argv, 0, Qns);
	checkarg(vm, "enconsts", argv, 1, Qvec);
	ns = valns(&argv[0]);
	v = valvec(&argv[1]);

	/* abstract domain for name space being extended */
	d = mkdom();
	d->ns = ns;
	d->as = mkabas();

	/* determine type that contains all constant values */
	a = mkcval(d, d->ns->base[Vint], 0);
	for(m = 0; m < v->len; m++){
		e = valvec(vecref(v, m)); /* FIXME check sanity */
		a = xcvalalu(vm, Iadd, a,
			     domcast(vm, d, valcval(vecref(e, 1))));
	}
	t = a->type;

	/* cast all values to new type */
	for(m = 0; m < v->len; m++){
		e = valvec(vecref(v, m)); /* FIXME check sanity */
		cv = typecast(vm, t, domcast(vm, d, valcval(vecref(e, 1))));
		mkvalcval2(cv, &val);
		vecset(vm, e, 1, &val);
	}

	/* return type */
	mkvalxtn(t, rv);
}

static void
l1_mkabas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkabas");
	as = mkabas();
	mkvalas(as, rv);
}

static void
l1_stringof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Cval *cv;
	Xtypename *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stringof");
	checkarg(vm, "stringof", argv, 0, Qcval);
	cv = valcval(&argv[0]);
	t = chasetype(cv->type);
	if(t->tkind != Tbase ||
	   (t->basename != Vchar && t->basename != Vuchar))
		vmerr(vm, "operand 1 to stringof must be a char* cvalue");

	s = stringof(vm, cv);
	mkvalstr(s, rv);
}

typedef
struct NSroot {
	Rkind base[Vnbase];
	Cbase ptr;
} NSroot;

static NSroot c32le = {
.base = {
	[Vchar]=	Rs08le,
	[Vshort]=	Rs16le,
	[Vint]=		Rs32le,
	[Vlong]=	Rs32le,
	[Vvlong]=	Rs64le,
	[Vuchar]=	Ru08le,
	[Vushort]=	Ru16le,
	[Vuint]=	Ru32le,
	[Vulong]=	Ru32le,
	[Vuvlong]=	Ru64le,
	[Vfloat]=	Rundef,
	[Vdouble]=	Rundef,
	[Vlongdouble]=	Rundef,
	},
.ptr = Vulong,
};

static NSroot c32be = {
.base = {
	[Vchar]=	Rs08be,
	[Vshort]=	Rs16be,
	[Vint]=		Rs32be,
	[Vlong]=	Rs32be,
	[Vvlong]=	Rs64be,
	[Vuchar]=	Ru08be,
	[Vushort]=	Ru16be,
	[Vuint]=	Ru32be,
	[Vulong]=	Ru32be,
	[Vuvlong]=	Ru64be,
	[Vfloat]=	Rundef,
	[Vdouble]=	Rundef,
	[Vlongdouble]=	Rundef,
	},
.ptr = Vulong,
};

static NSroot c64le = {
.base = {
	[Vchar]=	Rs08le,
	[Vshort]=	Rs16le,
	[Vint]=		Rs32le,
	[Vlong]=	Rs64le,
	[Vvlong]=	Rs64le,
	[Vuchar]=	Ru08le,
	[Vushort]=	Ru16le,
	[Vuint]=	Ru32le,
	[Vulong]=	Ru64le,
	[Vuvlong]=	Ru64le,
	[Vfloat]=	Rundef,
	[Vdouble]=	Rundef,
	[Vlongdouble]=	Rundef,
	},
.ptr = Vuint,
};

static NSroot c64be = {
.base = {
	[Vchar]=	Rs08be,
	[Vshort]=	Rs16be,
	[Vint]=		Rs32be,
	[Vlong]=	Rs64be,
	[Vvlong]=	Rs64be,
	[Vuchar]=	Ru08be,
	[Vushort]=	Ru16be,
	[Vuint]=	Ru32be,
	[Vulong]=	Ru64be,
	[Vuvlong]=	Ru64be,
	[Vfloat]=	Rundef,
	[Vdouble]=	Rundef,
	[Vlongdouble]=	Rundef,
	},
.ptr = Vuint,
};

static NSroot clp64le = {
.base = {
	[Vchar]=	Rs08le,
	[Vshort]=	Rs16le,
	[Vint]=		Rs32le,
	[Vlong]=	Rs64le,
	[Vvlong]=	Rs64le,
	[Vuchar]=	Ru08le,
	[Vushort]=	Ru16le,
	[Vuint]=	Ru32le,
	[Vulong]=	Ru64le,
	[Vuvlong]=	Ru64le,
	[Vfloat]=	Rundef,
	[Vdouble]=	Rundef,
	[Vlongdouble]=	Rundef,
	},
.ptr = Vulong,
};

static NSroot clp64be = {
.base = {
	[Vchar]=	Rs08be,
	[Vshort]=	Rs16be,
	[Vint]=		Rs32be,
	[Vlong]=	Rs64be,
	[Vvlong]=	Rs64be,
	[Vuchar]=	Ru08be,
	[Vushort]=	Ru16be,
	[Vuint]=	Ru32be,
	[Vulong]=	Ru64be,
	[Vuvlong]=	Ru64be,
	[Vfloat]=	Rundef,
	[Vdouble]=	Rundef,
	[Vlongdouble]=	Rundef,
	},
.ptr = Vulong,
};

static Xtypename*
mkvoidxtn()
{
	Xtypename *xtn;
	xtn = mkxtn();
	xtn->tkind = Tvoid;
	return xtn;
}

static Xtypename*
mkundefxtn(Xtypename *t)
{
	Xtypename *xtn;
	xtn = mkxtn();
	xtn->tkind = Tundef;
	xtn->link = t;
	return xtn;
}

static Xtypename*
mkbasextn(Cbase name, Rkind rep)
{
	Xtypename *xtn;
	xtn = mkxtn();
	xtn->tkind = Tbase;
	xtn->basename = name;
	xtn->rep = rep;
	return xtn;
}

static Xtypename*
mkptrxtn(Xtypename *t, Rkind rep)
{
	Xtypename *xtn;
	xtn = mkxtn();
	xtn->tkind = Tptr;
	xtn->link = t;
	xtn->rep = rep;
	return xtn;
}

/* FIXME: this shouldn't need a VM */
static Tab*
basetab(VM *vm, Xtypename **base)
{
	Cbase cb;
	Val kv, vv;
	Tab *type;
	Xtypename *pt;

	type = mktab();
	for(cb = Vchar; cb < Vnbase; cb++){
		mkvalxtn(base[cb], &kv);
		mkvalxtn(base[cb], &vv);
		tabput(vm, type, &kv, &vv);
	}

	/* map pointer to integer representation */
	pt = mkbasextn(Vptr, Rundef);
	mkvalxtn(pt, &kv);
	mkvalxtn(base[Vptr], &vv);
	tabput(vm, type, &kv, &vv);

	return type;
}

static Ns*
mkrootns(VM *vm, NSroot *def)
{
	Tab *type;
	Cbase cb;
	Ns *ns;
	Xtypename *base[Vnallbase];

	for(cb = Vchar; cb < Vnbase; cb++)
		base[cb] = mkbasextn(cb, def->base[cb]);
	base[Vptr] = base[def->ptr];
	type = basetab(vm, base);
	ns = mknstab(type, mktab());
	nscachebase(vm, ns);
	return ns;
}

static char*
myroot()
{
	u32 x;
	char *p;
	int longsz;
	int ptrsz;
	int flags;
	enum {
		lw = 1<<0,
		lp = 1<<1,
		be = 1<<2,
	};
	static char *root[] = {
		[0] 		= "c32le",
		[lw]		= "c64le",
		[lp|lw]		= "clp64le",
		[be] 		= "c32be",
		[be|lw]		= "c64be",
		[be|lp|lw]	= "clp64be",
	};
	char *r;

	flags = 0;
	longsz = 8*sizeof(long);
	if(longsz == 64)
		flags |= lw;
	ptrsz = 8*sizeof(void*);
	if(ptrsz == 64)
		flags |= lp;
	x = 0x01020304;
	p = (char*)&x;
	if(*p == 0x01)
		flags |= be;

	r = root[flags];
	if(r == 0)
		return "unsupported";
	return r;
}

static Dom*
mksysdom(VM *vm)
{
	Ns *ns, *root;
	Dom *dom;
	Tab *rawtype, *rawsym;
	Vec *v;
	Str *id;
	Xtypename *t;
	Cval *o;
	Val keyv, valv;
	char *rootname;

	rootname = myroot();
//	printf("host name space: %s\n", rootname);
	if(!envlookup(vm->top, rootname, &valv))
		vmerr(vm, "undefined name space: %s", rootname);
	root = valns(&valv);

	rawtype = mktab();
	rawsym = mktab();

	t = mkxtn();
	t->tkind = Tarr;
	mkvalcval(vm->litdom, vm->litbase[Vint], 256, &t->cnt);
	t->link = mkbasextn(Vchar, Rundef);
	id = mkstr0("flags");
	o = mkcval(vm->litdom, vm->litbase[Vptr], (Imm)&flags);
	
	v = mkvec(3);
	mkvalxtn(t, &valv);
	_vecset(v, Typepos, &valv);
	mkvalstr(id, &valv);
	_vecset(v, Idpos, &valv);
	mkvalcval2(o, &valv);
	_vecset(v, Offpos, &valv);

	mkvalstr(id, &keyv);
	mkvalvec(v, &valv);
	tabput(vm, rawsym, &keyv, &valv);

	ns = dorawns(vm, root, rawtype, rawsym);
	dom = mkdom();
	dom->ns = ns;
	dom->as = mkmas(mkstrk(0, ~(0ULL), Sperm));

	return dom;
}

static Dom*
mklitdom(VM *vm)
{
	Ns *ns;
	Dom *dom;
	Tab *type;
	Cbase cb;
	Xtypename *base[Vnallbase];
	static NSroot *litdef = &clp64le;

	dom = mkdom();
	dom->as = mkabas();

	memset(base, 0, sizeof(base)); /* values will be seen by GC */
	for(cb = Vchar; cb <= Vnbase; cb++)
		base[cb] = mkbasextn(cb, litdef->base[cb]);
	base[Vptr] = base[litdef->ptr];
	type = basetab(vm, base);
	ns = mknstab(type, mktab());
	/* hand populate ns->base, because nscachebase calls the
	   VM, which requires a literal domain. */
	memcpy(ns->base, base, sizeof(base));

	dom->ns = ns;
	return dom;
}

static VM *vms[Maxvms];

#define FN(name) builtinfn(env, #name, mkcfn(#name, l1_##name))

VM*
mkvm(Env *env)
{
	VM *vm, **vmp;

	vm = xmalloc(sizeof(VM));
	vm->top = env;
	vm->pmax = GCinitprot;
	vm->prot = xmalloc(vm->pmax*sizeof(Root*));
	vm->emax = Errinitdepth;
	vm->err = xmalloc(vm->emax*sizeof(Err));
	
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
	builtinfn(env, "isvector", isvectorthunk());
	builtinfn(env, "string", stringthunk());
	builtinfn(env, "strlen", strlenthunk());
	builtinfn(env, "substr", substrthunk());
	builtinfn(env, "table", tablethunk());
	builtinfn(env, "tabinsert", tabinsertthunk());
	builtinfn(env, "tabdelete", tabdeletethunk());
	builtinfn(env, "tabenum", tabenumthunk());
	builtinfn(env, "tablook", tablookthunk());
	builtinfn(env, "list", listthunk());
	builtinfn(env, "mkvec", mkvecthunk());
	builtinfn(env, "vector", vectorthunk());
	builtinfn(env, "veclen", veclenthunk());
	builtinfn(env, "vecref", vecrefthunk());
	builtinfn(env, "vecset", vecsetthunk());

	FN(asdispatch);
	FN(nslooksym);
	FN(nslooktype);
	FN(nsenumsym);
	FN(nsenumtype);
	FN(looktype);
	FN(gettimeofday);
	FN(randseed);
	FN(rand);
	FN(printf);
	FN(tabkeys);
	FN(tabvals);
	FN(vmbacktrace);
	FN(mkabas);
	FN(stringof);

	FN(isvoid);
	FN(isundeftype);
	FN(isbase);
	FN(issu);
	FN(isstruct);
	FN(isunion);
	FN(isenum);
	FN(isenumconst);
	FN(isbitfield);
	FN(isptr);
	FN(isarray);
	FN(isfunc);
	FN(istypedef);

	FN(baseid);
	FN(subtype);
	FN(suekind);
	FN(suetag);
	FN(susize);
	FN(arraynelm);
	FN(bitfieldpos);
	FN(bitfieldwidth);
	FN(bitfieldcontainer);
	FN(rettype);
	FN(params);
	FN(paramtype);
	FN(paramid);
	FN(fields);
	FN(lookfield);
	FN(fieldtype);
	FN(fieldid);
	FN(fieldoff);
	FN(enumconsts);
	FN(typedefid);
	FN(typedeftype);
	FN(symid);
	FN(symtype);
	FN(symval);
	FN(domof);
	FN(nsptr);
	FN(strput);
	FN(isnil);
	FN(error);

	FN(mkctype_void);
	FN(mkctype_char);
	FN(mkctype_short);
	FN(mkctype_int);
	FN(mkctype_long);
	FN(mkctype_vlong);
	FN(mkctype_uchar);
	FN(mkctype_ushort);
	FN(mkctype_uint);
	FN(mkctype_ulong);
	FN(mkctype_uvlong);
	FN(mkctype_float);
	FN(mkctype_double);
	FN(mkctype_ldouble);
	FN(mkctype_ptr);
	FN(mkctype_typedef);
	FN(mkctype_struct);
	FN(mkctype_union);
	FN(mkctype_array);
	FN(mkctype_fn);
	FN(mkctype_bitfield);
	FN(mkctype_enum);
	FN(mkctype_const);
	FN(foreach);
	FN(enconsts);
	FN(mapfile);

	builtinfn(env, "$put", mkcfn("$put", l1_put));
	builtinfn(env, "$typeof", mkcfn("$typeof", l1_typeof));

//	builtinfn(env, "isundeftype", mkcfn("isundeftype", l1_isundeftype));

//	builtinstr(env, "$get", "get");
//	builtinstr(env, "$put", "put");
	builtinstr(env, "$looksym", "looksym");
	builtinstr(env, "$looktype", "looktype");

	/* FIXME: make litdom binding immutable (@litdom)? */
	vm->litdom = mklitdom(vm);
	vm->litns = vm->litdom->ns;
	vm->litbase = vm->litns->base;
	builtindom(env, "litdom", vm->litdom);
	vm->sget = mkstr0("get");
	vm->sput = mkstr0("put");
	vm->smap = mkstr0("map");

	vmp = vms;
	while(*vmp){
		vmp++;
		if(vmp > vms+Maxvms)
			fatal("too many vms");
	}
	*vmp = vm;

	vmreset(vm);
	concurrentgc(vm);
	/* vm is now callable */

	if(waserror(vm)){
		/* FIXME: kill GC */
		free(vm);
		return 0;
	}
	builtinns(env, "c32le", mkrootns(vm, &c32le));
	builtinns(env, "c32be", mkrootns(vm, &c32be));
	builtinns(env, "c64le", mkrootns(vm, &c64le));
	builtinns(env, "c64be", mkrootns(vm, &c64be));
	builtinns(env, "clp64le", mkrootns(vm, &clp64le));
	builtinns(env, "clp64be", mkrootns(vm, &clp64be));
	builtindom(env, "sys", mksysdom(vm));
	poperror(vm);
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
	free(vm->prot);
	free(vm->err);
	free(vm);
}

static void
vmfaulthook()
{
	VM **vmp;
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp){
			fprintf(stderr, "backtrace of vm %p:\n", *vmp);
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
	GCiterdone = xmalloc(1); /* unique pointer */
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
	free(GCiterdone);

	for(i = 0; i < Qnkind; i++){
		hp = &heap[i];
		if(hp->id)
			freeheap(hp);
	}
	setfaulthook(0);
}
