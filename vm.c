#include "sys.h"
#include "util.h"
#include "syscqct.h"

#define HEAPPROF 0
#define HEAPDEBUG 0

static void printval(VM *vm, Val val);

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
	[Qfd]=		"file descriptor",
	[Qlist]=	"list",
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

static Imm repsize[Rnrep] = {
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
	Listinitsize=16,
	Maxprintint=32,
	Typepos=0,
	Idpos=1,
	Offpos=2,
	Maxvms=1024,
	Errinitdepth=128,	/* initial max error stack */
};

struct Heap {
	char *id;
	Qkind qkind;
	unsigned sz;
	unsigned clearit;
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
	Root *free;
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
typedef struct Fd Fd;
typedef struct List List;
typedef struct Ns Ns;
typedef struct Pair Pair;
typedef struct Range Range;
typedef struct Str Str;
typedef struct Tab Tab;
typedef struct Vec Vec;
typedef struct Xtypename Xtypename;

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

typedef
struct Listx {
	/* invariants:
	 *  if(hd == tl)
	 *  	list is empty.
	 *  else
	 *  	tl>hd and list has tl-hd elements,
	 *      hd points to first element in list, and
	 *	tl-1 points to last.
	 */
	u32 hd, tl, sz;
	Val *val;		/* atomically swappable */
	Val *oval;		/* buffer for gc-safe sliding */
} Listx;

struct List {
	Head hd;
	Listx *x;		/* current storage, atomically swappable */
};

struct As {
	Head hd;
	Closure *dispatch;
	Str *name;
};

struct Ns {
	Head hd;

	Closure *enumsym;
	Closure *enumtype;
	Closure *looksym;
	Closure *looktype;
	Closure *lookaddr;
	Str *name;

	/* cached base type definition */
	Xtypename *base[Vnallbase];
};

struct Dom {
	Head hd;
	As *as;
	Ns *ns;
	Str *name;
};

struct Fd {
	Head hd;
	void (*free)(Fd *fd);
	int fd;
	Str *name;
	enum Fflag {
		Fclosed =	1,
		Fread =		Fclosed<<1,
		Fwrite =	Fread<<1,
	} flags;
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
	u64 len;
	size_t mlen;		/* Smmap size */
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
	Vec *param;		/* func */
	Vec *konst;		/* enum (constants) */
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
	Fd *stdout, *stdin;
	Root **prot;		/* stack of lists of GC-protected objects */
	Rootset rs;		/* Root free list for prot */
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

static void vmsetcl(VM *vm, Val val);
static void gcprotpush(VM *vm);
static void gcprotpop(VM *vm);
static void* gcprotect(VM *vm, void *hd);
static void vmerr(VM *vm, char *fmt, ...) NORETURN;
static Xtypename* chasetype(Xtypename *xtn);
static Xtypename* dolooktype(VM *vm, Xtypename *xtn, Ns *ns);
static Xtypename* mkvoidxtn();
static Xtypename* mkbasextn(Cbase name, Rkind rep);
static Xtypename* mkptrxtn(Xtypename *t, Rkind rep);
static Xtypename* mkconstxtn(Xtypename *t);
static Xtypename* mktypedefxtn(Str *tid, Xtypename *t);
static Xtypename* mkundefxtn(Xtypename *t);
static Str* fmtxtn(Xtypename *xtn);
static void checkarg(VM *vm, char *fn, Val *argv, unsigned arg, Qkind qkind);
static int isstrcval(Cval *cv);
static Str* stringof(VM *vm, Cval *cv);
static Val vecref(Vec *vec, Imm idx);
static void _vecset(Vec *vec, Imm idx, Val v);
static void vecset(VM *vm, Vec *vec, Imm idx, Val v);
static u32 listxlen(Listx *x);
static void l1_sort(VM *vm, Imm argc, Val *argv, Val *rv);

static Val Xundef;
static Val Xnil;
static Val Xnulllist;
static Dom *litdom;
static Ns *litns;
static Xtypename **litbase;
static Cval *cvalnull, *cval0, *cval1, *cvalminus1;

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
static void freefd(Head*);
static void freelist(Head*);
static void freestr(Head*);
static void freetab(Head*);
static void freevec(Head*);

static Head *iteras(Head*, Ictx*);
static Head *iterbox(Head*, Ictx*);
static Head *itercl(Head*, Ictx*);
static Head *itercode(Head*, Ictx*);
static Head *itercval(Head*, Ictx*);
static Head *iterdom(Head*, Ictx*);
static Head *iterfd(Head*, Ictx*);
static Head *iterns(Head*, Ictx*);
static Head *iterlist(Head*, Ictx*);
static Head *iterpair(Head*, Ictx*);
static Head *iterrange(Head*, Ictx*);
static Head *itertab(Head*, Ictx*);
static Head *itervec(Head*, Ictx*);
static Head *iterxtn(Head*, Ictx*);

static Heap heap[Qnkind] = {
	[Qas]	= { "as", Qas, sizeof(As), 0, 0, iteras },
	[Qbox]	= { "box", Qbox, sizeof(Box),	0, 0, iterbox },
	[Qcval] = { "cval", Qcval, sizeof(Cval), 0, 0, itercval },
	[Qcl]	= { "closure", Qcl, sizeof(Closure), 1, freecl, itercl },
	[Qcode]	= { "code", Qcode, sizeof(Code), 1, freecode, itercode },
	[Qdom]	= { "domain", Qdom, sizeof(Dom), 0, 0, iterdom },
	[Qfd]	= { "fd", Qfd,sizeof(Fd), 0, freefd, iterfd },
	[Qlist]	= { "list", Qlist, sizeof(List), 0, freelist, iterlist },
	[Qns]	= { "ns", Qns, sizeof(Ns), 1, 0, iterns },
	[Qpair]	= { "pair", Qpair, sizeof(Pair), 0, 0, iterpair },
	[Qrange] = { "range", Qrange, sizeof(Range), 0, 0, iterrange },
	[Qstr]	= { "string", Qstr, sizeof(Str), 1, freestr, 0 },
	[Qtab]	= { "table", Qtab, sizeof(Tab), 1, freetab, itertab },
	[Qvec]	= { "vector", Qvec, sizeof(Vec), 0, freevec, itervec },
	[Qxtn]	= { "typename", Qxtn, sizeof(Xtypename), 1, 0, iterxtn },
};

static u32 nohash(Val);
static u32 hashcval(Val);
static u32 hashptr(Val);
static u32 hashconst(Val);
static u32 hashrange(Val);
static u32 hashstr(Val);
static u32 hashxtn(Val);

static int eqcval(Val, Val);
static int eqptr(Val, Val);
static int eqtrue(Val, Val);
static int eqrange(Val, Val);
static int eqstrv(Val, Val);
static int eqxtnv(Val, Val);

typedef struct Hashop {
	u32 (*hash)(Val);
	int (*eq)(Val, Val);
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
	[Qfd]	= { hashptr, eqptr },
	[Qlist]	= { hashptr, eqptr },
	[Qns]	= { hashptr, eqptr },
	[Qpair]	= { hashptr, eqptr },
	[Qrange] =  { hashrange, eqrange },
	[Qstr]	= { hashstr, eqstrv },
	[Qtab]	= { hashptr, eqptr },
	[Qvec]	= { hashptr, eqptr },
	[Qxtn]	= { hashxtn, eqxtnv },
};

static Code *kcode, *cccode;

static Imm
stkimm(Val v)
{
	Imm imm;
	imm = (Imm)(uintptr_t)v;
	if((imm&1) != 1)
		fatal("stkimm on non-imm");
	imm >>= 1;
	return imm;
}

Imm
valimm(Val v)
{
	Cval *cv;
	if(v->qkind != Qcval)
		fatal("valimm on non-cval");
	cv = (Cval*)v;
	return cv->val;
}

static Val
valboxed(Val v)
{
	Box *b;
	if(v->qkind != Qbox)
		fatal("valboxed on non-box");
	b = (Box*)v;
	return b->v;
}

#define valas(v)	((As*)(v))
#define valcval(v)	((Cval*)(v))
#define valcl(v)	((Closure*)(v))
#define valdom(v)	((Dom*)(v))
#define valfd(v)	((Fd*)(v))
#define vallist(v)	((List*)(v))
#define valns(v)	((Ns*)(v))
#define valpair(v)	((Pair*)(v))
#define valrange(v)	((Range*)(v))
#define valstr(v)	((Str*)(v))
#define valtab(v)	((Tab*)(v))
#define valvec(v)	((Vec*)(v))
#define valxtn(v)	((Xtypename*)(v))
#define valboxedcval(b)	((Cval*)((Box*)(b))->v)

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
		if(HEAPPROF) heap->nfree--;
	}else if(heap->swept){
		heap->free = (Head*)read_and_clear(&heap->swept);
		if(HEAPPROF) heap->nfree += hlen(heap->free);
		goto retry;
	}else{
		ap = heap->alloc;
		fp = 0;
		for(m = 0; m < AllocBatch; m++){
			o = xmalloc(heap->sz);
//			VALGRIND_MAKE_MEM_NOACCESS(o+1, heap->sz-sizeof(Head));
			o->heap = heap;
			o->qkind = heap->qkind;
			o->alink = ap;
			o->link = fp;
			o->color = GCfree;
			o->state = -1;
			ap = o;
			fp = o;
		}
		heap->alloc = o;
		heap->free = o;
		if(HEAPPROF) heap->nalloc += AllocBatch;
		if(HEAPPROF) heap->nfree += AllocBatch;
		goto retry;
	}
	
	o->color = GCCOLOR(gcepoch);
	if(HEAPPROF) heap->nha++;
	if(HEAPDEBUG)
		printf("alloc %p (%s)\n", o, heap->id);

	// FIXME: only object types that do not initialize all fields
	// (e.g., Xtypename) need to be cleared.  Perhaps add a bit
	// to heap to select clearing.
//	VALGRIND_MAKE_MEM_UNDEFINED(o+1, heap->sz-sizeof(Head));
	if(heap->clearit)
		memset(o+1, 0, heap->sz-sizeof(Head));
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
//			VALGRIND_MAKE_MEM_NOACCESS(p+1, heap->sz-sizeof(Head));
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
valhead(Val v)
{
	Imm imm;

	if(v == 0)
		return 0;

	imm = (Imm)(uintptr_t)v;
	if((imm&1) != 0)
		/* stack immediate */
		return 0;

	switch(v->qkind){
	case Qundef:
	case Qnil:
	case Qnulllist:
	case Qimm:
		return 0;
		break;
	default:
		return (Head*)v;
		break;
	}
}

static Root*
newroot(Rootset *rs)
{
	Root *r;
	if(rs->free){
		r = rs->free;
		rs->free = r->link;
		return r;
	}
	return xmalloc(sizeof(Root));
}

static void
freeroot(Rootset *rs, Root *r)
{
//	free(r);
	r->link = rs->free;
	rs->free = r;
}

static void
freerootlist(Rootset *rs, Root *r)
{
	Root *nxt;
	while(r){
		nxt = r->link;
		freeroot(rs, r);
		r = nxt;
	}
}

static void
freefreeroots(Rootset *rs)
{
	Root *r, *nxt;
	r = rs->free;
	while(r){
		nxt = r->link;
		free(r);
		r = nxt;
	}
}

static void
doaddroot(Rootset *rs, Head *h)
{
	Root *r;
	int x;

	/* test if already on a rootlist */
	x = h->state;
	if(x > 2 || x < 0)
		fatal("addroot %p (%s) bad state %d", h, h->heap->id, x);
	atomic_inc(&h->state);

	r = newroot(rs);
	h->inrootset = 1;
	r->hd = h;
	r->link = rs->roots;
	writebarrier();
	rs->roots = r;
}

/* called on roots by marker.  called on stores by mutator. */
static void
addroot(Rootset *rs, Head *h)
{
	if(h == 0)
		return;

	if(HEAPDEBUG)
		printf("addroot %s %p %d %d\n",
		       rs == &roots ? "roots" : "stores",
		       h, h->inrootset, h->state);

	if(h->inrootset)
		return;
	doaddroot(rs, h);
}

static void
addrootstk(Rootset *rs, Head *h)
{
	if(h == 0)
		return;

	if(HEAPDEBUG)
		printf("addroot %s %p %d %d\n",
		       rs == &roots ? "roots" : "stores",
		       h, h->inrootset, h->state);

	if(h->color == GCfree)
		/* stale value on stack already collected */
		return;
	if(h->inrootset)
		return;

	doaddroot(rs, h);
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
	addroot(&roots, valhead(*(Val*)v));
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
		addrootstk(&roots, valhead(vm->stack[m]));
	hforeach(vm->top->ht, bindingroot, 0);
	addroot(&roots, valhead(vm->ac));
	addroot(&roots, valhead(vm->cl));
	
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
	freerootlist(rs, rs->roots);
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
		cv = valcval(xtn->sz);
		return cv->val;
	case Tenum:
		return typesize(vm, xtn->link);
	case Tptr:
		return repsize[xtn->rep];
	case Tarr:
		if(xtn->cnt->qkind != Qcval)
			vmerr(vm,
			      "attempt to compute size of unspecified array");
		cv = valcval(xtn->cnt);
		return cv->val*typesize(vm, xtn->link);
	case Tfun:
		vmerr(vm, "attempt to compute size of function type");
	case Tbitfield:
		vmerr(vm, "attempt to compute size of bitfield"); 
	case Tundef:
		es = fmtxtn(xtn->link);
		vmerr(vm, "attempt to compute size of undefined type: %.*s",
		      (int)es->len, es->s);
	case Tconst:
		vmerr(vm, "shouldn't this be impossible?");
	}
	fatal("bug");
}

static Head*
iterbox(Head *hd, Ictx *ictx)
{
	Box *box;
	box = (Box*)hd;
	if(ictx->n > 0)
		return GCiterdone;
	ictx->n = 1;
	return valhead(box->v);
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
	return valhead(cl->display[ictx->n++]);
}

static Head*
itercode(Head *hd, Ictx *ictx)
{
	Code *code;
	code = (Code*)hd;
	if(ictx->n >= hnent(code->konsti->ht))
		return GCiterdone;
	return valhead(hrefval(code->konsti->ht, ictx->n++));
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
		return valhead(pair->car);
	case 1:
		return valhead(pair->cdr);
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
	Val vp;
	unsigned m;

	va_start(args, dlen);
	cl = mkcl(cccode, 0, dlen, id);
	cl->ccl = ccl;
	for(m = 0; m < dlen; m++){
		vp = va_arg(args, Val);
		cl->display[m] = vp;
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

static void
freefdclose(Fd *fd)
{
	close(fd->fd);
}

static void
freefd(Head *hd)
{
	Fd *fd;
	fd = (Fd*)hd;
	if(fd->free)
		fd->free(fd);
}

static Fd*
mkfd(Str *name, int xfd, int flags, void (*free)(Fd *fd))
{
	Fd *fd;
	fd = (Fd*)halloc(&heap[Qfd]);
	fd->name = name;
	fd->fd = xfd;
	fd->free = free;
	fd->flags = flags;
	return fd;
}

static Head*
iterfd(Head *hd, Ictx *ictx)
{
	Fd *fd;
	fd = (Fd*)hd;
	switch(ictx->n++){
	case 0:
		return (Head*)fd->name;
	default:
		return GCiterdone;
	}
}

static int
eqval(Val v1, Val v2)
{
	if(v1->qkind != v2->qkind)
		return 0;
	return hashop[v1->qkind].eq(v1, v2);
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
nohash(Val val)
{
	fatal("bad type of key (%d) to table operation", val->qkind);
}

static u32
hashptr(Val val)
{
	return hashptr32shift(valhead(val));
}

static int
eqptr(Val a, Val b)
{
	return valhead(a)==valhead(b);
}

static u32
hashconst(Val val)
{
	switch(val->qkind){
	case Qnil:
		return hashptr32shift(Xnil);
	case Qnulllist:
		return hashptr32shift(Xnulllist);
	default:
		fatal("bug");
	}
}

static int
eqtrue(Val a, Val b)
{
	return 1;
}

static u32
hashcval(Val val)
{
	Cval *cv;
	cv = valcval(val);
	return hash6432shift(cv->val);
}

static int
eqcval(Val a, Val b)
{
	Cval *cva, *cvb;
	cva = valcval(a);
	cvb = valcval(b);
	return cva->val==cvb->val;
}

static u32
hashrange(Val val)
{
	Range *r;
	r = valrange(val);
	return hash6432shift(r->beg->val)^hash6432shift(r->len->val);
}

static int
eqrange(Val a, Val b)
{
	Range *ra, *rb;
	ra = valrange(a);
	rb = valrange(b);
	return ra->beg->val==rb->beg->val && ra->len->val==rb->len->val;
}

static u32
hashstr(Val val)
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
eqstrv(Val a, Val b)
{
	return eqstr(valstr(a), valstr(b));
}

static u32
hashxtn(Val val)
{
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
		return hashstr((Val)xtn->tid)>>xtn->tkind;
	case Tstruct:
	case Tunion:
	case Tenum:
		return hashstr((Val)xtn->tag)>>xtn->tkind;
	case Tundef:
	case Tptr:
		return hashxtn((Val)xtn->link)>>xtn->tkind;
	case Tarr:
		x = hashxtn((Val)xtn->link)>>xtn->tkind;
		if(xtn->cnt->qkind == Qcval)
			x ^= hashcval(xtn->cnt);
		return x;
	case Tfun:
		x = hashxtn((Val)xtn->link)>>xtn->tkind;
		for(m = 0; m < xtn->param->len; m++){
			x <<= 1;
			vec = valvec(vecref(xtn->param, m));
			x ^= hashxtn(vecref(vec, Typepos));
		}
		return x;
	case Tbitfield:
		x = hashxtn((Val)xtn->link)>>xtn->tkind;
		x ^= hashcval(xtn->sz);
		return x;
	case Tconst:
		return hashxtn((Val)xtn->link)>>xtn->tkind;
	}
	fatal("bug");
}

static int
eqxtn(Xtypename *a, Xtypename *b)
{
	Imm m;

	if(a->tkind != b->tkind)
		return 0;

	switch(a->tkind){
	case Tvoid:
		return 1;
	case Tbase:
		return a->basename == b->basename;
	case Ttypedef:
		return eqstr(a->tid, b->tid);
	case Tstruct:
	case Tunion:
	case Tenum:
		return eqstr(a->tag, b->tag);
	case Tundef:
	case Tptr:
		return eqxtn(a, b);
	case Tarr:
		if(a->cnt->qkind != b->cnt->qkind)
			return 0;
		if(a->cnt->qkind == Qcval){
			if(!eqcval(a->cnt, b->cnt))
				return 0;
		}
		return eqxtn(a, b);
	case Tfun:
		if(a->param->len != b->param->len)
			return 0;
		if(!eqxtn(a, b))
			return 0;
		for(m = 0; m < a->param->len; m++)
			if(!eqxtn(valxtn(vecref(valvec(vecref(a->param, m)),
						Typepos)),
				  valxtn(vecref(valvec(vecref(b->param, m)),
						Typepos))))
				return 0;
		return 1;
	case Tbitfield:
		if(!eqcval(a->sz, b->sz))
			return 0;
		return eqxtn(a, b);
	case Tconst:
		return eqxtn(a, b);
	}
	fatal("bug");
}

static int
eqxtnv(Val a, Val b)
{
	return eqxtn(valxtn(a), valxtn(b));
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
mkstr(char *s, Imm len)
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
mkstrk(char *s, Imm len, Skind skind)
{
	Str *str;
	str = (Str*)halloc(&heap[Qstr]);
	str->s = s;
	str->len = len;
	str->skind = skind;
	if(skind == Smmap)
		str->mlen = len;
	return str;
}

static Str*
mkstrn(VM *vm, Imm len)
{
	Str *str;
	str = (Str*)halloc(&heap[Qstr]);
	if(len >= PAGESZ){
		str->len = len;
		str->s = mmap(0, PAGEUP(len), PROT_READ|PROT_WRITE,
			      MAP_NORESERVE|MAP_PRIVATE|MAP_ANON, 0, 0);
		if(str->s == (void*)(-1))
			vmerr(vm, "out of memory");
		str->mlen = PAGEUP(len);
		str->skind = Smmap;
	}else{
		str->len = len;
		str->s = xmalloc(str->len);
		str->skind = Smalloc;
	}
	return str;
}

static Str*
mkstrlits(Lits *lits)
{
	return mkstr(lits->s, lits->len);
}

static char*
str2cstr(Str *str)
{
	char *s;
	s = xmalloc(str->len+1);
	memcpy(s, str->s, str->len);
	return s;
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
		munmap(str->s, str->mlen);
		break;
	case Smalloc:
		free(str->s);
		break;
	case Sperm:
		break;
	}
}

static int
listlenpair(Val v, Imm *rv)
{
	Imm m;
	Pair *p;

	m = 0;
	while(v->qkind == Qpair){
		m++;
		p = valpair(v);
		v = p->cdr;
	}
	if(v->qkind != Qnulllist)
		return 0;
	*rv = m;
	return 1;
}

static int
listlen(Val v, Imm *rv)
{
	List *lst;
	if(v->qkind == Qpair || v->qkind == Qnulllist)
		return listlenpair(v, rv);
	if(v->qkind == Qlist){
		lst = vallist(v);
		*rv = listxlen(lst->x);
		return 1;
	}
	return 0;
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

static Val
vecref(Vec *vec, Imm idx)
{
	return vec->vec[idx];
}

static void
_vecset(Vec *vec, Imm idx, Val v)
{
	vec->vec[idx] = v;
}

static void
vecset(VM *vm, Vec *vec, Imm idx, Val v)
{
	if(vm->gcrun)
		addroot(&stores, valhead(vec->vec[idx]));
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
	return valhead(vec->vec[ictx->n++]);
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
	x->val = xmalloc(x->lim*sizeof(Val)); /* must be 0 or Xundef */
	x->key = xmalloc(x->lim*sizeof(Val)); /* must be 0 or Xundef */
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
		return valhead(x->val[idx]);
	}
	idx = ictx->n++;
	return valhead(x->key[idx]);
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
_tabget(Tab *tab, Val keyv, Tabidx ***prev)
{
	Tabidx **p, *tk;
	Val kv;
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
		kv = x->key[tk->idx];
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

static Val
tabget(Tab *tab, Val keyv)
{
	Tabidx *tk;
	tk = _tabget(tab, keyv, 0);
	if(tk)
		return tab->x->val[tk->idx];
	return 0;
}

/* double hash table size and re-hash entries */
static void
tabexpand(Tab *tab)
{
	Tabidx *tk, *nxt;
	u32 i, m, idx;
	Tabx *x, *nx;
	Val kv;

	x = tab->x;
	nx = mktabx(x->sz*2);
	m = 0;
	for(i = 0; i < x->sz && m < tab->cnt; i++){
		tk = x->idx[i];
		while(tk){
			nxt = tk->link;
			kv = x->key[tk->idx];
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
dotabput(VM *vm, Tab *tab, Val keyv, Val val)
{
	Tabidx *tk;
	u32 idx;
	Tabx *x;

	x = tab->x;
	tk = _tabget(tab, keyv, 0);
	if(tk){
		if(vm && vm->gcrun)
			addroot(&stores, valhead(x->val[tk->idx]));
		x->val[tk->idx] = val;
		return;
	}

	if(x->nxt >= x->lim){
		tabexpand(tab);
		x = tab->x;
	}

	tk = xmalloc(sizeof(Tabidx));

	/* FIXME: snapshot-at-beginning seems to imply that it does
	   not matter whether gc can see these new values in this
	   epoch. right? */
	tk->idx = x->nxt;
	x->key[tk->idx] = keyv;
	x->val[tk->idx] = val;
	idx = hashop[keyv->qkind].hash(keyv)&(x->sz-1);
	tk->link = x->idx[idx];
	x->idx[idx] = tk;
	x->nxt++;
	tab->cnt++;
}

static void
_tabput(Tab *tab, Val keyv, Val val)
{
	dotabput(0, tab, keyv, val);
}

static void
tabput(VM *vm, Tab *tab, Val keyv, Val val)
{
	dotabput(vm, tab, keyv, val);
}

static void
tabdel(VM *vm, Tab *tab, Val keyv)
{
	Tabidx *tk, **ptk;
	Tabx *x;
	
	x = tab->x;
	tk = _tabget(tab, keyv, &ptk);
	if(tk == 0)
		return;
	if(vm->gcrun){
		addroot(&stores, valhead(x->key[tk->idx]));
		addroot(&stores, valhead(x->val[tk->idx]));
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
			_vecset(vec, m, x->key[tk->idx]);
			_vecset(vec, m+tab->cnt, x->val[tk->idx]);
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
			_vecset(vec, m, x->key[tk->idx]);
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
			_vecset(vec, m, x->val[tk->idx]);
			m++;
			tk = tk->link;
		}
	}
	return vec;
}

static u32
listxlen(Listx *x)
{
	return x->tl-x->hd;
}

static Listx*
mklistx(u32 sz)
{
	Listx *x;
	x = xmalloc(sizeof(Listx));
	x->hd = x->tl = sz/2;
	x->sz = sz;
	x->val = xmalloc(x->sz*sizeof(Val)); /* must be 0 or Xundef */
	return x;
}

static List*
_mklist(Listx *x)
{
	List *lst;
	lst = (List*)halloc(&heap[Qlist]);
	lst->x = x;
	return lst;
}

static List*
mklist()
{
	return _mklist(mklistx(Listinitsize));
}

static void
freelistx(Listx *x)
{
	free(x->val);
	free(x->oval);
	free(x);
}

static void
freelist(Head *hd)
{
	List *lst;
	lst = (List*)hd;
	freelistx(lst->x);
}

static void
listxaddroots(VM *vm, Listx *x, u32 idx, u32 n)
{
	if(!vm->gcrun)
		return;
	while(n-- > 0)
		addroot(&stores, valhead(x->val[x->hd+idx+n]));
}

static Val
listref(VM *vm, List *lst, Imm idx)
{
	Listx *x;
	x = lst->x;
	if(idx >= listxlen(x))
		vmerr(vm, "listref out of bounds");
	return x->val[x->hd+idx];
}

static List*
listset(VM *vm, List *lst, Imm idx, Val v)
{
	Listx *x;
	x = lst->x;
	if(idx >= listxlen(x))
		vmerr(vm, "listset out of bounds");
	listxaddroots(vm, x, idx, 1);
	x->val[x->hd+idx] = v;
	return lst;
}

static List*
listcopy(List *lst)
{
	List *new;
	u32 hd, tl, len;
	new = _mklist(mklistx(lst->x->sz));
	new->x->hd = hd = lst->x->hd;
	new->x->tl = tl = lst->x->tl;
	len = tl-hd;
	memcpy(&new->x->val[hd], &lst->x->val[hd], len*sizeof(Val));
	return new;
}

static List*
listreverse(List *lst)
{
	List *new;
	u32 hd, tl, len, m;
	new = _mklist(mklistx(lst->x->sz));
	new->x->hd = hd = lst->x->hd;
	new->x->tl = tl = lst->x->tl;
	len = tl-hd;
	for(m = 0; m < len; m++)
		new->x->val[hd++] = lst->x->val[--tl];
	return new;
}

static Val
listhead(VM *vm, List *lst)
{
	Listx *x;
	x = lst->x;
	if(listxlen(x) == 0)
		vmerr(vm, "head on empty list");
	return x->val[x->hd];
}

static void
listpop(VM *vm, List *lst, Val *vp)
{
	Listx *x;
	x = lst->x;
	if(listxlen(x) == 0)
		vmerr(vm, "pop on empty list");
	*vp = x->val[x->hd];
	listxaddroots(vm, x, 0, 1);
	x->val[x->hd] = Xundef;
	x->hd++;
}

static List*
listtail(VM *vm, List *lst)
{
	List *new;
	if(listxlen(lst->x) == 0)
		vmerr(vm, "tail on empty list");
	new = listcopy(lst);
	new->x->hd++;
	return new;
}

static int
equallist(List *a, List *b)
{
	Listx *xa, *xb;
	u32 len, m;
	xa = a->x;
	xb = b->x;
	len = listxlen(xa);
	if(len != listxlen(xb))
		return 0;
	for(m = 0; m < len; m++)
		if(!eqval(xa->val[xa->hd+m], xb->val[xb->hd+m]))
			return 0;
	return 1;
}

static int
equallistv(Val a, Val b)
{
	return equallist(vallist(a), vallist(b));
}

static void
listexpand(List *lst)
{
	Listx *x, *nx;
	u32 len;

	x = lst->x;
	nx = mklistx(x->sz*2);
	len = listxlen(x);
	if(x->hd == 0){
		/* expanding to the left */
		nx->hd = x->sz;
		nx->tl = x->tl+x->sz;
	}else{
		/* expanding to the right */
		nx->hd = x->hd;
		nx->tl = x->tl;
	}
	memcpy(&nx->val[nx->hd], &x->val[x->hd], len*sizeof(Val));
	_mklist(x);		/* preserve potential concurrent iterlist's
				   view; same strategy as tabexpand. */
	lst->x = nx;
}

static Listx*
maybelistexpand(List *lst)
{
	Listx *x;
again:
	x = lst->x;
	if(x->hd == 0 || x->tl == x->sz){
		listexpand(lst);
		goto again;	/* at most twice iff full on both ends */
	}
	return lst->x;
}

/* maybe listdel and listins should slide whichever half is shorter */

enum {
	SlideIns,
	SlideDel,
};

/* if op==SlideIns, expect room to slide by 1 in either direction;
   however, currently we always slide to right */
static void
slide(Listx *x, u32 idx, int op)
{
	Val *t;
	u32 m;
	m = listxlen(x)-idx;
	if(m <= 1 && op == SlideDel)
		/* deleting last element; nothing to do */
		return;		
	if(x->oval == 0)
		x->oval = xmalloc(x->sz*sizeof(Val));
	memcpy(&x->oval[x->hd], &x->val[x->hd], idx*sizeof(Val));
	switch(op){
	case SlideIns:
		memcpy(&x->oval[x->hd+idx+1], &x->val[x->hd+idx],
		       m*sizeof(Val));
		break;
	case SlideDel:
		memcpy(&x->oval[x->hd+idx], &x->val[x->hd+idx+1],
		       (m-1)*sizeof(Val));
		break;
	}
	t = x->val;
	x->val = x->oval;
	x->oval = t;
}

static List*
listdel(VM *vm, List *lst, Imm idx)
{
	Listx *x;
	u32 m, len;
	x = lst->x;
	len = listxlen(x);
	if(idx >= len)
		vmerr(vm, "listdel out of bounds");
	m = len-idx;
	listxaddroots(vm, x, idx, m);
	slide(x, idx, SlideDel);
	x->val[x->tl-1] = Xundef;
	x->tl--;
	return lst;
}

static List*
listins(VM *vm, List *lst, Imm idx, Val v)
{
	Listx *x;
	u32 m, len;
	x = lst->x;
	len = listxlen(x);
	if(idx > len)
		vmerr(vm, "listins out of bounds");
	x = maybelistexpand(lst);
	if(idx == 0)
		x->val[--x->hd] = v;
	else if(idx == len)
		x->val[x->tl++] = v;
	else{
		m = len-idx;
		listxaddroots(vm, x, idx, m);
		slide(x, idx, SlideIns);
		x->tl++;
		x->val[x->hd+idx] = v;
	}
	return lst;
}

static void
_listappend(List *lst, Val v)
{
	Listx *x;
	u32 idx;
	x = lst->x;
	idx = listxlen(x);
	x = maybelistexpand(lst);
	x->val[x->tl++] = v;
}

static List*
listpush(VM *vm, List *lst, Val v)
{
	return listins(vm, lst, 0, v);
}

static List*
listappend(VM *vm, List *lst, Val v)
{
	return listins(vm, lst, listxlen(lst->x), v);
}

static Head*
iterlist(Head *hd, Ictx *ictx)
{
	List *lst;
	Listx *x;
	lst = (List*)hd;
	if(ictx->n == 0)
		ictx->x = lst->x;
	x = ictx->x;
	if(ictx->n >= x->sz)
		return GCiterdone;
	// x->val may change from call to call because of slide,
	// but will always point to buffer of markable Vals
	return valhead(x->val[ictx->n++]);
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
			return valhead(xtn->sz);
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
			return valhead(xtn->cnt);
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
			return valhead(xtn->sz);
		case 1:
			return valhead(xtn->bit0);
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
	Val id;
	
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
			snprintf(buf, m, "%.*s %s", (int)s->len, s->s, o);
		else
			snprintf(buf, m, "%.*s", (int)s->len, s->s);
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
					 w, (int)s->len, s->s, o);
			else
				snprintf(buf, m, "%s %.*s", w,
					 (int)s->len, s->s);
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
		if(xtn->cnt->qkind == Qnil)
			snprintf(buf, m, "%s[]", o);
		else{
			cv = valcval(xtn->cnt);
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
mkas(Closure *dispatch, Str *name)
{
	As *as;
	as = (As*)halloc(&heap[Qas]);
	as->dispatch = dispatch;
	as->name = name;
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
	case 1:
		return (Head*)as->name;
	default:
		return GCiterdone;
	}
}

static Dom*
mkdom(Ns *ns, As *as, Str *name)
{
	Dom *dom;
	dom = (Dom*)halloc(&heap[Qdom]);
	dom->ns = ns;
	dom->as = as;
	dom->name = name;
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
	case 2:
		return (Head*)dom->name;
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
		return (Head*)ns->lookaddr;
	case 1:
		return (Head*)ns->looksym;
	case 2:
		return (Head*)ns->looktype;
	case 3:
		return (Head*)ns->enumtype;
	case 4:
		return (Head*)ns->enumsym;
	case 5:
		return (Head*)ns->name;
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
		*v = Xundef;
		hput(env->ht, xstrdup(id), strlen(id), v);
	}
	return v;
}

static void
envbind(Env *env, char *id, Val val)
{
	Val *v;

	v = envgetbind(env, id);
	*v = val;
}

static int
envlookup(Env *env, char *id, Val *val)
{
	Val *v;

	v = envgetbind(env, id);
	if((*v)->qkind == Qundef)
		return 0;
	*val = *v;
	return 1;
}

static void
freebinding(void *u, char *id, void *v)
{
	free(id);
	free((Val*)v);
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

static Val
mkvalcval(Dom *dom, Xtypename *t, Imm imm)
{
	return (Val)mkcval(dom, t, imm);
}

/* the difference between mkvalimm and mkvalcval is usage:
   the former is for VM literals, the latter for other values */
static Val
mkvalimm(Dom *dom, Xtypename *t, Imm imm)
{
	return mkvalcval(dom, t, imm);
}

static Val
mkvalcval2(Cval *cv)
{
	return (Val)cv;
}

Val
mklitcval(Cbase base, Imm val)
{
	return mkvalcval(litdom, litbase[base], val);
}

static Val
mkvalcl(Closure *cl)
{
	return (Val)cl;
}

static Val
mkvalbox(Val boxed)
{
	Box *box;
	box = (Box*)halloc(&heap[Qbox]);
	box->v = boxed;
	return (Val)box;
}

static Val
mkvalas(As *as)
{
	return (Val)as;
}

static Val
mkvaldom(Dom *dom)
{
	return (Val)dom;
}

static Val
mkvalfd(Fd *fd)
{
	return (Val)fd;
}

static Val
mkvallist(List *lst)
{
	return (Val)lst;
}

static Val
mkvalns(Ns *ns)
{
	return (Val)ns;
}

static Val
mkvalpair(Val car, Val cdr)
{
	Pair *pair;
	pair = (Pair*)halloc(&heap[Qpair]);
	pair->car = car;
	pair->cdr = cdr;
	return (Val)pair;
}

static Val
mkvalstr(Str *str)
{
	return (Val)str;
}

static Val
mkvaltab(Tab *tab)
{
	return (Val)tab;
}

static Val
mkvalvec(Vec *vec)
{
	return (Val)vec;
}

static Range*
mkrange(Cval *beg, Cval *len)
{
	Range *r;
	r = (Range*)halloc(&heap[Qrange]);
	r->beg = beg;
	r->len = len;
	return r;
}

static Val
mkvalrange(Cval *beg, Cval *len)
{
	Range *r;
	r = mkrange(beg, len);
	return (Val)r;
}

static Val
mkvalrange2(Range *r)
{
	return (Val)r;
}

static Val
mkvalxtn(Xtypename *xtn)
{
	return (Val)xtn;
}

static int
zeroval(Val v)
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
putbox(VM *vm, Val box, Val boxed)
{
	Box *b;
//	if(box->qkind != Qbox)
//		fatal("putbox on non-box");
//	if(boxed->qkind == Qbox)
//		fatal("boxing boxes is insane");
	b = (Box*)box;
	if(vm->gcrun)
		addroot(&stores, valhead(b->v));
	b->v = boxed;
}

static void
putval(VM *vm, Val v, Location *loc)
{
	Val *dst;

	switch(loc->kind){
	case Lreg:
		switch(loc->idx){
		case Rac:
			vm->ac = v;
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
			vm->cl = v;
			vmsetcl(vm, vm->cl);
			break;
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		dst = &vm->stack[(vm->fp+1)+loc->idx];
		if(loc->indirect)
			putbox(vm, *dst, v);
		else
			*dst = v;
		break;
	case Llocal:
		dst = &vm->stack[(vm->fp-1)-loc->idx];
		if(loc->indirect)
			putbox(vm, *dst, v);
		else
			*dst = v;
		break;
	case Ldisp:
		dst = &vm->clx->display[loc->idx];
		if(loc->indirect)
			putbox(vm, *dst, v);
		else
			*dst = v;
		break;
	case Ltopl:
		dst = loc->val;
		*dst = v;
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
		if(strcmp(cl->id, "$halt")){
//			fprintf(out, "fp=%05lld pc=%08lld ", fp, pc);
			printsrc(out, cl, pc);
		}
		narg = stkimm(vm->stack[fp]);
		pc = stkimm(vm->stack[fp+narg+1]);
		pc--; /* pc was insn following call */
		cl = valcl(vm->stack[fp+narg+2]);
		fp = stkimm(vm->stack[fp+narg+3]);
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

static Val
getval(VM *vm, Location *loc)
{
	Val p;

	switch(loc->kind){
	case Lreg:
		switch(loc->idx){
		case Rac:
			return vm->ac;
		case Rsp:
			return mkvalimm(vm->litdom, vm->litbase[Vuint], vm->sp);
		case Rfp:
			return mkvalimm(vm->litdom, vm->litbase[Vuint], vm->fp);
		case Rpc:
			return mkvalimm(vm->litdom, vm->litbase[Vuint], vm->pc);
		case Rcl:
			return vm->cl;
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		p = vm->stack[(vm->fp+1)+loc->idx];
		if(loc->indirect)
			return valboxed(p);
		else
			return p;
	case Llocal:
		p = vm->stack[(vm->fp-1)-loc->idx];
		if(loc->indirect)
			return valboxed(p);
		else
			return p;
	case Ldisp:
		p = vm->clx->display[loc->idx];
		if(loc->indirect)
			return valboxed(p);
		else
			return p;
	case Ltopl:
		p = *(loc->val);
		if(p == Xundef)
			vmerr(vm, "reference to undefined variable: %s",
			      topvecid(loc->idx, vm->clx->code->topvec));
		return p;
	default:
		fatal("bug");
	}
}

static Cval*
getcval(VM *vm, Location *loc)
{
	Val p;

	switch(loc->kind){
	case Lreg:
		switch(loc->idx){
		case Rac:
			return valcval(vm->ac);
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
		p = vm->stack[(vm->fp+1)+loc->idx];
		if(loc->indirect)
			return valboxedcval(p);
		return valcval(p);
	case Llocal:
		p = vm->stack[(vm->fp-1)-loc->idx];
		if(loc->indirect)
			return valboxedcval(p);
		return valcval(p);
	case Ldisp:
		p = vm->clx->display[loc->idx];
		if(loc->indirect)
			return valboxedcval(p);
		return valcval(p);
	case Ltopl:
		p = *(loc->val);
		if(p->qkind == Qundef)
			vmerr(vm, "reference to undefined variable: %s",
			      topvecid(loc->idx, vm->clx->code->topvec));
		return valcval(p);
	default:
		fatal("bug");
	}
}

static Val
getvalrand(VM *vm, Operand *r)
{
	switch(r->okind){
	case Oloc:
		return getval(vm, &r->u.loc);
	case Oliti:
		return r->u.liti;
	case Olits:
		return (Val)mkstrlits(r->u.lits);
	case Onil:
		return Xnil;
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
		return valcval(r->u.liti);
	default:
		fatal("bug");
	}
}

static void
putvalrand(VM *vm, Val v, Operand *r)
{
	if(r->okind != Oloc)
		fatal("bad destination");
	putval(vm, v, &r->u.loc);
}

static void
putcvalrand(VM *vm, Cval *cv, Operand *r)
{
	if(r->okind != Oloc)
		fatal("bad destination");
	putval(vm, mkvalcval2(cv), &r->u.loc);
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
imm2str(VM *vm, Xtypename *xtn, Imm imm)
{
	Str *str;
	char *s;

	xtn = chasetype(xtn);
	if(xtn->tkind != Tbase && xtn->tkind != Tptr)
		fatal("imm2str on non-scalar type");

	switch(xtn->rep){
	case Rs08le:
		str = mkstrn(vm, sizeof(s8));
		s = str->s;
		*(s8*)s = (s8)imm;
		return str;
	case Rs16le:
		str = mkstrn(vm, sizeof(s16));
		s = str->s;
		*(s16*)s = (s16)imm;
		return str;
	case Rs32le:
		str = mkstrn(vm, sizeof(s32));
		s = str->s;
		*(s32*)s = (s32)imm;
		return str;
	case Rs64le:
		str = mkstrn(vm, sizeof(s64));
		s = str->s;
		*(s64*)s = (s64)imm;
		return str;
	case Ru08le:
		str = mkstrn(vm, sizeof(u8));
		s = str->s;
		*(u8*)s = (u8)imm;
		return str;
	case Ru16le:
		str = mkstrn(vm, sizeof(u16));
		s = str->s;
		*(u16*)s = (u16)imm;
		return str;
	case Ru32le:
		str = mkstrn(vm, sizeof(u32));
		s = str->s;
		*(u32*)s = (u32)imm;
		return str;
	case Ru64le:
		str = mkstrn(vm, sizeof(u64));
		s = str->s;
		*(u64*)s = (u64)imm;
		return str;
	case Rs08be:
		str = mkstrn(vm, sizeof(s8));
		s = str->s;
		putbeint(s, imm, 1);
		return str;
	case Rs16be:
		str = mkstrn(vm, sizeof(s16));
		s = str->s;
		putbeint(s, imm, 2);
		return str;
	case Rs32be:
		str = mkstrn(vm, sizeof(s32));
		s = str->s;
		putbeint(s, imm, 4);
		return str;
	case Rs64be:
		str = mkstrn(vm, sizeof(s64));
		s = str->s;
		putbeint(s, imm, 8);
		return str;
	case Ru08be:
		str = mkstrn(vm, sizeof(u8));
		s = str->s;
		putbeint(s, imm, 1);
		return str;
	case Ru16be:
		str = mkstrn(vm, sizeof(u16));
		s = str->s;
		putbeint(s, imm, 2);
		return str;
	case Ru32be:
		str = mkstrn(vm, sizeof(u32));
		s = str->s;
		putbeint(s, imm, 4);
		return str;
	case Ru64be:
		str = mkstrn(vm, sizeof(u64));
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
	old = chasetype(old);
	new = chasetype(new);
	if(old->rep == Rundef || new->rep == Rundef)
		fatal("undef!");
	switch((new->rep<<5)|old->rep){
		#include "rerep.switch"
	}
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
		      (int)es->len, es->s);
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

static Xtypename*
commontype(Xtypename *t1, Xtypename *t2)
{
	Xtypename *p1, *p2;
	p1 = t1;
	while(1){
		p2 = t2;
		while(1){
			if(eqxtn(p1, p2))
				return p1;
			if(p2->tkind != Ttypedef && p2->tkind != Tenum)
				break;
			p2 = p2->link;
		}
		if(p1->tkind != Ttypedef && p1->tkind != Tenum)
			break;
		p1 = p1->link;
	}
	fatal("cannot determine common type");
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
	Xtypename *t1, *t2, *b1, *b2, *nxtn;

	op1 = intpromote(vm, op1);
	op2 = intpromote(vm, op2);

	t1 = op1->type;
	t2 = op2->type;
	b1 = chasetype(t1);
	b2 = chasetype(t2);
	if(b1->tkind != Tbase || b2->tkind != Tbase){
		/* presumably one of them is a Tptr; nothing else is sane. */
		*rv1 = op1;
		*rv2 = op2;
		return;
	}

	c1 = b1->basename;
	c2 = b2->basename;
	if(c1 == c2){
		/* combinations of distinct typedefs
		   and/or enums yield the first type
		   they have in common (not necessarily
		   the base type). */
		if(t1->tkind == Ttypedef || t2->tkind == Ttypedef){
			nxtn = commontype(t1, t2);
			if(t1 != nxtn)
				op1 = typecast(vm, nxtn, op1);
			if(t2 != nxtn)
				op2 = typecast(vm, nxtn, op2);
		}
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
xcallc(VM *vm)
{
	Imm argc;
	Val *argv;
	Val rv;

	if(vm->clx->cfn == 0 && vm->clx->ccl == 0)
		vmerr(vm, "bad closure for builtin call");

	rv = Xnil;
	argc = stkimm(vm->stack[vm->fp]);
	argv = &vm->stack[vm->fp+1];
	if(vm->clx->cfn)
		vm->clx->cfn(vm, argc, argv, &rv);
	else
		vm->clx->ccl(vm, argc, argv, vm->clx->display, &rv);
	vm->ac = rv;
}

static int
Strcmp(Str *s1, Str *s2)
{
	unsigned char *p1, *p2;
	Imm l1, l2;

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
			cvr = cval0;
		else
			cvr = cval1;
		break;
	default:
		fatal("unknown unary operator %d", op);
	}
	putcvalrand(vm, cvr, dst);
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
	if(rv)
		return cval1;
	else
		return cval0;
}

static void
xbinop(VM *vm, ikind op, Operand *op1, Operand *op2, Operand *dst)
{
	Val v1, v2;
	Cval *cv1, *cv2, *cvr;
	Str *s1, *s2;
	Imm nv;

	v1 = getvalrand(vm, op1);
	v2 = getvalrand(vm, op2);

	if(v1->qkind == Qcval && v2->qkind == Qcval){
		cv1 = valcval(v1);
		cv2 = valcval(v2);
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
		putcvalrand(vm, cvr, dst);
		return;
	}

	if(v1->qkind == Qstr && v2->qkind == Qstr){
		s1 = valstr(v1);
		s2 = valstr(v2);
dostr:
		nv = binopstr(vm, op, s1, s2);
		if(nv)
			putvalrand(vm, mkvalcval2(cval1), dst);
		else
			putvalrand(vm, mkvalcval2(cval0), dst);
		return;
	}

	if(v1->qkind == Qstr && v2->qkind == Qcval){
		cv2 = valcval(v2);
		if(isstrcval(cv2)){
			s1 = valstr(v1);
			s2 = stringof(vm, cv2);
			goto dostr;
		}
		/* fall through */
	}

	if(v2->qkind == Qstr && v1->qkind == Qcval){
		cv1 = valcval(v1);
		if(isstrcval(cv1)){
			s1 = stringof(vm, cv1);
			s2 = valstr(v2);
			goto dostr;
		}
		/* fall through */
	}

	/* all other types support only == and != */
	if(op != Icmpeq && op != Icmpneq)
		vmerr(vm, "incompatible operands for binary %s", opstr[op]);

	nv = eqval(v1, v2);
	if(op == Icmpneq)
		nv = !nv;
	if(nv)
		putvalrand(vm, mkvalcval2(cval1), dst);
	else
		putvalrand(vm, mkvalcval2(cval0), dst);
}

static void
xclo(VM *vm, Operand *dl, Ctl *label, Operand *dst)
{
	Closure *cl;
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

	putvalrand(vm, mkvalcl(cl), dst);
}

static void
xkg(VM *vm, Operand *dst)
{
	Closure *k;
	Imm len;

	len = Maxstk-vm->sp;
	k = mkcl(kcode, 0, len, 0);
	memcpy(k->display, &vm->stack[vm->sp], len*sizeof(Val));
	k->fp = vm->fp;

	putvalrand(vm, mkvalcl(k), dst);
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
	v = getvalrand(vm, src);
	putvalrand(vm, v, dst);
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
	v = getvalrand(vm, src);
	if(zeroval(v))
		vm->pc = label->insn;
}

static void
checkoverflow(VM *vm, unsigned dec)
{
	if(dec > vm->sp)
		fatal("stack overflow");
}

static void
vmpush(VM *vm, Val v)
{
	checkoverflow(vm, 1);
	vm->stack[--vm->sp] = v;
}

static void
vmpushi(VM *vm, Imm imm)
{
	checkoverflow(vm, 1);
	imm = (imm<<1)|1;
	vm->stack[--vm->sp] = (Val)(uintptr_t)imm;
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
	v = getvalrand(vm, op);
	vmpush(vm, v);
}

static void
xpushi(VM *vm, Operand *op)
{
	Val v;
	Cval *cv;

	v = getvalrand(vm, op);
	cv = valcval(v);
	vmpushi(vm, cv->val);
}

static void
xbox(VM *vm, Operand *op)
{
	Val v;
	v = getvalrand(vm, op);
	putvalrand(vm, mkvalbox(v), op);
}

static void
xbox0(VM *vm, Operand *op)
{
	putvalrand(vm, mkvalbox(Xundef), op);
}

static void
xref(VM *vm, Operand *dom, Operand *type, Operand *cval, Operand *dst)
{
	Val domv, typev, cvalv, rv;
	Xtypename *t, *b, *pt;
	Dom *d;
	Cval *cv;
	Imm imm;

	typev = getvalrand(vm, type);
	cvalv = getvalrand(vm, cval);
	domv = getvalrand(vm, dom);
	d = valdom(domv);
	t = valxtn(typev);
	cv = valcval(cvalv);

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
		rv = mkvalcval(d, pt, imm);
		break;
	case Tarr:
		/* construct pointer to first element */
		pt = mkptrxtn(t->link, d->ns->base[Vptr]->rep);
		imm = cv->val;
		imm = truncimm(imm, pt->rep);
		rv = mkvalcval(d, pt, imm);
		break;
	case Tbitfield:
		vmerr(vm, "attempt to construct pointer to a bitfield");
		break;
	case Tconst:
		vmerr(vm, "attempt to use enumeration constant as location");
	case Ttypedef:
		fatal("bug");
	}
	putvalrand(vm, rv, dst);
}

static int
dobitfieldgeom(Cval *addr, Xtypename *b, BFgeom *bfg)
{
	Cval *bit0, *bs;
	Xtypename *bb;
		
	bit0 = valcval(b->bit0);
	bs = valcval(b->sz);
	bfg->bp = 8*addr->val+bit0->val; /* FIXME: overflow bug */
	bfg->bs = bs->val;
	bb = chasetype(b->link);
	bfg->isbe = isbigendian[bb->rep];
	return bitfieldgeom(bfg);
}

static void
xcval(VM *vm, Operand *dom, Operand *type, Operand *cval, Operand *dst)
{
	Val domv, typev, cvalv, rv, p, argv[3];
	Imm imm;
	Dom *d;
	Xtypename *t, *b, *pt;
	Cval *cv, *len;
	Str *s, *es;
	BFgeom bfg;

	typev = getvalrand(vm, type);
	cvalv = getvalrand(vm, cval);
	domv = getvalrand(vm, dom);
	d = valdom(domv);
	t = valxtn(typev);
	cv = valcval(cvalv);

	b = chasetype(t);
	switch(b->tkind){
	case Tbitfield:
		if(b->link->tkind == Tundef){
			es = fmtxtn(b->link->link);
			vmerr(vm, "attempt to read object of undefined type: "
			      "%.*s", (int)es->len, es->s);
		}
		if(0 > dobitfieldgeom(cv, b, &bfg))
			vmerr(vm, "invalid bitfield access");
		argv[0] = mkvalstr(vm->sget);
		argv[1] = domv;
		argv[2] = mkvalrange(mkcval(vm->litdom,
					    vm->litbase[Vptr], bfg.addr),
				     mkcval(vm->litdom,
					    vm->litbase[Vptr], bfg.cnt));
		p = dovm(vm, d->as->dispatch, 3, argv);
		s = valstr(p);
		imm = bitfieldget(s->s, &bfg);
		rv = mkvalcval(d, b->link, imm);
		break;
	case Tconst:
		rv = mkvalcval(d, b->link, cv->val);
		break;
	case Tbase:
	case Tptr:
		len = mkcval(vm->litdom, vm->litbase[Vptr], typesize(vm, t));
		argv[0] = mkvalstr(vm->sget);
		argv[1] = domv;
		argv[2] = mkvalrange(cv, len);
		p = dovm(vm, d->as->dispatch, 3, argv);
		imm = str2imm(t, valstr(p));
		rv = mkvalcval(d, t, imm);
		break;
	case Tarr:
		/* construct pointer to first element */
		pt = mkptrxtn(t->link, d->ns->base[Vptr]->rep);
		imm = truncimm(cv->val, pt->rep);
		rv = mkvalcval(d, pt, imm);
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
		      (int)es->len, es->s);
	case Tenum:
	case Ttypedef:
		fatal("bug");
	}
	putvalrand(vm, rv, dst);
}

static int
iscvaltype(Xtypename *t)
{
	static int okay[Tntkind] = {
		[Tbase] = 1,
		[Tptr] = 1,
		[Tbitfield] = 1,
	};

	t = chasetype(t);
	return okay[t->tkind];
}

static void
xxcast(VM *vm, Operand *typeordom, Operand *cval, Operand *dst)
{
	Val typeordomv, cvalv, rv;
	Cval *cv;
	Dom *d;
	Xtypename *t;

	cvalv = getvalrand(vm, cval);
	if(cvalv->qkind != Qcval)
		vmerr(vm,
		      "operand 2 to extended cast operator must be a cvalue");
	cv = valcval(cvalv);

	typeordomv = getvalrand(vm, typeordom);
	if(typeordomv->qkind == Qxtn){
		t = valxtn(typeordomv);
		if(!iscvaltype(t))
			vmerr(vm, "illegal type conversion");
		rv = mkvalcval2(typecast(vm, t, cv));
	}else if(typeordomv->qkind == Qdom){
		d = valdom(typeordomv);
		rv = mkvalcval2(domcast(vm, d, cv));
	}else
		vmerr(vm,
		      "operand 2 to extended cast operator must be "
		      "a domain or ctype");
	putvalrand(vm, rv, dst);
}

static void
xlist(VM *vm, Operand *op, Operand *dst)
{
	Val v;
	Imm sp, n, i;
	List *lst;
	Val rv;
	
	v = getvalrand(vm, op);
	sp = valimm(v);
	n = stkimm(vm->stack[sp]);
	lst = mklist();
	for(i = 0; i < n; i++)
		listappend(vm, lst, vm->stack[sp+1+i]);
	rv = mkvallist(lst);
	putvalrand(vm, rv, dst);
}

static void
xsizeof(VM *vm, Operand *op, Operand *dst)
{
	Val v, rv;
	Imm imm;
	Xtypename *xtn;
	Cval *cv;

	v = getvalrand(vm, op);
	if(v->qkind == Qcval){
		cv = valcval(v);
		xtn = cv->type;
	}else if(v->qkind == Qxtn)
		xtn = valxtn(v);
	else
		vmerr(vm, "operand 1 to sizeof must be a type or cvalue");
	imm = typesize(vm, xtn);
	rv = mkvalcval(vm->litdom, vm->litbase[Vuint], imm);
	putvalrand(vm, rv, dst);
}

static void
nilfn(VM *vm, Imm argc, Val *argv, Val *rv)
{
}

/* enumsym for namespaces constructed by @names */
static void
enumsym(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to enumsym");
	*rv = disp[0];
}

/* enumtype for namespaces constructed by @names */
static void
enumtype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to enumtype");
	*rv = disp[0];
}

/* dispatch for abstract address spaces */
static void
nasdispatch(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *cmd;

	if(argc < 2)
		vmerr(vm,
		      "wrong number of arguments to address space dispatch");
	checkarg(vm, "nasdispatch", argv, 0, Qstr);
	cmd = valstr(argv[0]);
	if(eqstrc(cmd, "map")){
		if(argc != 1)
			vmerr(vm, "wrong number of arguments to map");
		*rv = mkvalvec(mkvec(0));
		return;
	}
	vmerr(vm, "attempt to access abstract address space");
}

static As*
mknas()
{
	As *as;
	as = mkas(mkcfn("nasdispatch", nasdispatch), mkstr0("nullas"));
	return as;
}

static void
sasdispatch(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s, *cmd, *dat;
	Range *r;
	Vec *v;
	Cval *beg, *end;
	Val val;

	s = valstr(disp[0]);
	if(argc < 1)
		vmerr(vm,
		      "wrong number of arguments to address space dispatch");
	checkarg(vm, "sasdispatch", argv, 0, Qstr);
	cmd = valstr(argv[0]);
	if(eqstrc(cmd, "get")){
		if(argc != 3)
			vmerr(vm, "wrong number of arguments to get");
		checkarg(vm, "sasdispatch", argv, 2, Qrange);
		r = valrange(argv[2]);
		beg = r->beg;
		end = xcvalalu(vm, Iadd, beg, r->len);
		if(beg->val > s->len)
			vmerr(vm, "out of bounds string access");
		if(end->val > s->len)
			vmerr(vm, "out of bounds string access");
		if(beg->val > end->val)
			vmerr(vm, "out of bounds string access");
		dat = strslice(s, beg->val, end->val);
		*rv = mkvalstr(dat);
	}else if(eqstrc(cmd, "put")){
		if(argc != 4)
			vmerr(vm, "wrong number of arguments to put");
		checkarg(vm, "sasdispatch", argv, 2, Qrange);
		checkarg(vm, "sasdispatch", argv, 3, Qstr);
		r = valrange(argv[2]);
		dat = valstr(argv[3]);
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
		if(argc != 2)
			vmerr(vm, "wrong number of arguments to map");
		v = mkvec(1);
		val = mkvalrange(cvalnull,
				 mkcval(vm->litdom,
					vm->litbase[Vptr], s->len));
		_vecset(v, 0, val);
		*rv = mkvalvec(v);
	}else
		vmerr(vm, "undefined operation on address space: %.*s",
		      (int)cmd->len, cmd->s);
}

static As*
mksas(Str *s)
{
	return mkas(mkccl("sasdispatch", sasdispatch, 1, mkvalstr(s)), 0);
}

static As*
mkzas(VM *vm, Imm len)
{
	return mksas(mkstrn(vm, len));
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
_dolooktype(VM *vm, Xtypename *xtn, Ns *ns)
{
	Val argv[1], rv;
	Xtypename *tmp, *xtmp, *new;
	Vec *vec;
	Imm i;
	Str *es;

	switch(xtn->tkind){
	case Tvoid:
		return mkvoidxtn();
	case Tbase:
	case Ttypedef:
	case Tstruct:
	case Tunion:
	case Tenum:
		argv[0] = mkvalxtn(xtn);
		rv = dovm(vm, ns->looktype, 1, argv);
		if(rv->qkind == Qnil)
			return 0;
		return valxtn(rv);
	case Tptr:
		new = gcprotect(vm, mkxtn());
		new->tkind = Tptr;
		new->link = _dolooktype(vm, xtn->link, ns);
		if(new->link == 0){
			es = fmtxtn(xtn->link);
			vmerr(vm, "name space does not define %.*s",
			      (int)es->len, es->s);
		}
		tmp = ns->base[Vptr];
		new->rep = tmp->rep;
		return new;
	case Tarr:
		new = gcprotect(vm, mkxtn());
		new->tkind = Tarr;
		new->link = _dolooktype(vm, xtn->link, ns);
		if(new->link == 0){
			es = fmtxtn(xtn->link);
			vmerr(vm, "name space does not define %.*s",
			      (int)es->len, es->s);
		}
		new->cnt = xtn->cnt;
		return new;
	case Tfun:
		new = gcprotect(vm, mkxtn());
		new->tkind = Tfun;
		new->link = _dolooktype(vm, xtn->link, ns);
		if(new->link == 0){
			es = fmtxtn(xtn->link);
			vmerr(vm, "name space does not define %.*s",
			      (int)es->len, es->s);
		}
		new->param = mkvec(xtn->param->len);
		for(i = 0; i < xtn->param->len; i++){
			vec = veccopy(valvec(vecref(xtn->param, i)));
			xtmp = valxtn(vecref(vec, Typepos));
			tmp = _dolooktype(vm, xtmp, ns);
			if(tmp == 0){
				es = fmtxtn(xtmp);
				vmerr(vm, "name space does not define %.*s",
				      (int)es->len, es->s);
			}
			vecset(vm, vec, Typepos, mkvalxtn(tmp)); 
			vecset(vm, new->param, i, mkvalvec(vec));
		}
		return new;
	case Tundef:
		/* FIXME: do we want this? */
		return _dolooktype(vm, xtn->link, ns);
	case Tconst:
	case Tbitfield:
		fatal("bug");
	}
	fatal("bug");
}

/* NS must have initialized base cache */
static Xtypename*
dolooktype(VM *vm, Xtypename *xtn, Ns *ns)
{
	switch(xtn->tkind){
	case Tvoid:
		return mkvoidxtn();
	case Tbase:
		return ns->base[xtn->basename];
	default:
		return _dolooktype(vm, xtn, ns);
	}
}

static void
nscache1base(VM *vm, Ns *ns, Cbase cb)
{
	Xtypename *xtn;

	/* will be garbage; safe across dolooktype
	   because as an argument to looktype it will
	   be in vm roots */
	xtn = mkbasextn(cb, Rundef);

	xtn = _dolooktype(vm, xtn, ns);
	if(xtn == 0)
		vmerr(vm, "name space does not define %s", basename[cb]);
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
	Tab *sym;
	Val vp;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to looksym");
	if(argv[0]->qkind != Qstr)
		vmerr(vm, "argument 1 to looksym must be a string");
	sym = valtab(disp[0]);
	vp = tabget(sym, argv[0]);
	if(vp)
		*rv = vp;
}

/* looktype for namespaces constructed by @names */
static void
looktype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Tab *type;
	Val vp;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to looktype");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "argument 1 to looktype must be a typename");

	type = valtab(disp[0]);
	vp = tabget(type, argv[0]);
	if(vp)
		*rv = vp;
}

/* lookaddr for namespaces constructed by @names */
static void
lookaddr(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	List *l;
	Cval *cv, *a;
	Imm addr, m, i, b, n;
	Vec *sym;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to lookaddr");
	if(argv[0]->qkind != Qcval)
		vmerr(vm, "argument 1 to lookaddr must be an address");

	l = vallist(disp[0]);
	cv = valcval(argv[0]);
	addr = cv->val;
	listlen(disp[0], &n);
	if(n == 0){
		*rv = Xnil;
		return;
	}
	sym = valvec(listref(vm, l, 0));
	a = valcval(vecref(sym, Offpos));
	if(a->val > addr){
		*rv = Xnil;
		return;
	}

	/* binary search */
	b = 0;
	while(1){
		i = n/2;
		m = b+i;
		if(i == 0)
			break;
		sym = valvec(listref(vm, l, m));
		a = valcval(vecref(sym, Offpos));
		if(addr < a->val)
			n = i;
		else{
			b = m;
			n = n-i;
		}
	}
	sym = valvec(listref(vm, l, m));
	*rv = mkvalvec(sym);
}

typedef
struct NSctx {
	Ns *ons;		/* name space being extended */
	Tab *otype, *osym;	/* bindings passed to @names */
	Tab *rawtype, *rawsym;	/* @names declarations */
	Tab *type, *sym;	/* resulting bindings */
	Tab *undef;		/* undefined type names */
	Rkind ptrrep;		/* pointer representation */
} NSctx;

static Xtypename* resolvetypename(VM *vm, Xtypename *xtn, NSctx *ctx);

static Xtypename*
resolvetid(VM *vm, Val xtnv, NSctx *ctx)
{
	Val rv;
	Xtypename *xtn, *new;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, xtnv);
	if(rv)
		return valxtn(rv);

	/* do we have an unprocessed definition for the type? */
	rv = tabget(ctx->rawtype, xtnv);
	if(rv){
		xtn = valxtn(xtnv);
		new = mktypedefxtn(xtn->tid, 0);

		/* bind before recursive resolve call to stop cycles */
		tabput(vm, ctx->type, xtnv, mkvalxtn(new));

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

/* determine a common type for all constants defined in an enumeration;
   cast the constants to this type in place.  return the type. */
static Xtypename*
doenconsts(VM *vm, Vec *v, Ns *ns)
{
	Dom *d;
	Vec *e;
	Imm m;
	Cval *a, *cv;
	Xtypename *t;
	Val val;

	/* abstract domain for name space being extended */
	d = mkdom(ns, mknas(), 0);

	/* this code assumes that domcast does not dolooktype */

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
		val = mkvalcval2(cv);
		vecset(vm, e, 1, val);
	}

	return t;
}

static Xtypename*
resolvetag(VM *vm, Val xtnv, NSctx *ctx)
{
	Val rv, v, vp, sz;
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
			sz = xtn->sz;

			new = mkxtn();
			new->tkind = xtn->tkind;
			new->tag = xtn->tag;
			new->field = mkvec(fld->len);
			new->sz = sz;

			/* bind before recursive resolve call to stop cycles */
			tabput(vm, ctx->type, xtnv, mkvalxtn(new));

			for(i = 0; i < fld->len; i++){
				vec = valvec(vecref(fld, i));
				vp = vecref(vec, Typepos);
				tmp = valxtn(vp);
				tmp = resolvetypename(vm, tmp, ctx);
				v = mkvalxtn(tmp);
				fv = mkvec(3);
				_vecset(fv, Typepos, v);
				_vecset(fv, Idpos, vecref(vec, 1));
				_vecset(fv, Offpos, vecref(vec, 2));
				v = mkvalvec(fv);
				_vecset(new->field, i, v);
			}
			return new;
		case Tenum:
			tmp = doenconsts(vm, valvec(xtn->konst), ctx->ons);
			new = mkxtn();
			new->tkind = Tenum;
			new->tag = xtn->tag;
			new->konst = xtn->konst;

			tabput(vm, ctx->type, xtnv, mkvalxtn(new));
			new->link = resolvetypename(vm, tmp, ctx);
			return new;
		default:
			es = fmtxtn(xtn);
			vmerr(vm, "bad definition for tagged type: %.*s",
			      (int)es->len, es->s);
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
resolvebase(VM *vm, Val xtnv, NSctx *ctx)
{
	Val rv;
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
		return resolvebase(vm, mkvalxtn(xtn), ctx);
	case Ttypedef:
		new = resolvetid(vm, mkvalxtn(xtn), ctx);
		return new;
	case Tenum:
	case Tstruct:
	case Tunion:
		new = resolvetag(vm, mkvalxtn(xtn), ctx);
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
			v = mkvalxtn(tmp);
			_vecset(pv, Typepos, v);
			_vecset(pv, Idpos, vecref(vec, Idpos));
			v = mkvalvec(pv);
			_vecset(new->param, i, v);
		}
		return new;
	case Tundef:
		fatal("bug");
	}
	fatal("bug");
}

static Ns*
mknsfn(Closure *looktype, Closure *enumtype,
       Closure *looksym, Closure *enumsym,
       Closure *lookaddr, Str *name)
{
	Ns *ns;
	ns = mkns();
	ns->looktype = looktype;
	ns->enumtype = enumtype;
	ns->looksym = looksym;
	ns->enumsym = enumsym;
	ns->lookaddr = lookaddr;
	ns->name = name;
	return ns;
}
       

static Ns*
mknstype(Tab *type, Str *name)
{
	return mknsfn(mkccl("looktype", looktype, 1, mkvaltab(type)),
		      mkccl("enumtype", enumtype, 1, mkvaltab(type)),
		      mkcfn("looksym", nilfn),
		      mkccl("enumsym", enumsym, 1, mkvaltab(mktab())),
		      mkcfn("lookaddr", nilfn),
		      name);
}

static void
symcmp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *sa, *sb;
	Cval *a, *b;
	sa = valvec(argv[0]);
	sb = valvec(argv[1]);
	a = valcval(vecref(sa, Offpos));
	b = valcval(vecref(sb, Offpos));
	if(a->val < b->val)
		*rv = mkvalcval2(cvalminus1);
	else if(a->val > b->val)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static Ns*
mknstypesym(VM *vm, Tab *type, Tab *sym, Str *name)
{
	Val rv, vp, op;
	Vec *vec, *s;
	List *ls;
	Imm m, len;
	Val argv[2];

	gcprotect(vm, type);
	gcprotect(vm, sym);
	gcprotect(vm, name);

	/* create sorted list of symbols with offsets for lookaddr*/
	vec = tabenum(sym);
	len = vec->len/2;
	ls = mklist();
	for(m = 0; m < len; m++){
		vp = vecref(vec, len+m);	/* symbol #m */
		s = valvec(vp);
		op = vecref(s, Offpos);
		if(op->qkind != Qcval)
			continue;
		_listappend(ls, vp);
	}
	argv[0] = mkvallist(ls);
	argv[1] = mkvalcl(mkcfn("symcmp", symcmp));
	gcprotect(vm, argv[0]);
	gcprotect(vm, argv[1]);
	l1_sort(vm, 2, argv, &rv);

	return mknsfn(mkccl("looktype", looktype, 1, mkvaltab(type)),
		      mkccl("enumtype", enumtype, 1, mkvaltab(type)),
		      mkccl("looksym", looksym, 1, mkvaltab(sym)),
		      mkccl("enumsym", enumsym, 1, mkvaltab(sym)),
		      mkccl("lookaddr", lookaddr, 1, mkvallist(ls)),
		      name);
}

static Ns*
mknstab(VM *vm, Ns *ons, Tab *rawtype, Tab *rawsym, Str *name)
{
	Val v, idv, vecv, vp;
	Vec *vec, *kvec, *nvec;
	Tabx *x;
	Tabidx *tk;
	Xtypename *xtn, *tmp;
	NSctx ctx;
	u32 i, j;
	Ns *ns;
	Imm m;
	Str *as;

	ctx.rawtype = rawtype;
	ctx.rawsym = rawsym;

	ctx.type = mktab();
	ctx.sym = mktab();
	ctx.undef = mktab();

	ctx.ons = ons;
	ctx.otype = valtab(dovm(vm, ons->enumtype, 0, 0));
	ctx.osym = valtab(dovm(vm, ons->enumsym, 0, 0));

	/* get pointer representation from parent name space */
	xtn = mkxtn();		/* will be garbage */
	xtn->tkind = Tbase;
	xtn->basename = Vptr;
	vp = tabget(ctx.otype, mkvalxtn(xtn));
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
			xtn = valxtn(x->key[tk->idx]);
			resolvetypename(vm, xtn, &ctx);
			tk = tk->link;
		}
	}

	x = ctx.rawsym->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			/* id -> [ xtn, id, off ] */
			idv = x->key[tk->idx];
			vecv = x->val[tk->idx];
			vec = valvec(vecv);
			xtn = valxtn(vecref(vec, Typepos));
			xtn = resolvetypename(vm, xtn, &ctx);
			if(xtn != 0){
				v = mkvalxtn(xtn);
				vecset(vm, vec, Typepos, v); /* reuse vec */
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
			vp = x->key[tk->idx];
			if(!tabget(ctx.type, vp))
				tabput(vm, ctx.type, vp, x->val[tk->idx]);
			tk = tk->link;
		}
	}
	x = ctx.osym->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			vp = x->key[tk->idx];
			if(!tabget(ctx.sym, vp))
				tabput(vm, ctx.sym, vp, x->val[tk->idx]);
			tk = tk->link;
		}
	}

	/* add enumeration constants to symtab */
	x = ctx.type->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			xtn = valxtn(x->val[tk->idx]);
			if(xtn->tkind != Tenum)
				goto next;
			tmp = mkconstxtn(xtn);
			vec = valvec(xtn->konst);
			for(j = 0; j < vec->len; j++){
				kvec = valvec(vecref(vec, j));
				nvec = mkvec(3);
				_vecset(nvec, 0, mkvalxtn(tmp));
				_vecset(nvec, 1, vecref(kvec, 0));
				_vecset(nvec, 2, vecref(kvec, 1));
				tabput(vm, ctx.sym,
				       vecref(kvec, 0), mkvalvec(nvec));
			}
		next:
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
			printf("\t%.*s\n", (int)as->len, as->s);
		}
		fflush(stdout);
	}

	ns = mknstypesym(vm, ctx.type, ctx.sym, name);
	nscachebase(vm, ns);
	return ns;
}

static void* gotab[Iopmax];

static void
vmsetcl(VM *vm, Val val)
{
	unsigned k;
	Closure *cl;
	Insn *i;

	if(val->qkind != Qcl)
		vmerr(vm, "attempt to apply non-procedure");
	cl = valcl(val);
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
	freerootlist(&vm->rs, vm->prot[vm->pdepth-1]);
	vm->prot[vm->pdepth-1] = 0;
	vm->pdepth--;
}

static void
gcunprotect(VM *vm, Val v)
{
	Root *r, **pr;
	
	pr = &vm->prot[vm->pdepth-1];
	r = *pr;
	while(r){
		if(r->hd == v){
			(*pr)->link = r;
			freeroot(&vm->rs, r);
			break;
		}
		pr = &r->link;
		r = *pr;
	}
}

static void*
gcprotect(VM *vm, void *obj)
{
	Root *r;

	if(obj == 0)
		return 0;
	r = newroot(&vm->rs);
	r->hd = obj;
	r->link = vm->prot[vm->pdepth-1];
	vm->prot[vm->pdepth-1] = r;	
	return obj;
}

void
cqctgcprotect(VM *vm, void *obj)
{
	gcprotect(vm, obj);
}

void
vmreset(VM *vm)
{
	while(vm->pdepth > 0)
		gcprotpop(vm);
	gcprotpush(vm);
	vm->edepth = 0;
	vm->fp = 0;
	vm->sp = Maxstk;
	vm->ac = Xundef;
	vm->cl = mkvalcl(panicthunk());
}

Val
dovm(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	static int once;
	static Val haltv;
	static Closure *halt;
	Insn *i;
	Cval *cv;
	Val val;
	Imm m, narg, onarg;

	if(!once){
		once = 1;
		gotab[Iadd]	= &&Iadd;
		gotab[Iand]	= &&Iand;
		gotab[Iargc]	= &&Iargc;
		gotab[Ibox]	= &&Ibox;
		gotab[Ibox0]	= &&Ibox0;
		gotab[Icall]	= &&Icall;
		gotab[Icallc]	= &&Icallc;
		gotab[Icallt]	= &&Icallt;
		gotab[Iclo]	= &&Iclo;
		gotab[Icmpeq] 	= &&Icmpeq;
		gotab[Icmpgt] 	= &&Icmpgt;
		gotab[Icmpge] 	= &&Icmpge;
		gotab[Icmplt] 	= &&Icmplt;
		gotab[Icmple] 	= &&Icmple;
		gotab[Icmpneq] 	= &&Icmpneq;
		gotab[Icval] 	= &&Icval;
		gotab[Idiv] 	= &&Idiv;
		gotab[Iframe] 	= &&Iframe;
		gotab[Ihalt] 	= &&Ihalt;
		gotab[Iinv] 	= &&Iinv;
		gotab[Ijmp] 	= &&Ijmp;
		gotab[Ijnz] 	= &&Ijnz;
		gotab[Ijz] 	= &&Ijz;
		gotab[Ikg] 	= &&Ikg;
		gotab[Ikp] 	= &&Ikp;
		gotab[Ilist]	= &&Ilist;
		gotab[Imod] 	= &&Imod;
		gotab[Imov] 	= &&Imov;
		gotab[Imul] 	= &&Imul;
		gotab[Ineg] 	= &&Ineg;
		gotab[Inot] 	= &&Inot;
		gotab[Ior] 	= &&Ior;
		gotab[Inop] 	= &&Inop;
		gotab[Ipanic] 	= &&Ipanic;
		gotab[Ipush] 	= &&Ipush;
		gotab[Ipushi] 	= &&Ipushi;
		gotab[Iref] 	= &&Iref;
		gotab[Iret] 	= &&Iret;
		gotab[Ishl] 	= &&Ishl;
		gotab[Ishr] 	= &&Ishr;
		gotab[Isizeof]	= &&Isizeof;
		gotab[Isub] 	= &&Isub;
		gotab[Ixcast] 	= &&Ixcast;
		gotab[Ixor] 	= &&Ixor;

		if(!envlookup(vm->top, "halt", &haltv))
			fatal("broken vm");
		halt = valcl(haltv);
	}

	gcprotpush(vm);

	/* for recursive entry, store current context */
	vmpushi(vm, vm->fp);	/* fp */
	vmpush(vm, vm->cl);	/* cl */
	vmpushi(vm, vm->pc);	/* pc */
	vmpushi(vm, 0);		/* narg */
	vm->fp = vm->sp;

	/* push frame for halt thunk */
	vmpushi(vm, vm->fp);	/* fp */
	vmpush(vm, haltv);	/* cl */
	vmpushi(vm, halt->entry); /* pc */
	for(m = argc; m > 0; m--)
		vmpush(vm, argv[m-1]);
	vmpushi(vm, argc);	/* narg */
	vm->fp = vm->sp;

	/* switch to cl */
	vm->cl = mkvalcl(cl);
	vmsetcl(vm, vm->cl);
	vm->pc = vm->clx->entry;

	while(1){
		i = &vm->ibuf[vm->pc++];
		tick++;
		gcpoll(vm);
		goto *(i->go);
		fatal("bug");
	Inop:
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
	Ipushi:
		xpushi(vm, &i->op1);
		continue;
	Iargc:
		val = getvalrand(vm, &i->op1);
		cv = valcval(val);
		if(stkimm(vm->stack[vm->fp]) != cv->val)
			vmerr(vm, "wrong number of arguments to %s",
			      vm->clx->id);
		continue;
	Icall:
		vm->cl = getvalrand(vm, &i->op1);
		vmsetcl(vm, vm->cl);
		vm->pc = vm->clx->entry;
		vm->fp = vm->sp;
		continue;
	Icallt:
		vm->cl = getvalrand(vm, &i->op1);
		vmsetcl(vm, vm->cl);
		/* shift current arguments over previous arguments */
		narg = stkimm(vm->stack[vm->sp]);
		onarg = stkimm(vm->stack[vm->fp]);
		vm->fp = vm->fp+onarg-narg;
		memmove(&vm->stack[vm->fp], &vm->stack[vm->sp],
			(narg+1)*sizeof(Val));
		vm->sp = vm->fp;
		vm->pc = vm->clx->entry;
		continue;
	Iframe:
		vmpushi(vm, vm->fp);
		vmpush(vm, vm->cl);
		vmpushi(vm, i->dstlabel->insn);
		continue;
	Ipanic:
		fatal("vm panic");
	Ihalt:
		/* Ihalt is exactly like Iret... */
		vm->sp = vm->fp+stkimm(vm->stack[vm->fp])+1; /* narg+1 */
		vm->fp = stkimm(vm->stack[vm->sp+2]);
		vm->cl = vm->stack[vm->sp+1];
		vmsetcl(vm, vm->cl);
		vm->pc = stkimm(vm->stack[vm->sp]);
		vmpop(vm, 3);

		/* ...except that it returns from dovm */
		gcprotpop(vm);
		return vm->ac;
	Iret:
		vm->sp = vm->fp+stkimm(vm->stack[vm->fp])+1; /* narg+1 */
		vm->fp = stkimm(vm->stack[vm->sp+2]);
		vm->cl = vm->stack[vm->sp+1];
		vmsetcl(vm, vm->cl);
		vm->pc = stkimm(vm->stack[vm->sp]);
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
	Icval:
		xcval(vm, &i->op1, &i->op2, &i->op3, &i->dst);
		continue;
	Iref:
		xref(vm, &i->op1, &i->op2, &i->op3, &i->dst);
		continue;
	Ixcast:
		xxcast(vm, &i->op1, &i->op2, &i->dst);
		continue;
	Ilist:
		xlist(vm, &i->op1, &i->dst);
		continue;
	Isizeof:
		xsizeof(vm, &i->op1, &i->dst);
		continue;
	}
}

static void
builtinfn(Env *env, char *name, Closure *cl)
{
	Val val;
	val = mkvalcl(cl);
	envbind(env, name, val);
}

static void
builtinns(Env *env, char *name, Ns *ns)
{
	Val val;
	val = mkvalns(ns);
	envbind(env, name, val);
}

static void
builtindom(Env *env, char *name, Dom *dom)
{
	Val val;
	val = mkvaldom(dom);
	envbind(env, name, val);
}

static void
builtincval(Env *env, char *name, Cval *cv)
{
	Val val;
	val = mkvalcval2(cv);
	envbind(env, name, val);
}

static void
builtinfd(Env *env, char *name, Fd *fd)
{
	Val val;
	val = mkvalfd(fd);
	envbind(env, name, val);
}

static void
checkarg(VM *vm, char *fn, Val *argv, unsigned arg, Qkind qkind)
{
	if(argv[arg]->qkind != qkind)
		vmerr(vm, "operand %d to %s must be a %s",
		      arg+1, fn, qname[qkind]);
}

static int
isnegcval(VM *vm, Val v)
{
	Cval *cv;
	Xtypename *t;
	if(v->qkind != Qcval)
		vmerr(vm, "got %s where expecting cvalue", qname[v->qkind]);
	cv = valcval(v);
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		vmerr(vm, "got %s where expecting integer cvalue",
		      tkindstr[t->tkind]);
	if(isunsigned[t->basename])
		return 0;
	return (s64)cv->val < 0;
}

static int
docmp(VM *vm, Val *vs, Imm i, Imm j, Closure *cmp)
{
	Val argv[2], rv;
	argv[0] = vs[i];
	argv[1] = vs[j];
	rv = dovm(vm, cmp, 2, argv);
	if(isnegcval(vm, rv))
		return -1;
	if(zeroval(rv))
		return 0;
	return 1;
}

static void
doswap(VM *vm, Val *vs, Imm i, Imm j)
{
	Val t;
	if(vm->gcrun){
		addroot(&stores, valhead(vs[i]));
		addroot(&stores, valhead(vs[j]));
	}
	t = vs[i];
	vs[i] = vs[j];
	vs[j] = t;
}

static void
dosort(VM *vm, Val *vs, Imm n, Closure *cmp)
{
	Imm lo, hi, p;
	if(n < 2)
		return;

	lo = 0;
	hi = n;
	p = n>>1;		/* weak pivot */
	doswap(vm, vs, p, lo);
	while(1){
		do
			lo++;
		while(lo < n && docmp(vm, vs, lo, 0, cmp) < 0);
		do
			hi--;
		while(0 < hi && docmp(vm, vs, hi, 0, cmp) > 0);
		if(hi < lo)
			break;
		doswap(vm, vs, lo, hi);
	}
	doswap(vm, vs, 0, hi);
	dosort(vm, vs, hi, cmp);
	dosort(vm, vs+hi+1, n-hi-1, cmp);
}

static void
l1_sort(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Listx *x;
	Vec *v;
	Val *vs;
	Closure *cmp;
	Imm n;
	
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to sort");
	if(argv[0]->qkind != Qlist && argv[0]->qkind != Qvec)
		vmerr(vm, "operand 1 to sort must a list or vector");
	checkarg(vm, "sort", argv, 1, Qcl);
	cmp = valcl(argv[1]);
	switch(argv[0]->qkind){
	case Qlist:
		l = vallist(argv[0]);
		x = l->x;
		n = x->tl-x->hd;
		vs = &x->val[x->hd];
		break;
	case Qvec:
		v = valvec(argv[0]);
		n = v->len;
		vs = v->vec;
		break;
	default:
		return;
	}
	if(n < 2)
		return;
	dosort(vm, vs, n, cmp);
}

static void
l1_getpid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	*rv = mkvalcval(vm->litdom, vm->litbase[Vulong], getpid());
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
	*rv = mkvalcval(vm->litdom, vm->litbase[Vulong], tod);
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
	*rv = mkvalcval(vm->litdom, vm->litbase[Vulong], r);
}

static void
l1_looktype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	Xtypename *xtn;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looktype");

	if(argv[0]->qkind == Qns)
		ns = valns(argv[0]);
	else if(argv[0]->qkind == Qdom){
		dom = valdom(argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to looktype must be a namespace or domain");
		      
	if(argv[1]->qkind != Qxtn)
		vmerr(vm, "operand 2 to looktype must be a typename");
	xtn = valxtn(argv[1]);

	xtn = dolooktype(vm, xtn, ns);
	if(xtn)
		*rv = mkvalxtn(xtn);
}

static void
l1_domof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to domof");
	arg0 = argv[0];
	if(arg0->qkind != Qcval)
		vmerr(vm,
		      "operand 1 to domof must be a cvalue");
	cv = valcval(arg0);
	*rv = mkvaldom(cv->dom);
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

static Vec*
callmap(VM *vm, As *as)
{
	Val argv[2], rv;
	argv[0] = mkvalstr(vm->smap);
	argv[1] = mkvalas(as);
	rv = dovm(vm, as->dispatch, 2, argv);
	if(rv->qkind != Qvec)
		vmerr(vm, "address space map returned invalid value");
	return valvec(rv);
}

static Range*
mapstab(VM *vm, Vec *map, Imm addr, Imm len)
{
	Imm m;
	Val rp;
	Range *r;

	for(m = 0; m < map->len; m++){
		rp = vecref(map, m);
		if(rp->qkind != Qrange)
			vmerr(vm, "address space map returned invalid value");
		r = valrange(rp);
		if(r->beg->val > addr)
			return 0;
		if(r->beg->val+r->len->val >= addr+len)
			return r;
	}
	return 0;
}


/* assume CV has been vetted by isstrcval */
static Str*
stringof(VM *vm, Cval *cv)
{
	Str *s;
	char *buf, *q;
	Val p, argv[3];
	Vec *v;
	Range *r;
	Imm l, m, n, o;
	static unsigned unit = 128;
	
	v = callmap(vm, cv->dom->as);
	r = mapstab(vm, v, cv->val, 0);	/* FIXME: type sanity */
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
		argv[0] = mkvalstr(vm->sget);
		argv[1] = mkvaldom(cv->dom);
		r = mkrange(mkcval(cv->dom, cv->dom->ns->base[Vptr], o),
			    mkcval(cv->dom, cv->dom->ns->base[Vptr], n));
		gcprotect(vm, r); /* FIXME: why? */
		argv[2] = mkvalrange2(r);
		p = dovm(vm, cv->dom->as->dispatch, 3, argv);
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

static int
ismapped(VM *vm, Cval *addr, Cval *len)
{
	Imm sz;
	Vec *v;
	Range *r;

	if(len == 0)
		sz = typesize(vm, addr->type);
	else
		sz = len->val;	/* FIXME: check sign */
	if(addr->val+sz < addr->val)
		/* bogus size */
		return 0;	
	v = callmap(vm, addr->dom->as);
	r = mapstab(vm, v, addr->val, sz);	/* FIXME: type sanity */
	return r != 0;
}

typedef struct Fmt Fmt;
struct Fmt {
	char *start, *to, *stop;
	int (*flush)(Fmt*);
	void *farg;
};

static int
fmtputc(Fmt *f, char ch)
{
	if(f->to >= f->stop)
		if(f->flush(f) == -1)
			return -1;
	*f->to++ = ch;
	return 0;
}

static int
fmtputs(Fmt *f, char *p, Imm m)
{
	while(m > 0){
		if(f->to >= f->stop)
			if(f->flush(f) == -1)
				return -1;
		*f->to++ = *p++;
		m--;
	}
	return 0;
}

static int
fmtputs0(Fmt *f, char *p)
{
	return fmtputs(f, p, strlen(p));
}

static int
fmtval(Fmt *f, Val val)
{
	char buf[256];
	Cval *cv;
	Closure *cl;
	List *l;
	Listx *lx;
	Vec *v;
	Range *r;
	Str *str;
	Val bv;
	u32 m;
	Head *hd;
	Dom *d;
	As *as;

	switch(val->qkind){
	case Qcval:
		cv = valcval(val);
		snprintf(buf, sizeof(buf), "<cval %" PRIu64 ">", cv->val);
		return fmtputs0(f, buf);
	case Qcl:
		cl = valcl(val);
		if(cl->fp){
			snprintf(buf, sizeof(buf), "<continuation %p>", cl);
			return fmtputs0(f, buf);
		}else if(cl->dlen > 0){
			if(fmtputs0(f, "<closure "))
				return -1;
		}else{
			if(fmtputs0(f, "<procedure "))
				return -1;
		}
		if(fmtputs0(f, cl->id))
			return -1;
		return fmtputs0(f, ">");
	case Qundef:
		return 0;
	case Qnil:
		return fmtputs0(f, "<nil>");
	case Qnulllist:
		return fmtputs0(f, "<null>");
	case Qbox:
		if(fmtputs0(f, "<box >"))
			return -1;
		bv = valboxed(val);
		if(fmtval(f, bv))
			return -1;
		return fmtputs0(f, ">");
	case Qas:
		as = valas(val);
		if(as->name)
			snprintf(buf, sizeof(buf), "<as %.*s>",
				 (int)as->name->len, as->name->s);
		else
			snprintf(buf, sizeof(buf), "<as %p>", as);
		return fmtputs0(f, buf);
	case Qdom:
		d = valdom(val);
		if(d->name)
			snprintf(buf, sizeof(buf), "<domain %.*s>",
				 (int)d->name->len, d->name->s);
		else
			snprintf(buf, sizeof(buf), "<domain %p>", d);
		return fmtputs0(f, buf);
	case Qfd:
	case Qns:
	case Qpair:
	case Qtab:
	case Qxtn:
		hd = valhead(val);
		snprintf(buf, sizeof(buf), "<%s %p>", hd->heap->id, hd);
		return fmtputs0(f, buf);
	case Qvec:
		v = valvec(val);
		if(fmtputs0(f, "vector("))
			return -1;
		for(m = 0; m < v->len; m++){
			if(m > 0){
				if(fmtputs0(f, ", "))
					return -1;
			}else{
				if(fmtputs0(f, " "))
					return -1;
			}
			if(fmtval(f, v->vec[m]))
				return -1;
		}
		return fmtputs0(f, " )");
	case Qlist:
		l = vallist(val);
		lx = l->x;
		if(fmtputs0(f, "["))
			return -1;
		for(m = 0; m < listxlen(lx); m++){
			if(m > 0){
				if(fmtputs0(f, ", "))
					return -1;
			}else{
				if(fmtputs0(f, " "))
					return -1;
			}
			if(fmtval(f, lx->val[lx->hd+m]))
				return -1;
		}
		return fmtputs0(f, " ]");
	case Qrange:
		r = valrange(val);
 		snprintf(buf, sizeof(buf),
			 "<range 0x%" PRIx64 " 0x%" PRIx64 ">",
			 r->beg->val, r->len->val);
		return fmtputs0(f, buf);
	case Qstr:
		str = valstr(val);
		return fmtputs(f, str->s, str->len);
	default:
		printf("<unprintable type %d>", val->qkind);
		return -1;
	}
}


static int
fmticval(Fmt *f, unsigned char conv, Cval *cv)
{
	static char buf[Maxprintint];
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
		snprintf(buf, sizeof(buf), fmt, (u8)cv->val);
		break;
	case Rs08le:
	case Rs08be:
		snprintf(buf, sizeof(buf), fmt, (s8)cv->val);
		break;
	case Ru16le:
	case Ru16be:
		snprintf(buf, sizeof(buf), fmt, (u16)cv->val);
		break;
	case Rs16le:
	case Rs16be:
		snprintf(buf, sizeof(buf), fmt, (s16)cv->val);
		break;
	case Ru32le:
	case Ru32be:
		snprintf(buf, sizeof(buf), fmt, (u32)cv->val);
		break;
	case Rs32le:
	case Rs32be:
		snprintf(buf, sizeof(buf), fmt, (s32)cv->val);
		break;
	case Ru64le:
	case Ru64be:
		snprintf(buf, sizeof(buf), fmt, (u64)cv->val);
		break;
	case Rs64le:
	case Rs64be:
		snprintf(buf, sizeof(buf), fmt, (s64)cv->val);
		break;
	default:
		fatal("bug");
	}
	return fmtputs(f, buf, strlen(buf));
}

static int
fmtenconst(Fmt *f, Cval *cv)
{
	Xtypename *t;
	Vec *v;
	Cval *k;
	Str *s;
	u32 m;

	t = cv->type;
	while(t->tkind == Ttypedef)
		t = t->link;
	if(t->tkind != Tenum)
		return fmticval(f, 'd', cv);
	for(m = 0; m < t->konst->len; m++){
		v = valvec(vecref(t->konst, m));
		k = valcval(vecref(v, 1));
		/* direct comparison is sane because
		 * enum consts all have the same
		 * type */
		if(cv->val == k->val){
			s = valstr(vecref(v, 0));
			return fmtputs(f, s->s, s->len);
		}
	}
	return fmticval(f, 'd', cv);
}

static void
dofmt(VM *vm, Fmt *f, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	static char buf[3+Maxprintint];
	Val *vpp, vp, vq;
	Cval *cv;
	Str *as;
	char *efmt;
	char ch;
	unsigned char c;
	Xtypename *xtn;
	Vec *vec;

	vpp = &argv[0];
	efmt = fmt+fmtlen;
	while(1){
		vp = *vpp;
		while(fmt < efmt && (ch = *fmt++) != '%')
			if(fmtputc(f, ch))
				return;
		if(fmt >= efmt)
			return;
		ch = *fmt++;
		if(ch == '%'){
			if(fmtputc(f, ch))
				return;
			continue;
		}
		if(argc == 0)
			vmerr(vm, "format string needs more arguments");
		switch(ch){
		case 'a':
			if(fmtval(f, vp))
				return;
			break;
		case 'c':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			c = cv->val;
			if(isgraph(c) || isspace(c))
				snprintf(buf, sizeof(buf), "%c", c);
			else
				snprintf(buf, sizeof(buf), "\\%.3o", c);
			if(fmtputs(f, buf, strlen(buf)))
				return;
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
			if(fmticval(f, ch, cv))
				return;
			break;
		case 'e':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmtenconst(f, cv))
				return;
			break;
		case 'p':
			if(vp->qkind != Qcval)
				goto badarg;
			cv = valcval(vp);
			if(fmticval(f, 'x', cv))
				return;
			break;
		case 's':
			if(vp->qkind == Qstr)
				as = valstr(vp);
			else if(vp->qkind == Qcval){
				cv = valcval(vp);
				if(!isstrcval(cv))
					goto badarg;
				if(cv->val == 0)
					as = mkstr0("(null)");
				else
					as = stringof(vm, cv);
			}else
				goto badarg;
			if(fmtputs(f, as->s, as->len))
				return;
			break;
		case 't':
			if(vp->qkind == Qxtn)
				as = fmtxtn(valxtn(vp));
			else if(vp->qkind == Qvec){
				vec = valvec(vp);
				if(vec->len < 2)
					goto badarg;
				vq = vecref(vec, Typepos);
				if(vq->qkind != Qxtn)
					goto badarg;
				xtn = valxtn(vq);
				vq = vecref(vec, Idpos);
				if(vq->qkind == Qnil)
					as = fmtxtn(xtn);
				else if(vq->qkind == Qstr)
					as = fmtdecl(xtn, valstr(vq));
				else
					goto badarg;
			}else if(vp->qkind == Qcval){
				cv = valcval(vp);
				as = fmtxtn(cv->type);
			}else
				vmerr(vm, "bad operand to %%t");
			if(fmtputs(f, as->s, as->len))
				return;
			break;
		case 'y':
			if(vp->qkind != Qcval)
				vmerr(vm, "bad operand to %%y");
			cv = valcval(vp);
			vq = dovm(vm, cv->dom->ns->lookaddr, 1, &vp);
			cv = typecast(vm, cv->dom->ns->base[Vptr], cv);
			if(vq->qkind == Qnil){
				snprintf(buf, sizeof(buf),
					 "0x%" PRIx64, cv->val);
				if(fmtputs(f, buf, strlen(buf)))
					return;
				break;
			}else if(vq->qkind != Qvec)
			bady:
				vmerr(vm, "invalid response from lookaddr");
			vec = valvec(vq);
			if(vec->len < 3)
				goto bady;
			vq = vecref(vec, Idpos);
			if(vq->qkind != Qstr)
				goto bady;
			as = valstr(vq);
			vq = vecref(vec, Offpos);
			if(vq->qkind != Qcval)
				goto bady;
			if(fmtputs(f, as->s, as->len))
				return;
			cv = xcvalalu(vm, Isub, cv, valcval(vq));
			if(cv->val != 0){
				snprintf(buf, sizeof(buf),
					 "+0x%" PRIx64, cv->val);
				if(fmtputs(f, buf, strlen(buf)))
					return;
			}
			break;
		default:
			vmerr(vm, "unrecognized format conversion: %%%c", ch);
		}
		argc--;
		vpp++;
	}
	/* not reached */
badarg:
	vmerr(vm, "wrong type to %%%c conversion", ch);
}

static int
fmtfdflush(Fmt *f)
{
	Fd *fd;
	fd = (Fd*)f->farg;
	if(fd->flags&Fclosed)
		return -1;
	if((fd->flags&Fwrite) == 0)
		return -1;
	if(0 > xwrite(fd->fd, f->start, f->to-f->start))
		return -1;
	f->to = f->start;
	return 0;
}

static void
dofdprint(VM *vm, Fd *fd, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	Fmt f;
	char buf[256];

	f.farg = fd;
	f.start = buf;
	f.to = buf;
	f.stop = buf+sizeof(buf);
	f.flush = fmtfdflush;
	dofmt(vm, &f, fmt, fmtlen, argc, argv);
	fmtfdflush(&f);
}

static void
l1_printf(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *fmts;
	if(argc < 1)
		vmerr(vm, "wrong number of arguments to printf");
	if(argv[0]->qkind != Qstr)
		vmerr(vm, "operand 1 to printf must be a format string");
	fmts = valstr(argv[0]);
	dofdprint(vm, vm->stdout, fmts->s, fmts->len, argc-1, argv+1);
}

static void
l1_print(VM *vm, Imm argc, Val *argv, Val *rv)
{
	static char *fmt = "%a\n";
	dofdprint(vm, vm->stdout, fmt, strlen(fmt), argc, argv);
}

static void
l1_fprintf(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *fmts;
	if(argc < 2)
		vmerr(vm, "wrong number of arguments to fprintf");
	if(argv[0]->qkind != Qfd)
		vmerr(vm, "operand 1 to fprintf must be a file descriptor");
	if(argv[1]->qkind != Qstr)
		vmerr(vm, "operand 2 to fprintf must be a format string");
	fd = valfd(argv[0]);
	fmts = valstr(argv[1]);
	dofdprint(vm, fd, fmts->s, fmts->len, argc-2, argv+2);
}

static int
fmtstrflush(Fmt *f)
{
	u32 len;
	char *s;
	len = (u32)(uintptr_t)f->farg;
	s = f->start;
	f->start = xrealloc(f->start, len, len*2);
	len *= 2;
	f->farg = (void*)(uintptr_t)len;
	f->to = f->start+(f->to-s);
	f->stop = f->start+len;
	return 0;
}

static Str*
dovsprinta(VM *vm, char *fmt, Imm fmtlen, Imm argc, Val *argv)
{
	Fmt f;
	u32 initlen = 128;
	f.start = xmalloc(initlen);
	f.farg = (void*)(uintptr_t)initlen;
	f.to = f.start;
	f.stop = f.start+initlen;
	f.flush = fmtstrflush;
	dofmt(vm, &f, fmt, fmtlen, argc, argv);
	fmtstrflush(&f);
	return mkstrk(f.start, f.to-f.start, Smalloc);
}

static void
l1_sprintfa(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *fmts, *rs;
	if(argc < 1)
		vmerr(vm, "wrong number of arguments to sprintfa");
	if(argv[0]->qkind != Qstr)
		vmerr(vm, "operand 1 to sprintfa must be a format string");
	fmts = valstr(argv[0]);
	rs = dovsprinta(vm, fmts->s, fmts->len, argc-1, argv+1);
	*rv = mkvalstr(rs);
}

static void
l1_backtrace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to backtrace");
	vmbacktrace(vm);
}

static void
l1_tabkeys(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tabkeys");
	arg0 = argv[0];
	if(arg0->qkind != Qtab)
		vmerr(vm, "operand 1 to tabkeys must be a table");
	*rv = mkvalvec(tabenumkeys(valtab(arg0)));
}

static void
l1_tabvals(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tabvals");
	arg0 = argv[0];
	if(arg0->qkind != Qtab)
		vmerr(vm, "operand 1 to tabvals must be a table");
	*rv = mkvalvec(tabenumvals(valtab(arg0)));
}

static void
dotypepredicate(VM *vm, Imm argc, Val *argv, Val *rv, char *id, unsigned kind)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", id);
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to %s must be a ctype", id);
	xtn = valxtn(argv[0]);
	if(xtn->tkind == kind)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
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
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to issu must be a ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind == Tstruct || xtn->tkind == Tunion)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
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
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tbase)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	*rv = mkvalstr(mkstr0(basename[xtn->basename]));
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
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, err);
		      
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tptr && xtn->tkind != Tarr
	   && xtn->tkind != Tenum && xtn->tkind != Tconst
	   && xtn->tkind != Tundef)
		vmerr(vm, err);

	*rv = mkvalxtn(xtn->link);
}

static void
l1_rettype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to retttype");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to rettype must be a function ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tfun)
		vmerr(vm, "operand 1 to rettype must be a function ctype");
	*rv = mkvalxtn(xtn->link);
}

static void
l1_suekind(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suekind");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to suekind must be a tagged ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion
	   && xtn->tkind != Tenum)
		vmerr(vm, "operand 1 to suekind must be a tagged ctype");
	*rv = mkvalstr(mkstr0(tkindstr[xtn->tkind]));
}

static void
l1_suetag(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suetag");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to suetag must be a tagged ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion
	   && xtn->tkind != Tenum)
		vmerr(vm, "operand 1 to sutag must be a tagged ctype");
	*rv = mkvalstr(xtn->tag);
}

static void
l1_susize(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to susize");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm,
		      "operand 1 to susize must be a struct or union ctype");
	xtn = valxtn(argv[0]);
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
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to bitfieldwidth "
		      "must be a bitfield ctype");
		      
	xtn = valxtn(argv[0]);
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
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to bitfieldcontainer "
		      "must be a bitfield ctype");
		      
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldcontainer "
		      "must be a bitfield ctype");
	*rv = mkvalxtn(xtn->link);
}


static void
l1_bitfieldpos(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldpos");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to bitfieldpos must be a bitfield ctype");
		      
	xtn = valxtn(argv[0]);
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
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to arraynelm must be an array ctype");
		      
	xtn = valxtn(argv[0]);
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
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to typedefid must be a typedef ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Ttypedef)
		vmerr(vm, "operand 1 to typedefid must be a typedef ctype");
	*rv = mkvalstr(xtn->tid);
}

static void
l1_typedeftype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to typedeftype");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Ttypedef)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	*rv = mkvalxtn(xtn->link);
}

static void
l1_params(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to params");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to params must be a function ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tfun)
		vmerr(vm, "operand 1 to params must be a function ctype");
	*rv = mkvalvec(xtn->param);
}

static void
l1_paramtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to paramtype must be a vector returned by params";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to paramtype");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 2)
		vmerr(vm, err);
	vp = v->vec[Typepos];
	if(vp->qkind != Qxtn)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_paramid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to paramid must be a vector returned by params";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to paramid");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 2)
		vmerr(vm, err);
	vp = v->vec[Idpos];
	if(vp->qkind != Qstr && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_fields(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fields");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	*rv = mkvalvec(xtn->field);
}

static void
l1_lookfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	Val vp;
	Imm i;

	static char *err1
		= "operand 1 to lookfield must be a struct or union ctype";
	static char *err2
		= "operand 2 to lookfield must be a string";

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to lookfield");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, err1);
	xtn = valxtn(argv[0]);
	xtn = chasetype(xtn);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm, err1);
	if(argv[1]->qkind != Qstr)
		vmerr(vm, err2);
	for(i = 0; i < xtn->field->len; i++){
		vp = vecref(xtn->field, i);
		if(eqstrv(argv[1], vecref(valvec(vp), Idpos))){
			*rv = vp;
			return;
		}
	}
}

static void
l1_fieldtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to fieldtype must be a vector returned by fields";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fieldtype");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Typepos];
	if(vp->qkind != Qxtn)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_fieldid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to fieldid must be a vector returned by fields";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fieldid");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Idpos];
	if(vp->qkind != Qstr && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_fieldoff(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to fieldoff must be a vector returned by fields";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fieldoff");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Offpos];
	if(vp->qkind != Qcval && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_enumconsts(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	static char *err
		= "operand 1 to enumconsts must be a defined enum ctype";
	if(argc != 1)
		vmerr(vm, err);
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, err);
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tenum)
		vmerr(vm, err);
	if(xtn->konst == 0)
		vmerr(vm, err);
	*rv = mkvalvec(xtn->konst);
}

static void
l1_symtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to symtype must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symtype");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Typepos];
	if(vp->qkind != Qxtn)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_symid(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to symid must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symid");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Idpos];
	if(vp->qkind != Qcval && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_symval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to symval must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symval");
	if(argv[0]->qkind != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Offpos];
	if(vp->qkind != Qcval && vp->qkind != Qnil)
		vmerr(vm, err);
	*rv = vp;
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
	if(argv[0]->qkind == Qcval){
		cv = valcval(argv[0]);
		t = cv->type;
	}else if(argv[0]->qkind == Qxtn){
		t = valxtn(argv[0]);
		if(t->tkind == Tbitfield || t->tkind == Tconst)
			t = t->link;
	}else
		vmerr(vm, "operand 1 to $typeof must be a cvalue or type");
	*rv = mkvalxtn(t);
}

static void
l1_mkctype_void(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_void");
	*rv = mkvalxtn(mkvoidxtn());
}

static void
domkctype_base(Cbase name, Val *rv)
{
	Xtypename *xtn;
	xtn = mkbasextn(name, Rundef);
	*rv = mkvalxtn(xtn);
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
	Xtypename *xtn, *pxtn;
	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkctype_ptr");
	if(argv[0]->qkind != Qxtn)
		vmerr(vm, "operand 1 to mkctype_ptr must be a pointer ctype");
	xtn = valxtn(argv[0]);
	if(argc == 1)
		xtn = mkptrxtn(xtn, Rundef);
	else{
		if(argv[1]->qkind != Qxtn)
			vmerr(vm, "operand 2 to mkctype_ptr "
			      "must define a pointer type");
		checkarg(vm, "mkctype_ptr", argv, 1, Qxtn);
		pxtn = valxtn(argv[1]);
		pxtn = chasetype(pxtn);
		if((pxtn->tkind != Tptr && pxtn->tkind != Tbase)
		   || pxtn->rep == Rundef)
			vmerr(vm, "operand 2 to mkctype_ptr "
			      "must define a pointer type");
		xtn = mkptrxtn(xtn, pxtn->rep);
	}
	*rv = mkvalxtn(xtn);
}

static void
l1_mkctype_typedef(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *sub;
	Str *s;

	switch(argc){
	case 1:
		checkarg(vm, "mkctype_typedef", argv, 0, Qstr);
		s = valstr(argv[0]);
		xtn = mkxtn();
		xtn->tkind = Ttypedef;
		xtn->tid = s;
		break;
	case 2:
		checkarg(vm, "mkctype_typedef", argv, 0, Qstr);
		checkarg(vm, "mkctype_typedef", argv, 1, Qxtn);
		s = valstr(argv[0]);
		sub = valxtn(argv[1]);
		xtn = mkxtn();
		xtn->tkind = Ttypedef;
		xtn->tid = s;
		xtn->link = sub;
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_typedef");
	}
	*rv = mkvalxtn(xtn);
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
		s = valstr(argv[0]);
		xtn = mkxtn();
		xtn->tkind = tkind;
		xtn->tag = s;
		break;
	case 3:
		/* TAG FIELDS SIZE */
		checkarg(vm, fn, argv, 0, Qstr);
		checkarg(vm, fn, argv, 1, Qvec);
		checkarg(vm, fn, argv, 2, Qcval);
		s = valstr(argv[0]);
		f = valvec(argv[1]);
		xtn = mkxtn();
		xtn->tkind = tkind;
		xtn->tag = s;
		xtn->field = f;
		xtn->sz = argv[2];
		break;
	default:
		vmerr(vm, "wrong number of arguments to %s", fn);
	}
	*rv = mkvalxtn(xtn);
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
		sub = valxtn(argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tarr;
		xtn->link = sub;
		break;
	case 2:
		/* TYPE CNT */
		checkarg(vm, "mkctype_array", argv, 0, Qxtn);
		checkarg(vm, "mkctype_array", argv, 1, Qcval);
		sub = valxtn(argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tarr;
		xtn->link = sub;
		xtn->cnt = argv[1];
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_array");
	}
	*rv = mkvalxtn(xtn);
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
	sub = valxtn(argv[0]);
	p = valvec(argv[1]);
	xtn = mkxtn();
	xtn->tkind = Tfun;
	xtn->link = sub;
	xtn->param = p;
	*rv = mkvalxtn(xtn);
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
	sub = valxtn(argv[0]);
	xtn = mkxtn();
	xtn->tkind = Tbitfield;
	xtn->link = sub;
	xtn->sz = argv[1];
	xtn->bit0 = argv[2];
	*rv = mkvalxtn(xtn);
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
		s = valstr(argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tenum;
		xtn->tag = s;
		break;
	case 2:
		/* TAG CONSTS */
		checkarg(vm, "mkctype_enum", argv, 0, Qstr);
		checkarg(vm, "mkctype_enum", argv, 1, Qvec);
		s = valstr(argv[0]);
		c = valvec(argv[1]);
		xtn = mkxtn();
		xtn->tkind = Tenum;
		xtn->tag = s;
		xtn->konst = c;
		break;
	case 3:
		/* TAG CONSTS TYPE */
		checkarg(vm, "mkctype_enum", argv, 0, Qstr);
		checkarg(vm, "mkctype_enum", argv, 1, Qvec);
		checkarg(vm, "mkctype_enum", argv, 2, Qxtn);
		s = valstr(argv[0]);
		c = valvec(argv[1]);
		t = valxtn(argv[2]);
		xtn = mkxtn();
		xtn->tkind = Tenum;
		xtn->tag = s;
		xtn->link = t;
		xtn->konst = c;
		break;
	default:
		vmerr(vm, "wrong number of arguments to mkctype_enum");
	}
	*rv = mkvalxtn(xtn);
}

static void
l1_mkctype_const(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkctype_const");
	checkarg(vm, "mkctype_const", argv, 0, Qxtn);
	t = valxtn(argv[0]);
	xtn = mkxtn();
	xtn->tkind = Tconst;
	xtn->link = t;
	*rv = mkvalxtn(xtn);
}

static void
mksymorfieldorparam(char *what, VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *vec;

	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to %s", what);
	checkarg(vm, what, argv, 0, Qxtn);
	checkarg(vm, what, argv, 1, Qstr);
	if(argc == 3)
		if(argv[2]->qkind != Qcval && argv[2]->qkind != Qnil)
			vmerr(vm, "operand 3 to %s must be a cvalue or nil",
			      what);
	vec = mkvec(3);
	_vecset(vec, 0, argv[0]);
	_vecset(vec, 1, argv[1]);
	if(argc == 3)
		_vecset(vec, 2, argv[2]);
	else
		_vecset(vec, 2, Xnil);
	*rv = mkvalvec(vec);
}

static void
l1_mksym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	mksymorfieldorparam("mksym", vm, argc, argv, rv);
}

static void
l1_mkfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	mksymorfieldorparam("mkfield", vm, argc, argv, rv);	
}

static void
l1_mkparam(VM *vm, Imm argc, Val *argv, Val *rv)
{
	mksymorfieldorparam("mkparam", vm, argc, argv, rv);	
}

static void
l1_isnil(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isnil");
	if(argv[0]->qkind == Qnil)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_error(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	l1_sprintfa(vm, argc, argv, rv);
	s = valstr(*rv);
	vmerr(vm, "%.*s", (int)s->len, s->s);
}

static void
l1_fault(VM *vm, Imm argc, Val *argv, Val *rv)
{
	vmerr(vm, "memory access fault");
}

static void
l1_strput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s, *t;
	Cval *off;
	Imm o;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to strput");
	if(argv[0]->qkind != Qstr)
		vmerr(vm, "operand 1 to strput must be a string");
	if(argv[1]->qkind != Qcval)
		vmerr(vm, "operand 2 to strput must be an offset");
	if(argv[2]->qkind != Qstr)
		vmerr(vm, "operand 3 to strput must be a string");
	s = valstr(argv[0]);
	off = valcval(argv[1]);
	t = valstr(argv[2]);
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
	Val argv[4], p;
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
	d = valdom(iargv[0]);
	addr = valcval(iargv[1]);
	t = valxtn(iargv[2]);
	cv = valcval(iargv[3]);

	b = chasetype(t);
	switch(b->tkind){
	case Tbase:
	case Tptr:
		cv = typecast(vm, t, cv);
		gcprotect(vm, cv);
		bytes = imm2str(vm, t, cv->val);
		len = mkcval(vm->litdom, vm->litbase[Vptr], typesize(vm, t));
		argv[0] = mkvalstr(vm->sput);
		argv[1] = mkvaldom(d);
		argv[2] = mkvalrange(addr, len);
		argv[3] = mkvalstr(bytes);
		dovm(vm, d->as->dispatch, 4, argv);
		*rv = mkvalcval2(cv);
		break;
	case Tbitfield:
		if(b->link->tkind == Tundef){
			es = fmtxtn(b->link->link);
			vmerr(vm, "attempt to write object of undefined type: "
			      "%.*s", (int)es->len, es->s);
		}
		if(0 > dobitfieldgeom(addr, b, &bfg))
			vmerr(vm, "invalid bitfield access");

		/* get contents of bitfield container */
		argv[0] = mkvalstr(vm->sget);
		argv[1] = mkvaldom(d);
		/* FIXME: why litbase[Vptr]; why not d->ns->base[Vptr] ? */
		r = mkrange(mkcval(vm->litdom, vm->litbase[Vptr], bfg.addr),
			    mkcval(vm->litdom, vm->litbase[Vptr], bfg.cnt));
		gcprotect(vm, r);
		argv[2] = mkvalrange2(r);
		p = dovm(vm, d->as->dispatch, 3, argv);
		bytes = valstr(p);

		/* update bitfield container */
		imm = bitfieldput(bytes->s, &bfg, cv->val);

		/* put updated bitfield container */
		argv[0] = mkvalstr(vm->sput);
			/* reuse argv[1] dom */
			/* reuse argv[2] range */
		argv[3] = mkvalstr(bytes);
		dovm(vm, d->as->dispatch, 4, argv);

		/* return value of bitfield (not container) */
		*rv = mkvalcval(d, b->link, imm);
		break;
	case Tconst:
		vmerr(vm, "attempt to use enumeration constant as location");
		break;
	case Tundef:
		es = fmtxtn(b->link);
		vmerr(vm,
		      "attempt to write object of undefined type: %.*s",
		      (int)es->len, es->s);
	case Tenum:
	case Ttypedef:
	case Tvoid:
	case Tstruct:
	case Tunion:
	case Tfun:
	case Tarr:
		vmerr(vm,
		      "attempt to write %s-valued object to address space",
		      tkindstr[b->tkind]);
	}
}

static void
l1_foreach(VM *vm, Imm argc, Val *iargv, Val *rv)
{
	List *l;
	Vec *k, *v;
	Tab *t;
	Closure *cl;
	Imm m, len;
	Val argv[2];

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to foreach");
	checkarg(vm, "foreach", iargv, 0, Qcl);
	if(iargv[1]->qkind != Qlist
	   && iargv[1]->qkind != Qvec
	   && iargv[1]->qkind != Qtab)
		vmerr(vm,
		      "operand 1 to foreach must be a list, table, or vector");
	cl = valcl(iargv[0]);
	switch(iargv[1]->qkind){
	case Qlist:
		l = vallist(iargv[1]);
		len = listxlen(l->x);
		for(m = 0; m < len; m++){
			argv[0] = listref(vm, l, m);
			dovm(vm, cl, 1, argv);
		}
		break;
	case Qtab:
		t = valtab(iargv[1]);
		k = tabenumkeys(t);
		v = tabenumvals(t);
		gcprotect(vm, k);
		gcprotect(vm, v);
		for(m = 0; m < v->len; m++){
			argv[0] = vecref(k, m);
			argv[1] = vecref(v, m);
			dovm(vm, cl, 2, argv);
		}
		break;
	case Qvec:
		v = valvec(iargv[1]);
		for(m = 0; m < v->len; m++){
			argv[0] = vecref(v, m);
			dovm(vm, cl, 1, argv);
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
	names = valstr(argv[0]);
	name = str2cstr(names);
	fd = open(name, O_RDONLY);
	free(name);
	if(0 > fd)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	if(0 > fstat(fd, &st)){
		close(fd);
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	}
	p = mmap(0, st.st_size, PROT_READ|PROT_WRITE,
		 MAP_NORESERVE|MAP_PRIVATE, fd, 0);
	close(fd);
	if(p == MAP_FAILED)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	map = mkstrk(p, st.st_size, Smmap);
	*rv = mkvalstr(map);
}

static void
l1_open(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	int xfd;
	Str *names;
	char *name, *mode;
	int oflags, flags;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to open");
	checkarg(vm, "open", argv, 0, Qstr);
	checkarg(vm, "open", argv, 1, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	mode = str2cstr(valstr(argv[1]));

	flags = 0;
	oflags = 0;
	if(strchr(mode, 'r'))
		flags |= Fread;
	if(strchr(mode, 'w')){
		flags |= Fwrite;
		oflags |= O_CREAT;
	}
	if((flags&Fwrite) && !strchr(mode, 'a'))
		oflags |= O_TRUNC;

	if((flags&Fread) && (flags&Fwrite))
		oflags |= O_RDWR;
	else if(flags&Fread)
		oflags |= O_RDONLY;
	else if(flags&Fwrite)
		oflags |= O_WRONLY;

	xfd = open(name, oflags, 0777); /* ~umask */
	free(name);
	free(mode);
	if(0 > xfd)
		vmerr(vm, "cannot open %.*s: %s", (int)names->len, names->s,
		      strerror(errno));
	fd = mkfd(names, xfd, flags, freefdclose);
	*rv = mkvalfd(fd);
}

static void
l1_close(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to close");
	checkarg(vm, "close", argv, 0, Qfd);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		return;
	fd->flags |= Fclosed;
	close(fd->fd);
}

static void
l1_fdname(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to close");
	checkarg(vm, "close", argv, 0, Qfd);
	fd = valfd(argv[0]);
	*rv = mkvalstr(fd->name);
}

static void
l1_read(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *s;
	char *buf;
	Cval *n;
	Imm r;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to read");
	checkarg(vm, "read", argv, 0, Qfd);
	checkarg(vm, "read", argv, 1, Qcval);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to read from closed file descriptor");
	n = valcval(argv[1]);
	buf = xmalloc(n->val);	/* FIXME: check sign, <= SSIZE_MAX */
	r = xread(fd->fd, buf, n->val);
	if(r == (Imm)-1)
		vmerr(vm, "read error: %s", strerror(errno));
	if(n->val > 0 && r == 0)
		return;		/* nil */
	s = mkstrk(buf, r, Smalloc);
	*rv = mkvalstr(s);
}

static void
l1_write(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *s;
	int r;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to write");
	checkarg(vm, "write", argv, 0, Qfd);
	checkarg(vm, "write", argv, 1, Qstr);
	fd = valfd(argv[0]);
	if(fd->flags&Fclosed)
		vmerr(vm, "attempt to write to closed file descriptor");
	s = valstr(argv[1]);
	r = xwrite(fd->fd, s->s, s->len);
	if(r == -1)
		vmerr(vm, "write error: %s", strerror(errno));
	/* return nil */
}

static void
l1_mkdir(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char *name;
	int r;
	
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkdir");
	checkarg(vm, "mkdir", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	r = mkdir(name, 0777);	/* ~umask */
	free(name);
	if(0 > r)
		vmerr(vm, "mkdir: %s", strerror(errno));
	/* return nil */
}
       
static void
l1_unlink(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char *name;
	int r;
	
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to unlink");
	checkarg(vm, "unlink", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	r = unlink(name);
	free(name);
	if(0 > r)
		vmerr(vm, "unlink: %s", strerror(errno));
	/* return nil */
}

static void
l1_rmdir(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *names;
	char *name;
	int r;
	
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rmdir");
	checkarg(vm, "rmdir", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	r = rmdir(name);
	free(name);
	if(0 > r)
		vmerr(vm, "rmdir: %s", strerror(errno));
	/* return nil */
}

static void
l1__readdir(VM *vm, Imm argc, Val *argv, Val *rv)
{
	DIR *dir;
	Str *names;
	char *name;
	u32 ndir, lim;
	char *buf;
	struct dirent *p, *d;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to readdir");
	checkarg(vm, "readdir", argv, 0, Qstr);
	names = valstr(argv[0]);
	name = str2cstr(names);
	dir = opendir(name);
	free(name);
	if(dir == NULL)
		vmerr(vm, "opendir: %s", strerror(errno));

	lim = 128;
	buf = xmalloc(lim*sizeof(struct dirent));
	ndir = 0;
	p = (struct dirent*)buf;
	while((d = readdir(dir))){
		if(ndir >= lim){
			buf = xrealloc(buf, lim*sizeof(struct dirent),
				       2*lim*sizeof(struct dirent));
			lim *= 2;
			p = (struct dirent*)buf+ndir;
		}
		memcpy(p, d, d->d_reclen);
		p++;
		ndir++;
	}
	closedir(dir);
	*rv = mkvalas(mksas(mkstrk(buf, ndir*sizeof(struct dirent), Smalloc)));
}

static void
l1_opentcp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Str *str;
	char *s;
	int xfd;
	struct sockaddr_in saddr;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to opentcp");
	checkarg(vm, "opentcp", argv, 0, Qstr);
	str = valstr(argv[0]);
	s = str2cstr(str);
	if(0 > parseip(s, &saddr))
		vmerr(vm, "unrecognized address: %.*s", (int)str->len, str->s);
	free(s);
	xfd = socket(AF_INET, SOCK_STREAM, 0);
	if(0 > xfd)
		vmerr(vm, "opentcp: %s", strerror(errno));
	if(0 > connect(xfd, (struct sockaddr*)&saddr, sizeof(saddr)))
		vmerr(vm, "opentcp: %s", strerror(errno));
	nodelay(xfd);
	fd = mkfd(str, xfd, Fread|Fwrite, freefdclose);
	*rv = mkvalfd(fd);
}

static void
l1_mknas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mknas");
	as = mknas();
	*rv = mkvalas(as);
}

static void
l1_mksas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Str *s;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mksas");
	checkarg(vm, "mksas", argv, 0, Qstr);
	s = valstr(argv[0]);
	as = mksas(s);
	*rv = mkvalas(as);
}

static void
l1_mkzas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Cval *cv;
	Xtypename *t;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkzas");
	checkarg(vm, "mkzas", argv, 0, Qcval);
	cv = valcval(argv[0]);
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		vmerr(vm, "operand 1 to mkzas must be an integer cvalue");
	as = mkzas(vm, cv->val);
	*rv = mkvalas(as);
}

static void
l1_mkas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Closure *cl;
	Str *name;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkas");
	checkarg(vm, "mkas", argv, 0, Qcl);
	name = 0;
	if(argc == 2){
		checkarg(vm, "mkas", argv, 1, Qstr);
		name = valstr(argv[1]);
	}
	cl = valcl(argv[0]);
	as = mkas(cl, name);
	*rv = mkvalas(as);
}

static void
l1_mknsraw(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ns *ons, *ns;
	Tab *rawtype, *rawsym;
	Str *name;

	if(argc != 3 && argc != 4)
		vmerr(vm, "wrong number of arguments to mknsraw");
	checkarg(vm, "mkns", argv, 0, Qns);
	checkarg(vm, "mkns", argv, 1, Qtab);
	checkarg(vm, "mkns", argv, 2, Qtab);
	name = 0;
	if(argc == 4){
		checkarg(vm, "mknsraw", argv, 3, Qstr);
		name = valstr(argv[3]);
	}
	ons = valns(argv[0]);
	rawtype = valtab(argv[1]);
	rawsym = valtab(argv[2]);
	ns = mknstab(vm, ons, rawtype, rawsym, name);
	*rv = mkvalns(ns);
}

static void
l1_mkns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ns *ns;
	Str *name;

	if(argc != 5 && argc != 6)
		vmerr(vm, "wrong number of arguments to mkns");
	checkarg(vm, "mkns", argv, 0, Qcl);
	checkarg(vm, "mkns", argv, 1, Qcl);
	checkarg(vm, "mkns", argv, 2, Qcl);
	checkarg(vm, "mkns", argv, 3, Qcl);
	checkarg(vm, "mkns", argv, 4, Qcl);
	name = 0;
	if(argc == 6){
		checkarg(vm, "mknsraw", argv, 5, Qstr);
		name = valstr(argv[5]);
	}
	ns = mknsfn(valcl(argv[0]),
		    valcl(argv[1]),
		    valcl(argv[2]),
		    valcl(argv[3]),
		    valcl(argv[4]),
		    name);
	nscachebase(vm, ns);
	*rv = mkvalns(ns);
}

static void
l1_mkdom(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Ns *ns;
	Dom *dom;
	Str *name;

	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to mkdom");
	checkarg(vm, "mkdom", argv, 0, Qns);
	checkarg(vm, "mkdom", argv, 1, Qas);
	name = 0;
	if(argc == 3){
		checkarg(vm, "mkdom", argv, 2, Qstr);
		name = valstr(argv[2]);
	}
	ns = valns(argv[0]);
	as = valas(argv[1]);
	dom = mkdom(ns, as, name);
	*rv = mkvaldom(dom);
}

static void
l1_asof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to asof");
	if(argv[0]->qkind == Qdom)
		dom = valdom(argv[0]);
	else if(argv[0]->qkind == Qcval){
		cv = valcval(argv[0]);
		dom = cv->dom;
	}else
		vmerr(vm, "operand 1 to asof must be a domain or cvalue");
	*rv = mkvalas(dom->as);
}

static void
l1_nsof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsof");
	if(argv[0]->qkind == Qdom)
		dom = valdom(argv[0]);
	else if(argv[0]->qkind == Qcval){
		cv = valcval(argv[0]);
		dom = cv->dom;
	}else
		vmerr(vm, "operand 1 to nsof must be a domain or cvalue");
	*rv = mkvalns(dom->ns);
}

static void
l1_asdispatch(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	As *as;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to asdispatch");
	arg0 = argv[0];
	if(arg0->qkind != Qas && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nslooksym must be an addrspace or domain");
	if(arg0->qkind == Qas)
		as = valas(arg0);
	else{
		dom = valdom(arg0);
		as = dom->as;
	}
	*rv = mkvalcl(as->dispatch);
}

static void
l1_nslookaddr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nslookaddr");
	arg0 = argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nslookaddr must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	*rv = mkvalcl(ns->lookaddr);
}

static void
l1_nslooksym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nslooksym");
	arg0 = argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nslooksym must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	*rv = mkvalcl(ns->looksym);
}

static void
l1_nslooktype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nslooktype");
	arg0 = argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nslooktype must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	*rv = mkvalcl(ns->looktype);
}

static void
l1_nsenumsym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsenumsym");
	arg0 = argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nsenumsym must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	*rv = mkvalcl(ns->enumsym);
}

static void
l1_nsenumtype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsenumtype");
	arg0 = argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nsenumtype must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	*rv = mkvalcl(ns->enumtype);
}

static void
l1_nsptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Dom *dom;
	Ns *ns;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsptr");
	arg0 = argv[0];
	if(arg0->qkind != Qns && arg0->qkind != Qdom)
		vmerr(vm,
		      "operand 1 to nsptr must be a namespace or domain");
	if(arg0->qkind == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	*rv = mkvalxtn(ns->base[Vptr]);
}

static void
l1_mkrange(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to mkrange");
	checkarg(vm, "mkrange", argv, 0, Qcval);
	checkarg(vm, "mkrange", argv, 1, Qcval);
	/* FIXME: check sanity */
	*rv = mkvalrange(valcval(argv[0]), valcval(argv[1]));
}

static void
l1_rangebeg(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Range *r;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rangebeg");
	checkarg(vm, "rangebeg", argv, 0, Qrange);
	r = valrange(argv[0]);
	*rv = mkvalcval2(r->beg);
}

static void
l1_rangelen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Range *r;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rangelen");
	checkarg(vm, "rangelen", argv, 0, Qrange);
	r = valrange(argv[0]);
	*rv = mkvalcval2(r->len);
}

static void
l1_mkstr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkstr");
	checkarg(vm, "mkstr", argv, 0, Qcval);
	cv = valcval(argv[0]);
	/* FIXME: sanity */
	*rv = mkvalstr(mkstrn(vm, cv->val));
}

static void
l1_strlen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to strlen");
	checkarg(vm, "strlen", argv, 0, Qstr);
	s = valstr(argv[0]);
	*rv = mkvalcval(vm->litdom, vm->litbase[Vuvlong], s->len);
}

static void
l1_substr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *b, *e;
	Str *s;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to substr");
	checkarg(vm, "substr", argv, 0, Qstr);
	checkarg(vm, "substr", argv, 1, Qcval);
	checkarg(vm, "substr", argv, 2, Qcval);
	s = valstr(argv[0]);
	b = valcval(argv[1]);
	e = valcval(argv[2]);
	if(b->val > s->len)
		vmerr(vm, "substring out of bounds");
	if(e->val > s->len)
		vmerr(vm, "substring out of bounds");
	if(b->val > e->val)
		vmerr(vm, "substring out of bounds");
	*rv = mkvalstr(strslice(s, b->val, e->val));
}

static void
l1_stringof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Cval *cv;
	static char *err =
		"operand 1 to stringof must be a "
		"char* or unsigned char* cvalue";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to stringof");
	checkarg(vm, "stringof", argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(!isstrcval(cv))
		vmerr(vm, err);
	s = stringof(vm, cv);
	*rv = mkvalstr(s);
}

static void
l1_strton(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Liti liti;
	char *err;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to strton");
	checkarg(vm, "strton", argv, 0, Qstr);
	s = valstr(argv[0]);
	if(0 != parseliti(s->s, s->len, &liti, &err))
		vmerr(vm, err);
	*rv = mkvalcval(vm->litdom, vm->litbase[liti.base], liti.val);
}

static void
l1_mkvec(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkvec");
	checkarg(vm, "mkvec", argv, 0, Qcval);
	cv = valcval(argv[0]);
	/* FIXME: sanity */
	*rv = mkvalvec(mkvecnil(cv->val));
}

static void
l1_vector(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Imm i;

	v = mkvec(argc);
	for(i = 0; i < argc; i++)
		_vecset(v, i, argv[i]);
	*rv = mkvalvec(v);
}

static void
l1_veclen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to veclen");
	checkarg(vm, "veclen", argv, 0, Qvec);
	v = valvec(argv[0]);
	*rv = mkvalcval(vm->litdom, vm->litbase[Vuvlong], v->len);
}

static void
l1_vecref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Cval *cv;
	Val vp;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to vecref");
	checkarg(vm, "vecref", argv, 0, Qvec);
	checkarg(vm, "vecref", argv, 1, Qcval);
	v = valvec(argv[0]);
	cv = valcval(argv[1]);
	/* FIXME: check sign of cv */
	if(cv->val >= v->len)
		vmerr(vm, "vecref out of bounds");
	vp = vecref(v, cv->val);
	*rv = vp;
}

static void
l1_vecset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Cval *cv;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to vecset");
	checkarg(vm, "vecset", argv, 0, Qvec);
	checkarg(vm, "vecset", argv, 1, Qcval);
	v = valvec(argv[0]);
	cv = valcval(argv[1]);
	/* FIXME: check sign of cv */
	if(cv->val >= v->len)
		vmerr(vm, "vecset out of bounds");
	vecset(vm, v, cv->val, argv[2]);
}

static void
l1_mktab(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mktab");
	*rv = mkvaltab(mktab());
}

static void
l1_tabdelete(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Tab *t;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to tabdelete");
	checkarg(vm, "tabdelete", argv, 0, Qtab);
	t = valtab(argv[0]);
	tabdel(vm, t, argv[1]);
}

static void
l1_tablook(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Tab *t;
	Val vp;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to tablook");
	checkarg(vm, "tablook", argv, 0, Qtab);
	t = valtab(argv[0]);
	vp = tabget(t, argv[1]);
	if(vp)
		*rv = vp;
	/* otherwise return nil */
}

static void
l1_tabinsert(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Tab *t;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to tabinsert");
	checkarg(vm, "tabinsert", argv, 0, Qtab);
	t = valtab(argv[0]);
	tabput(vm, t, argv[1], argv[2]);
}

static void
l1_tabenum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Tab *t;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tabenum");
	checkarg(vm, "tabenum", argv, 0, Qtab);
	t = valtab(argv[0]);
	*rv = mkvalvec(tabenum(t));
}

static void
l1_ismapped(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *addr, *len;
	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to ismapped");
	checkarg(vm, "ismapped", argv, 0, Qcval);
	addr = valcval(argv[0]);
	len = 0;
	if(argc == 2){
		checkarg(vm, "ismapped", argv, 1, Qcval);
		len = valcval(argv[1]);
	}
	if(ismapped(vm, addr, len))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_getbytes(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Cval *addr, *len;
	Imm n;
	Xtypename *t;
	Val argv[3], p;
	Range *r;

	if(iargc != 1 && iargc != 2)
		vmerr(vm, "wrong number of arguments to getbytes");

	checkarg(vm, "getbytes", iargv, 0, Qcval);
	addr = valcval(iargv[0]);
	t = chasetype(addr->type);
	if(t->tkind != Tptr)
		vmerr(vm, "operand 1 to getbytes must be a pointer");

	if(iargc == 2){
		checkarg(vm, "getbytes", iargv, 1, Qcval);
		len = valcval(iargv[1]);
		n = len->val;
	}else
		n = typesize(vm, t->link);

	argv[0] = mkvalstr(vm->sget);
	r = mkrange(mkcval(addr->dom, addr->dom->ns->base[Vptr], addr->val),
		    mkcval(addr->dom, addr->dom->ns->base[Vptr], n));
	argv[1] = mkvaldom(addr->dom);
	argv[2] = mkvalrange2(r);
	p = dovm(vm, addr->dom->as->dispatch, 3, argv);
	if(p->qkind != Qstr)
		vmerr(vm, "address space get operation did not return string");
	*rv = p;
}

static void
l1_putbytes(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Cval *addr;
	Str *str;
	Xtypename *t;
	Range *r;
	Val argv[4];

	if(iargc != 2)
		vmerr(vm, "wrong number of arguments to putbytes");
	checkarg(vm, "putbytes", iargv, 0, Qcval);
	checkarg(vm, "putbytes", iargv, 1, Qstr);
	addr = valcval(iargv[0]);
	str = valstr(iargv[1]);
	t = chasetype(addr->type);
	if(t->tkind != Tptr)
		vmerr(vm, "operand 1 to putbytes must be a pointer");
	argv[0] = mkvalstr(vm->sput);
	argv[1] = mkvaldom(addr->dom);
	r = mkrange(mkcval(addr->dom, addr->dom->ns->base[Vptr], addr->val),
		    mkcval(addr->dom, addr->dom->ns->base[Vptr], str->len));
	argv[2] = mkvalrange2(r);
	argv[3] = mkvalstr(str);
	dovm(vm, addr->dom->as->dispatch, 4, argv);
}

static void
l1_apply(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Imm ll, argc, m;
	Val *argv, *ap, *ip, vp;
	Closure *cl;
	List *lst;

	if(iargc < 2)
		vmerr(vm, "wrong number of arguments to apply");
	checkarg(vm, "apply", iargv, 0, Qcl);
	cl = valcl(iargv[0]);
	if(listlen(iargv[iargc-1], &ll) == 0)
		vmerr(vm, "final operand to apply must be a proper list");
	argc = iargc-2+ll;
	argv = xmalloc(argc*sizeof(Val));
	ap = argv;
	ip = &iargv[1];
	for(m = 0; m < iargc-2; m++)
		*ap++ = *ip++;
	lst = vallist(*ip);
	for(m = 0; m < ll; m++){
		vp = listref(vm, lst, m);
		*ap++ = vp;
	}
	vp = dovm(vm, cl, argc, argv);
	*rv = vp;
	free(argv);
}

static void
l1_isempty(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isempty");
	if(argv[0]->qkind == Qlist){
		lst = vallist(argv[0]);
		if(listxlen(lst->x) != 0)
			*rv = mkvalcval2(cval0);
		else
			*rv = mkvalcval2(cval1);
	}else
		vmerr(vm, "isempty defined only for lists");
}

static void
l1_length(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Vec *vec;
	Str *str;
	Imm len;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to length");
	switch(argv[0]->qkind){
	default:
		vmerr(vm, "operand 1 to length must be something lengthy");
	case Qlist:
		lst = vallist(argv[0]);
		len = listxlen(lst->x);
		break;
	case Qstr:
		str = valstr(argv[0]);
		len = str->len;
		break;
	case Qvec:
		vec = valvec(argv[0]);
		len = vec->len;
		break;
	}
	*rv = mkvalcval(vm->litdom, vm->litbase[Vuvlong], len);
}

static void
l1_null(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to null");
	*rv = Xnulllist;
}

static void
l1_car(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Pair *p;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to car");
	checkarg(vm, "car", argv, 0, Qpair);
	p = valpair(argv[0]);
	*rv = p->car;
}

static void
l1_cdr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Pair *p;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cdr");
	checkarg(vm, "cdr", argv, 0, Qpair);
	p = valpair(argv[0]);
	*rv = p->cdr;
}

static void
l1_cons(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cons");
	*rv = mkvalpair(argv[0], argv[1]);
}

static void
l1_list(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Imm i;

	l = mklist();
	for(i = 0; i < argc; i++)
		listappend(vm, l, argv[i]);
	*rv = mkvallist(l);
}

static void
l1_listref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val vp;
	List *lst;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to listref");
	checkarg(vm, "listref", argv, 0, Qlist);
	checkarg(vm, "listref", argv, 1, Qcval);
	cv = valcval(argv[1]);	/* FIXME check sign */
	lst = vallist(argv[0]);
	vp = listref(vm, lst, cv->val);
	*rv = vp;
}

static void
l1_listdel(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to listdel");
	checkarg(vm, "listdel", argv, 0, Qlist);
	checkarg(vm, "listdel", argv, 1, Qcval);
	cv = valcval(argv[1]);	/* FIXME check sign */
	lst = listdel(vm, vallist(argv[0]), cv->val);
	*rv = mkvallist(lst);
}

static void
l1_listset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to listset");
	checkarg(vm, "listset", argv, 0, Qlist);
	checkarg(vm, "listset", argv, 1, Qcval);
	cv = valcval(argv[1]);	/* FIXME check sign */
	lst = vallist(argv[0]);
	lst = listset(vm, lst, cv->val, argv[2]);
	*rv = mkvallist(lst);
}

static void
l1_listins(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to listins");
	checkarg(vm, "listins", argv, 0, Qlist);
	checkarg(vm, "listins", argv, 1, Qcval);
	cv = valcval(argv[1]);	/* FIXME check sign */
	lst = listins(vm, vallist(argv[0]), cv->val, argv[2]);
	*rv = mkvallist(lst);
}

static void
l1_pop(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to listpop");
	checkarg(vm, "listpop", argv, 0, Qlist);
	listpop(vm, vallist(argv[0]), rv);
}

static void
l1_head(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val vp;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to head");
	checkarg(vm, "head", argv, 0, Qlist);
	vp = listhead(vm, vallist(argv[0]));
	*rv = vp;
}

static void
l1_tail(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tail");
	checkarg(vm, "tail", argv, 0, Qlist);
	lst = listtail(vm, vallist(argv[0]));
	*rv = mkvallist(lst);
}

static void
l1_push(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to push");
	checkarg(vm, "push", argv, 0, Qlist);
	lst = listpush(vm, vallist(argv[0]), argv[1]);
	*rv = mkvallist(lst);
}

static void
l1_append(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to append");
	checkarg(vm, "append", argv, 0, Qlist);
	lst = listappend(vm, vallist(argv[0]), argv[1]);
	*rv = mkvallist(lst);
}

static void
l1_reverse(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to reverse");
	if(argv[0]->qkind == Qlist)
		*rv = mkvallist(listreverse(vallist(argv[0])));
	else
		vmerr(vm, "operand 1 to reverse must be a list");
}

static void
l1_copy(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to append");
	if(argv[0]->qkind == Qlist)
		*rv = mkvallist(listcopy(vallist(argv[0])));
	else if(argv[1]->qkind == Qvec)
		*rv = mkvalvec(veccopy(valvec(argv[0])));
	else
		vmerr(vm, "operand 1 to copy must be a list or vector");
}

static void
l1_equal(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Qkind kind;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to equal");
	kind = argv[0]->qkind;
	if(kind != argv[1]->qkind)
		*rv = mkvalcval2(cval0);
	else if(kind == Qlist){
		if(equallistv(argv[0], argv[1]))
			*rv = mkvalcval2(cval1);
		else
			*rv = mkvalcval2(cval0);
	}else
		vmerr(vm, "equal defined only for lists");
}

static void
l1_isx(VM *vm, Imm argc, Val *argv, Val *rv, char *name, Qkind kind)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", name);
	if(argv[0]->qkind == kind)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isas", Qas);
}

static void
l1_isctype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isctype", Qxtn);
}

static void
l1_iscvalue(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "iscval", Qcval);
}

static void
l1_isdomain(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isdomain", Qdom);
}

static void
l1_isfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isfd", Qfd);
}

static void
l1_islist(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "islist", Qlist);
}

static void
l1_isns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isns", Qns);
}

static void
l1_isnull(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isnull", Qnulllist);
}

static void
l1_ispair(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "ispair", Qpair);
}

static void
l1_isprocedure(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isprocedure", Qcl);
}

static void
l1_isrange(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isrange", Qrange);
}

static void
l1_isstring(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isstring", Qstr);
}

static void
l1_istable(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "istable", Qtab);
}

static void
l1_isvector(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isvector", Qvec);
}

static void
l1_gc(VM *vm, Imm argc, Val *argv, Val *rv)
{
}

static void
printval(VM *vm, Val val)
{
	Val argv[2], rv;
	Str *s;
	s = mkstrk("%a", 2, Sperm);
	argv[0] = mkvalstr(s);
	argv[1] = val;
	l1_printf(vm, 2, argv, &rv);
}

void
cqctprintval(VM *vm, Val v)
{
	printval(vm, v);
}

typedef
struct NSroot {
	Rkind base[Vnbase];
	Cbase ptr;
	Cbase xint8, xint16, xint32, xint64;
	Cbase xuint8, xuint16, xuint32, xuint64;
	char *name;
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
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vlong,
.xint64 = Vvlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vulong,
.xuint64 = Vuvlong,
.name = "c32le",
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
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vlong,
.xint64 = Vvlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vulong,
.xuint64 = Vuvlong,
.name = "c32be",
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
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "c64le",
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
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "c64be",
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
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "clp64le",
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
.xint8 = Vchar,
.xint16 = Vshort,
.xint32 = Vint,
.xint64 = Vlong,
.xuint8 = Vuchar,
.xuint16 = Vushort,
.xuint32 = Vuint,
.xuint64 = Vulong,
.name = "clp64be",
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

static Xtypename*
mkconstxtn(Xtypename *t)
{
	Xtypename *xtn;
	xtn = mkxtn();
	xtn->tkind = Tconst;
	xtn->link = t;
	return xtn;
}

static Xtypename*
mktypedefxtn(Str *tid, Xtypename *t)
{
	Xtypename *xtn;
	xtn = mkxtn();
	xtn->tkind = Ttypedef;
	xtn->tid = tid;
	xtn->link = t;
	return xtn;
}

static Tab*
basetab(NSroot *def, Xtypename **base)
{
	Cbase cb;
	Val kv, vv;
	Tab *type;
	Str *tn;

	for(cb = Vchar; cb < Vnbase; cb++)
		base[cb] = mkbasextn(cb, def->base[cb]);
	base[Vptr] = base[def->ptr];

	type = mktab();
	for(cb = Vchar; cb < Vnbase; cb++){
		kv = mkvalxtn(base[cb]);
		vv = mkvalxtn(base[cb]);
		_tabput(type, kv, vv);
	}

	/* map pointer to integer representation */
	kv = mkvalxtn(mkbasextn(Vptr, Rundef));
	vv = mkvalxtn(base[Vptr]);
	_tabput(type, kv, vv);

	/* define stdint-like integer typedefs */

	tn = mkstr0("uintptr");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->ptr]));
	_tabput(type, kv, vv);

	tn = mkstr0("int8");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint8]));
	_tabput(type, kv, vv);
	tn = mkstr0("int16");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint16]));
	_tabput(type, kv, vv);
	tn = mkstr0("int32");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint32]));
	_tabput(type, kv, vv);
	tn = mkstr0("int64");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint64]));
	_tabput(type, kv, vv);

	tn = mkstr0("uint8");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint8]));
	_tabput(type, kv, vv);
	tn = mkstr0("uint16");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint16]));
	_tabput(type, kv, vv);
	tn = mkstr0("uint32");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint32]));
	_tabput(type, kv, vv);
	tn = mkstr0("uint64");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint64]));
	_tabput(type, kv, vv);

	return type;
}

static Ns*
mkrootns(NSroot *def)
{
	Tab *type;
	Ns *ns;
	Xtypename *base[Vnallbase];

	memset(base, 0, sizeof(base)); /* values will be seen by GC */
	type = basetab(def, base);
	ns = mknstype(type, mkstr0(def->name));
	memcpy(ns->base, base, sizeof(base));
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
	root = valns(valv);

	rawtype = mktab();
	rawsym = mktab();

	t = mkxtn();
	t->tkind = Tarr;
	t->cnt = mkvalcval(vm->litdom, vm->litbase[Vint], 256);
	t->link = mkbasextn(Vchar, Rundef);
	id = mkstr0("flags");
	o = mkcval(vm->litdom, vm->litbase[Vptr], (Imm)&cqctflags);
	
	v = mkvec(3);
	valv = mkvalxtn(t);
	_vecset(v, Typepos, valv);
	valv = mkvalstr(id);
	_vecset(v, Idpos, valv);
	valv = mkvalcval2(o);
	_vecset(v, Offpos, valv);

	keyv = mkvalstr(id);
	valv = mkvalvec(v);
	tabput(vm, rawsym, keyv, valv);

	ns = mknstab(vm, root, rawtype, rawsym, mkstr0("sys"));
	dom = mkdom(ns, mksas(mkstrk(0, ~(0ULL), Sperm)), mkstr0("sys"));

	return dom;
}

static Dom*
mklitdom()
{
	Dom *dom;
	dom = mkdom(mkrootns(&clp64le), mknas(), mkstr0("litdom"));

	litdom = dom;
	litns = dom->ns;
	litbase = litns->base;

	return dom;
}

static VM *vms[Maxvms];

#define FN(name) builtinfn(env, #name, mkcfn(#name, l1_##name))

Env*
mktopenv()
{
	Env *env;
	Dom *litdom;

	env = mkenv();
	
	builtinfn(env, "halt", haltthunk());
	builtinfn(env, "callcc", callcc());

	FN(_readdir);
	FN(append);
	FN(apply);
	FN(arraynelm);
	FN(asdispatch);
	FN(asof);
	FN(backtrace);
	FN(baseid);
	FN(bitfieldcontainer);
	FN(bitfieldpos);
	FN(bitfieldwidth);
	FN(car);
	FN(cdr);
	FN(close);
	FN(cons);
	FN(copy);
	FN(domof);
	FN(enumconsts);
	FN(equal);
	FN(error);
	FN(fault);
	FN(fdname);
	FN(fieldid);
	FN(fieldoff);
	FN(fields);
	FN(fieldtype);
	FN(foreach);
	FN(fprintf);
	FN(gc);
	FN(getbytes);
	FN(getpid);
	FN(gettimeofday);
	FN(head);
	FN(isarray);
	FN(isas);
	FN(isbase);
	FN(isbitfield);
	FN(isctype);
	FN(iscvalue);
	FN(isdomain);
	FN(isempty);
	FN(isenum);
	FN(isenumconst);
	FN(isfd);
	FN(isfunc);
	FN(islist);
	FN(ismapped);
	FN(isnil);
	FN(isns);
	FN(isnull);
	FN(ispair);
	FN(isprocedure);
	FN(isptr);
	FN(isrange);
	FN(isstring);
	FN(isstruct);
	FN(issu);
	FN(istable);
	FN(istypedef);
	FN(isundeftype);
	FN(isunion);
	FN(isvector);
	FN(isvoid);
	FN(length);
	FN(list);
	FN(listdel);
	FN(listins);
	FN(listref);
	FN(listset);
	FN(lookfield);
	FN(looktype);
	FN(mapfile);
	FN(mkas);
	FN(mkctype_array);
	FN(mkctype_bitfield);
	FN(mkctype_char);
	FN(mkctype_const);
	FN(mkctype_double);
	FN(mkctype_enum);
	FN(mkctype_float);
	FN(mkctype_fn);
	FN(mkctype_int);
	FN(mkctype_ldouble);
	FN(mkctype_long);
	FN(mkctype_ptr);
	FN(mkctype_short);
	FN(mkctype_struct);
	FN(mkctype_typedef);
	FN(mkctype_uchar);
	FN(mkctype_uint);
	FN(mkctype_ulong);
	FN(mkctype_union);
	FN(mkctype_ushort);
	FN(mkctype_uvlong);
	FN(mkctype_vlong);
	FN(mkctype_void);
	FN(mkdir);
	FN(mkdom);
	FN(mkfield);
	FN(mknas);
	FN(mkns);
	FN(mknsraw);
	FN(mkparam);
	FN(mkrange);
	FN(mksas);
	FN(mkstr);
	FN(mksym);
	FN(mktab);
	FN(mkvec);
	FN(mkzas);
	FN(nsof);
	FN(nsenumsym);
	FN(nsenumtype);
	FN(nslookaddr);
	FN(nslooksym);
	FN(nslooktype);
	FN(nsptr);
	FN(null);
	FN(open);
	FN(opentcp);
	FN(paramid);
	FN(params);
	FN(paramtype);
	FN(pop);
	FN(print);		/* FIXME: remove: held for test suite */
	FN(printf);
	FN(push);
	FN(putbytes);
	FN(rangebeg);
	FN(rangelen);
	FN(rand);
	FN(randseed);
	FN(read);
	FN(rettype);
	FN(reverse);
	FN(rmdir);
	FN(sort);
	FN(sprintfa);
	FN(stringof);
	FN(strlen);
	FN(strput);
	FN(strton);
	FN(substr);
	FN(subtype);
	FN(suekind);
	FN(suetag);
	FN(susize);
	FN(symid);
	FN(symtype);
	FN(symval);
	FN(tabdelete);
	FN(tabenum);
	FN(tabinsert);
	FN(tabkeys);
	FN(tablook);
	FN(tabvals);
	FN(tail);
	FN(typedefid);
	FN(typedeftype);
	FN(unlink);
	FN(veclen);
	FN(vecref);
	FN(vecset);
	FN(vector);
	FN(write);

	/* FIXME: these bindings should be immutable */
	litdom = mklitdom();
	builtindom(env, "litdom", litdom);
	builtinfd(env, "stdin", mkfd(mkstr0("<stdin>"), 0, Fread, 0));
	builtinfd(env, "stdout", mkfd(mkstr0("<stdout>"), 1, Fwrite, 0));
	builtinns(env, "c32le", mkrootns(&c32le));
	builtinns(env, "c32be", mkrootns(&c32be));
	builtinns(env, "c64le", mkrootns(&c64le));
	builtinns(env, "c64be", mkrootns(&c64be));
	builtinns(env, "clp64le", mkrootns(&clp64le));
	builtinns(env, "clp64be", mkrootns(&clp64be));
	cvalnull = mkcval(litdom, litdom->ns->base[Vptr], 0);
	builtincval(env, "NULL", cvalnull);

	cval0 = mkcval(litdom, litdom->ns->base[Vint], 0);
	cval1 = mkcval(litdom, litdom->ns->base[Vint], 1);
	cvalminus1 = mkcval(litdom, litdom->ns->base[Vint], -1);

	/* stashed in env only to protect them from gc */
	builtincval(env, "$0", cval0);
	builtincval(env, "$1", cval1);
	builtincval(env, "$-1", cvalminus1);

	/* expanded source may call these magic functions */
	builtinfn(env, "$put", mkcfn("$put", l1_put));
	builtinfn(env, "$typeof", mkcfn("$typeof", l1_typeof));

	return env;
}

VM*
cqctmkvm(Env *env)
{
	VM *vm, **vmp;
	Val val;

	vm = xmalloc(sizeof(VM));
	vm->top = env;
	vm->pmax = GCinitprot;
	vm->prot = xmalloc(vm->pmax*sizeof(Root*));
	vm->emax = Errinitdepth;
	vm->err = xmalloc(vm->emax*sizeof(Err));
	
	envlookup(env, "litdom", &val);
	vm->litdom = valdom(val);
	vm->litns = vm->litdom->ns;
	vm->litbase = vm->litns->base;
	vm->sget = mkstr0("get");
	vm->sput = mkstr0("put");
	vm->smap = mkstr0("map");
	envlookup(env, "stdin", &val);
	vm->stdin = valfd(val);
	envlookup(env, "stdout", &val);
	vm->stdout = valfd(val);

	vmp = vms;
	while(*vmp){
		vmp++;
		if(vmp > vms+Maxvms)
			fatal("too many vms");
	}
	*vmp = vm;

	vmreset(vm);
	concurrentgc(vm);
	gcprotpush(vm);
	
	/* vm is now callable */

	/* add toplevel bindings that require VM to construct */
	if(!envlookup(env, "sys", &val))
		builtindom(env, "sys", mksysdom(vm));

	return vm;
}

void
cqctfreevm(VM *vm)
{
	VM **vmp;

	gcprotpop(vm);
	gckill(vm);
	freefreeroots(&vm->rs);
	free(vm->prot);
	free(vm->err);
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp == vm){
			*vmp = 0;
			break;
		}
		vmp++;
	}
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
	Head *hd;

	hd = xmalloc(sizeof(Head));
	hd->qkind = Qundef;
	Xundef = hd;

	hd = xmalloc(sizeof(Head));
	hd->qkind = Qnil;
	Xnil = hd;

	hd = xmalloc(sizeof(Head));
	hd->qkind = Qnulllist;
	Xnulllist = hd;

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

	freefreeroots(&roots);
	freefreeroots(&stores);

	freecode((Head*)kcode);
	freecode((Head*)cccode);
	free(GCiterdone);

	free(Xundef);
	free(Xnil);
	free(Xnulllist);

	for(i = 0; i < Qnkind; i++){
		hp = &heap[i];
		if(hp->id)
			freeheap(hp);
	}
	setfaulthook(0);
}

int
cqctcallthunk(VM *vm, Closure *cl, Val *vp)
{
	if(waserror(vm)){
		vmreset(vm);
		return -1;
	}
	dovm(vm, cl, 0, 0);
	*vp = vm->ac;
	return 0;
}

/* these routines assume litdom is clp64le */

int8_t
cqctval2int8(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint8];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

int16_t
cqctval2int16(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint16];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

int32_t
cqctval2int32(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint32];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

int64_t
cqctval2int64(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint64];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint8_t
cqctval2uint8(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint8];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint16_t
cqctval2uint16(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint16];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint32_t
cqctval2uint32(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint32];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint64_t
cqctval2uint64(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(v->qkind != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint64];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

char*
cqctval2cstr(Val v)
{
	if(v->qkind != Qstr)
		return 0;
	return str2cstr(valstr(v));
}
