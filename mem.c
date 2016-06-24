#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum {
	Noheap = 0,
	Heapdefined = 1,
};

static Head constnil;
#pragma weak findheapimage
extern void *findheapimage(u64 *len);
static u32 heapversion = 2;

/* if you change this, be sure the
   ordering in MTx enum remains consistent */
typedef
enum
{
	Mhole,
	Mnix,
	Mfree,
	Mweak,
	Mbox,
	Mmutable,
	Mdata,
	Mstack,
	Mcode,
	Nm,
} Mtag;

typedef
enum
{
	/* bit positions */
	Fold = 0,
	Fbig,
	Fbigcont,
	Ftag,
} Flags;

/* meta type values.
   keep the ordering here consistent with Mtag ordering
   in order to ensure that Nmt subsumes all types */
enum
{
	MThole    = (Mhole    << Ftag),
	MTnix     = (Mnix     << Ftag),
	MTfree    = (Mfree    << Ftag),
	MTweak    = (Mweak    << Ftag),
	MTbox     = (Mbox     << Ftag),
	MTmutable = (Mmutable << Ftag),
	MTdata    = (Mdata    << Ftag),
	MTbigdata = (Mdata    << Ftag) | (1 << Fbig),
	MTstack   = (Mstack   << Ftag),
	MTcode    = (Mcode    << Ftag),
	MTbigcode = (Mcode    << Ftag) | (1 << Fbig),
	Nmt,
};

static char *MTname[] = {
	[MThole]    = "hole",
	[MTnix]     = "nix",
	[MTfree]    = "free",
	[MTdata]    = "data",
	[MTcode]    = "code",
	[MTweak]    = "weak",
	[MTbox]     = "box",
	[MTstack]   = "stack",
	[MTmutable] = "mutable",
	[MTbigdata] = "bigdata",
	[MTbigcode] = "bigcode",

	[MThole|1<<Fold]    = "old hole",
	[MTnix|1<<Fold]     = "old nix",
	[MTfree|1<<Fold]    = "old free",
	[MTdata|1<<Fold]    = "old data",
	[MTcode|1<<Fold]    = "old code",
	[MTweak|1<<Fold]    = "old weak",
	[MTbox|1<<Fold]     = "old box",
	[MTstack|1<<Fold]   = "old stack",
	[MTmutable|1<<Fold] = "old mutable",
	[MTbigdata|1<<Fold] = "old bigdata",
	[MTbigcode|1<<Fold] = "old bigcode",
};

#define MTtag(mt)        ((mt) >> Ftag)
#define MTold(mt)        (((mt) >> Fold) & 1)
#define MTbig(mt)        (((mt) >> Fbig) & 1)
#define MTbigcont(mt)    (((mt) >> Fbigcont) & 1)
#define MTsettag(mt, t)   ((mt) = ((t) << Ftag) | ((mt) & ((1<<Ftag) - 1)))
#define MTsetold(mt)     ((mt) |= 1 << Fold)
#define MTsetbig(mt)     ((mt) |= 1 << Fbig)
#define MTsetbigcont(mt) ((mt) |= 1 << Fbigcont)
#define MTclrold(mt)     ((mt) &= ~(1 << Fold))
#define MTclrbig(mt)     ((mt) &= ~(1 << Fbig))
#define MTclrbigcont(mt) ((mt) &= ~(1 << Fbigcont))

/* #define to ensure 64-bit constants */
#if 0
#define Segbits   12
#define Segsize   (((uptr)1) << Segbits)
#define	GCthresh  2*1024*Segsize
#define Seghunk	  4*1024*Segsize
#else
#define Segbits   20
#define Segsize   (((uptr)1) << Segbits)
#define	GCthresh  4*Segsize
#define Seghunk	  4*Segsize
#endif

#define Cardbits  3
#define Ncard     (((uptr)1) << Cardbits)
#define Cardsize  (((uptr)1) << (Segbits - Cardbits))
#define Seguse    (Segsize - sizeof(void*))
#define Segmask   ~(Segsize - 1)
#define Minheap   10*Seghunk
#define Align     4
#define card(a)   ((u8)(((uptr)(a)>>(Segbits-Cardbits))&(Ncard-1)))

/* n must be a power-of-2 */
#define roundup(l, n)   ((uptr)(((uptr)(l)+((n)-1))&~((n)-1)))
#define rounddown(l, n) ((uptr)(((uptr)(l))&~((n)-1)))
#define align(l)       (roundup((l), Align))

enum
{
	GCradix = 4,
};

/* Gen values */
enum
{
	G0,
	G1,
	G2,
	G3,
	Gstatic,
	Ngen = Gstatic,
	Nsgen = Gstatic + 1,
	Allgen,
	Clean = 0xff,
	Dirty = 0x00,
};

static char *genname[] = {
	"0",
	"1",
	"2",
	"3",
	"S",
};

typedef u8 Gen;
typedef u8 MT;

typedef struct Seg Seg;
struct Seg
{
	MT mt;
	Gen gen;
	void *a;		/* allocation pointer */
	void *e;		/* pointer to last byte in segment */
	u8 card[Ncard];
	u8 crossing[Ncard];
	u32 n;			/* segment # for saveheap */
	void *link;
};

typedef
struct Segmap
{
	void *lo, *hi;
	void *free;
	Seg *map;
} Segmap;

typedef
struct LSctx
{
	enum {
		LSsave,
		LSload,
	} mode;

	/* context for load */
	Seg *seg;
	u32 nseg;
	char **csym;
	u32 ncsym;
	void *top;
} LSctx;

typedef
struct Qtype
{
	char *id;
	u32 sz;
	u32 clearit;
	u8 (*scan)(Head *hd);
	u8 (*loadsave)(Head *hd, LSctx *ls);
} Qtype;

typedef
struct M
{
	void *h, *t;		/* head and tail of segment list */
	void *scan;		/* scan pointer */
} M;

typedef
struct Guard
{
	Pair *gd[Nsgen];	/* guarded objects: (obj . guardian) pairs  */
} Guard;

typedef
struct Heap
{
	M m[Nmt][Nsgen];	/* metatypes */
	u32 g, tg;		/* collect generation and target generation */
	u64 na;			/* bytes allocated since last gc */
	u64 ma;			/* bytes allocated to trigger collect */
	u64 inuse;		/* bytes currently allocated to active segs */
	u64 free;		/* bytes currently allocated to free segs */
	u64 heapsz;		/* total bytes currently allocated to heap */
	u64 bigsz;		/* bytes currently allocated to big segs */
	u64 smapsz;		/* bytes currently allocated to segment map */
	Guard guard;		/* guard list */
	unsigned disable;
	u32 gctrip, gcsched[Ngen], ingc;
} Heap;

typedef
struct Stats
{
	u32 ncard;
	u32 nseg[Nmt][Nsgen];
	u64 roottime;
	u64 sweeptime;
	u64 guardtime;
	u64 cardtime;
	u64 oldtime;
	u64 weaktime;
	u64 inittime;
	u64 recycletime;
	u64 resettime;
	u64 finimainttime;
	u64 crossings[Ncard];
	u64 multicardscan;
} Stats;


typedef
struct Cfnrec
{
	char *name;
	void *addr;
} Cfnrec;

enum{
	Maxcfnrec = 1024,
};

static u32 ncfn;
static Cfnrec cfn[Maxcfnrec];

static u8	scanas(Head*);
static u8	scanbox(Head*);
static u8	scancl(Head*);
static u8	scancode(Head*);
static u8	scancont(Head*);
static u8	scanctype(Head*);
static u8	scancval(Head*);
static u8	scandom(Head*);
static u8	scanexpr(Head*);
static u8	scanlist(Head*);
static u8	scanns(Head*);
static u8	scanpair(Head*);
static u8	scanprecode(Head*);
static u8	scanrange(Head*);
static u8	scantab(Head*);
static u8	scanvec(Head*);

static u8	loadsaveas(Head*, LSctx *ls);
static u8	loadsavebox(Head*, LSctx *ls);
static u8	loadsavecl(Head*, LSctx *ls);
static u8	loadsavecode(Head*, LSctx *ls);
static u8	loadsavecont(Head*, LSctx *ls);
static u8	loadsavectype(Head*, LSctx *ls);
static u8	loadsavecval(Head*, LSctx *ls);
static u8	loadsavedom(Head*, LSctx *ls);
static u8	loadsaveexpr(Head*, LSctx *ls);
static u8	loadsavelist(Head*, LSctx *ls);
static u8	loadsavens(Head*, LSctx *ls);
static u8	loadsavepair(Head*, LSctx *ls);
static u8	loadsaveprecode(Head*, LSctx *ls);
static u8	loadsaverange(Head*, LSctx *ls);
static u8	loadsavetab(Head*, LSctx *ls);
static u8	loadsavevec(Head*, LSctx *ls);

static void	copykstack(Cont *k, u8 *min);
static void	copystack(void **basep, u32 sz, void **rap, Closure *cl, u32 fpo, u8 *min);
static void	gcopy(Val *v, u8 *min);

static Qtype qs[Qnkind] = {
	[Qas]	 	= { "as", sizeof(As), 1, scanas, loadsaveas, },
	[Qbox]	 	= { "box", sizeof(Box), 0, scanbox, loadsavebox },
	[Qcid]   	= { "cid", sizeof(Cid), 1, 0 },
	[Qcl]	 	= { "closure", sizeof(Closure), 1, scancl, loadsavecl },
	[Qcode]	 	= { "code", sizeof(Code), 1, scancode, loadsavecode },
	[Qcont]	 	= { "stack", sizeof(Cont), 1, scancont, loadsavecont },
	[Qctype] 	= { "ctype", sizeof(Ctype), 1, scanctype, loadsavectype },
	[Qcval]  	= { "cval", sizeof(Cval), 0, scancval, loadsavecval },
	[Qdom]	 	= { "domain", sizeof(Dom), 0, scandom, loadsavedom },
	[Qexpr]	 	= { "expr", sizeof(Expr), 1, scanexpr, loadsaveexpr },
	[Qlist]	 	= { "list", sizeof(List), 0, scanlist, loadsavelist },
	[Qnil]	 	= { "nil", sizeof(Head), 0, 0 },
	[Qns]	 	= { "ns", sizeof(Ns), 1, scanns, loadsavens },
	[Qpair]	 	= { "pair", sizeof(Pair), 0, scanpair, loadsavepair },
	[Qprecode]	= { "precode", sizeof(Precode), 1, scanprecode, loadsaveprecode },
	[Qrange] 	= { "range", sizeof(Range), 0, scanrange, loadsaverange },
	//[Qrd]    	= { "rd", sizeof(Rd), 0, scanrd, loadsaverd },
	//[Qrec]	 	= { "record", sizeof(Rec), 0, scanrec, loadsaverec },
	[Qstr]	 	= { "string", sizeof(Str), 1, 0 },
	[Qtab]	 	= { "table",  sizeof(Tab), 1, scantab, loadsavetab },
	[Qvec]	 	= { "vector", sizeof(Vec), 0, scanvec, loadsavevec },
};

static Segmap	segmap;
static Heap	H;
static unsigned	alldbg = 0;
static Stats	stats;

static u8
scanas(Head *hd)
{
	u8 min;
	As *as;

	min = Clean;
	as = (As*)hd;

	gcopy((Val*)&as->mtab, &min);
	gcopy((Val*)&as->name, &min);
	gcopy((Val*)&as->get, &min);
	gcopy((Val*)&as->put, &min);
	gcopy((Val*)&as->ismapped, &min);
	gcopy((Val*)&as->map, &min);
	gcopy((Val*)&as->dispatch, &min);
	return min;
}

static u8
scanbox(Head *hd)
{
	u8 min;
	Box *box;

	min = Clean;
	box = (Box*)hd;
	gcopy((Val*)&box->v, &min);
	return min;
}

static u8
scancl(Head *hd)
{
	u8 min;
	unsigned i;
	Closure *cl;

	min = Clean;
	cl = (Closure*)hd;
	gcopy((Val*)&cl->code, &min);
	for(i = 0; i < cl->dlen; i++)
		gcopy(&cldisp(cl)[i], &min);
	return min;
}

static u8
scancode(Head *h)
{
	u8 min;
	Imm i;
	Reloc *r;
	void *v, **cp;
	void *p;
	Code *c;

	min = Clean;
	c = (Code*)h;
	gcopy((Val*)&c->id, &min);
	gcopy((Val*)&c->reloc, &min);
	gcopy((Val*)&c->lm, &min);
	gcopy((Val*)&c->dbg, &min);
	gcopy((Val*)&c->src, &min);
	gcopy((Val*)&c->lex, &min);
	if(c->kind == Calien)
		gcopy(&c->alien, &min);
	if(c->reloc == 0)
		return min;
	r = (Reloc*)strdata(c->reloc);
	p = c+1;
	for(i = 0; i < c->nreloc; i++, r++){
		cp = (void**)(p+r->coff);
		v = *cp;
		gcopy((Val*)&v, &min);
		*cp = v;
	}
	return min;
}

/* order matters: copy the stack before the closure. */
static u8
scancont(Head *hd)
{
	u8 min;
	Cont *k;

	min = Clean;
	k = (Cont*)hd;
	if(k->base == 0)
		/* defunct continuation in restored heap */
		return min;
	gcopy((Val*)&k->link, &min);
	copykstack(k, &min);
	gcopy((Val*)&k->cl, &min);
	return min;
}

static u8
scanprecode(Head *hd)
{
	u8 min;
	Precode *c;

	min = Clean;
	c = (Precode*)hd;
	gcopy((Val*)&c->id, &min);
	gcopy((Val*)&c->lm, &min);
	gcopy((Val*)&c->dbg, &min);
	gcopy((Val*)&c->src, &min);
	return min;
}

static u8
scanctype(Head *hd)
{
	u8 min;
	Ctype *t;
	Ctypearr *ta;
	Ctypeconst *tc;
	Ctypedef *td;
	Ctypeenum *te;
	Ctypefunc *tf;
	Ctypesu *ts;
	Ctypeptr *tp;
	Ctypeundef *tu;
	Ctypebitfield *tw;

	min = Clean;
	t = (Ctype*)hd;
	switch(t->tkind){
	case Tvoid:
	case Tbase:
		break;
	case Tstruct:
	case Tunion:
		ts = (Ctypesu*)t;
		gcopy((Val*)&ts->tag, &min);
		gcopy((Val*)&ts->field, &min);
		gcopy(&ts->attr, &min);
		break;
	case Tenum:
		te = (Ctypeenum*)t;
		gcopy((Val*)&te->tag, &min);
		gcopy((Val*)&te->sub, &min);
		gcopy((Val*)&te->konst, &min);
		break;
	case Ttypedef:
		td = (Ctypedef*)t;
		gcopy((Val*)&td->sub, &min);
		gcopy((Val*)&td->tid, &min);
		break;
	case Tundef:
		tu = (Ctypeundef*)t;
		gcopy((Val*)&tu->sub, &min);
		break;
	case Tconst:
		tc = (Ctypeconst*)t;
		gcopy((Val*)&tc->sub, &min);
		break;
	case Tptr:
		tp = (Ctypeptr*)t;
		gcopy((Val*)&tp->sub, &min);
		break;
	case Tarr:
		ta = (Ctypearr*)t;
		gcopy(&ta->cnt, &min);
		gcopy((Val*)&ta->sub, &min);
		break;
	case Tfun:
		tf = (Ctypefunc*)t;
		gcopy((Val*)&tf->sub, &min);
		gcopy((Val*)&tf->param, &min);
		break;
	case Tbitfield:
		tw = (Ctypebitfield*)t;
		gcopy(&tw->cnt, &min);
		gcopy(&tw->bit0, &min);
		gcopy((Val*)&tw->sub, &min);
		break;
	default:
		bug();
	}
	return min;
}

static u8
scancval(Head *hd)
{
	u8 min;
	Cval *cval;

	min = Clean;
	cval = (Cval*)hd;
	gcopy((Val*)&cval->dom, &min);
	gcopy((Val*)&cval->type, &min);
	return min;
}

static u8
scandom(Head *hd)
{
	u8 min;
	Dom *dom;

	min = Clean;
	dom = (Dom*)hd;
	gcopy((Val*)&dom->as, &min);
	gcopy((Val*)&dom->ns, &min);
	gcopy((Val*)&dom->name, &min);
	return min;
}

static u8
scanexpr(Head *hd)
{
	u8 min;
	Expr *e;

	min = Clean;
	e = (Expr*)hd;
	gcopy((Val*)&e->e1, &min);
	gcopy((Val*)&e->e2, &min);
	gcopy((Val*)&e->e3, &min);
	gcopy((Val*)&e->e4, &min);
	gcopy((Val*)&e->aux, &min);
	gcopy((Val*)&e->skind, &min);
	gcopy((Val*)&e->src, &min);
	return min;
}

static u8
scanlist(Head *hd)
{
	u8 min;
	List *l;

	min = Clean;
	l = (List*)hd;
	gcopy((Val*)&l->v, &min);
	return min;
}

static u8
scanns(Head *hd)
{
	u8 min;
	Ns *ns;
	unsigned i;

	min = Clean;
	ns = (Ns*)hd;
	gcopy((Val*)&ns->lookaddr, &min);
	gcopy((Val*)&ns->looksym, &min);
	gcopy((Val*)&ns->looktype, &min);
	gcopy((Val*)&ns->enumtype, &min);
	gcopy((Val*)&ns->enumsym, &min);
	gcopy((Val*)&ns->name, &min);
	gcopy((Val*)&ns->dispatch, &min);
	gcopy((Val*)&ns->mtab, &min);
	for(i = 0; i < Vnallbase; i++)
		gcopy((Val*)&ns->base[i], &min);
	return min;
}

static u8
scanpair(Head *hd)
{
	u8 min;
	Pair *pair;

	min = Clean;
	pair = (Pair*)hd;
	gcopy(&pair->cdr, &min);
	if(isweak(hd))
		return min;
	gcopy(&pair->car, &min);
	return min;
}

static u8
scanrange(Head *hd)
{
	u8 min;
	Range *range;

	min = Clean;
	range = (Range*)hd;
	gcopy((Val*)&range->beg, &min);
	gcopy((Val*)&range->len, &min);
	return min;
}

static u8
scantab(Head *hd)
{
	u8 min;
	Tab *t;

	min = Clean;
	t = (Tab*)hd;
	gcopy((Val*)&t->ht, &min);
	gcopy((Val*)&t->tg, &min);
	gcopy((Val*)&t->def, &min);
	if(t->priv) {
		gcopy((Val*)&t->name, &min);
		gcopy((Val*)&t->fmt, &min);
	}
	return min;
}

static u8
scanvec(Head *hd)
{
	u8 min;
	Vec *vec;
	Imm i;

	min = Clean;
	vec = (Vec*)hd;
	for(i = 0; i < vec->len; i++)
		gcopy(&vecdata(vec)[i], &min);
	return min;
}

static void*
s2a(Seg *s)
{
	u64 o;
	if(s < segmap.map)
		fatal("bug");
	o = s-segmap.map;
	return segmap.lo+o*Segsize;
}

static Seg*
a2s(void *a)
{
	u64 o;
	if(a == Xnil)
		bug();
	o = (a-segmap.lo)/Segsize;
	return segmap.map+o;
}

static void
unmapmem(void *a, u64 sz)
{
	if(sz == 0)
		return;
	if(0 > munmap(a, sz))
		fatal("munmap: %s", strerror(errno));
}

static void*
mapmem(u64 sz)
{
	void *p;
	uptr l, r;

	/* sz must be Segsize-aligned.  pad by one
	   Segsize to ensure we can return a
	   Segsize-aligned allocation */
	p = mmap(0, sz+Segsize, PROT_READ|PROT_WRITE|PROT_EXEC,
		 MAP_ANON|MAP_PRIVATE,
		 -1, 0);
	if(p == MAP_FAILED)
		fatal("out of memory");

	/* trim to Segsize-aligned */
	l = (uptr)p&(Segsize-1);
	r = Segsize-l;
	unmapmem(p, r);
	unmapmem(p+r+sz, l);
	return p+r;
}

static void
setrangetype(void *p, void *e, MT t)
{
	u64 n;
	Seg *s, *es;

	p = (void*)rounddown(p, Segsize);
	e = (void*)roundup(e, Segsize);

	n = (e-p)/Segsize;
	s = a2s(p);
	es = s+n;
	while(s < es){
		s->mt = t;
		s++;
	}
}

static void
freerange(void *p, void *e)
{
	void *f, **q;

	/* this should be unnecessary */
	p = (void*)rounddown(p, Segsize);
	e = (void*)roundup(e, Segsize);

	setrangetype(p, e, MTfree);
	H.free += e-p;
	f = segmap.free;
	while(p < e){
		q = (void**)p;
		*q = f;
		f = p;
		p += Segsize;
	}
	segmap.free = f;
}

static void*
nextfree()
{
	void *p, **q;
	if(segmap.free == 0)
		fatal("bug");
	p = segmap.free;
	q = (void**)p;
	segmap.free = *q;
	H.free -= Segsize;
	return p;
}

static void
freeseg(Seg *s)
{
	void *a, **q;

	if(MTbig(s->mt))
		fatal("bug");
	s->mt = MTfree;
	a = s2a(s);
	q = (void**)a;
	*q = segmap.free;
	segmap.free = a;
	H.free += Segsize;
	H.inuse -= Segsize;
}

static void
freebigseg(Seg *s)
{
	void *p;
	u64 sz;
	Seg *es;

	p = s2a(s);
	sz = 0;
	es = a2s(s->e);
	while(s <= es){
		s->mt = MThole;
		sz += Segsize;
		s++;
	}
	unmapmem(p, sz);
	H.bigsz -= sz;
}

static void*
allocsegmap(u64 nseg)
{
	H.smapsz += roundup(nseg*sizeof(Seg), Segsize);
	return mapmem(roundup(nseg*sizeof(Seg), Segsize));
}

static void
freesegmap(void *m, u64 nseg)
{
	H.smapsz -= roundup(nseg*sizeof(Seg), Segsize);
	unmapmem(m, roundup(nseg*sizeof(Seg), Segsize));
}

static void
remapsegmap(void *p, void *e)
{
	uptr nseg, onseg;
	Seg *m, *om;
	void *olo, *ohi;

	if(p >= segmap.lo && e <= segmap.hi)
		return;

	olo = segmap.lo;
	ohi = segmap.hi;
	onseg = (ohi-olo)/Segsize;

	if(p <= olo)
		segmap.lo = p;
	if(e >= ohi)
		segmap.hi = e;
	nseg = (segmap.hi-segmap.lo)/Segsize;

	om = segmap.map;
	m = allocsegmap(nseg);
	memcpy(m+(olo-segmap.lo)/Segsize, om, onseg*sizeof(Seg));
	segmap.map = m;
	freesegmap(om, onseg);

	/* mark new hole (if any) */
	if(p > ohi)
		setrangetype(ohi, p, MThole);
	else if(e < olo)
		setrangetype(e, olo, MThole);
}

static Seg*
nextseg(Seg *s)
{
	if(MTbig(s->mt))
		return a2s(s->e)+1;
	return s+1;
}

static void
grow(u64 sz)
{
	void *p, *e;

	if(H.ingc)
		fatal("segmap update within gc");
	sz = roundup(sz, Segsize);
	p = mapmem(sz);
	e = p+sz;
	H.heapsz += sz;
	remapsegmap(p, e);
	freerange(p, e);
}

static void
initsegmap()
{
	u64 sz, nseg;
	void *p, *e;

	sz = Seghunk;
	nseg = sz/Segsize;
	p = mapmem(sz);
	e = p+sz;
	H.heapsz += sz;
	segmap.lo = p;
	segmap.hi = e;
	segmap.map = allocsegmap(nseg);
	segmap.free = 0;
	freerange(p, e);
}

static void
initsegmap2(void *p, u64 sz)
{
	u64 nseg;
	void *e;

	nseg = sz/Segsize;
	e = p+sz;
	H.heapsz += sz;
	segmap.lo = p;
	segmap.hi = e;
	segmap.map = allocsegmap(nseg);
	segmap.free = 0;
}

enum
{
	LORES = 1,  /* low reserve; triggers growth */
	HIRES = 4,  /* high reserve; triggers shrink */
	TARGRES = 2,
	/* HIRES-LORES must be > 1 */
};

static void
shrink(u64 targ)
{
	void *p, *m;
	Seg *s, *es;
	u64 sz, tsz, n;

	n = 0;
	m = segmap.hi;
	tsz = 0;
	while(H.heapsz > Minheap && H.free > targ){
		p = nextfree();
		s = a2s(p);
		s->mt = MTnix;
		if(p < m)
			m = p;
		H.heapsz -= Segsize;
		tsz += Segsize;
		n++;
	}

	s = a2s(m);
	es = a2s(segmap.hi);
	sz = 0;
	p = 0;
	while(n > 0 && s < es){
		if(s->mt == MTnix){
			if(p == 0)
				p = s2a(s);
			sz += Segsize;
			s->mt = MThole;
			n--;
		}else if(p){
			unmapmem(p, sz);
			tsz -= sz;
			p = 0;
			sz = 0;
		}
		s = nextseg(s);
	}
	if(p){
		unmapmem(p, sz);
		tsz -= sz;
	}
	if(tsz || n)
		fatal("bug");
}

static void
maintain()
{
	u64 sz1, sz2;

	/* reserve no less than LORES*inuse and no more than HIRES*inuse */
	/* when balancing, aim for (HIRES-LORES)/2 */

	if(H.ingc)
		/* reserves must be sufficient to do a
		   gc.  it would be complicated to
		   adjust (increase) reserves during
		   gc. */
		return;

	if(H.free < LORES*H.inuse || H.heapsz < Minheap){
		sz1 = H.free < LORES*H.inuse ? (TARGRES-1)*H.inuse : 0;
		sz2 = H.heapsz < Minheap ? Minheap-H.heapsz : 0;
		grow(max(sz1, sz2));
	}else if(H.heapsz > Minheap && H.free > HIRES*H.inuse){
		shrink(TARGRES*H.inuse);
		if(H.heapsz > Minheap && H.free > HIRES*H.inuse)
			fatal("bug");
	}
}

static Seg*
allocseg(MT mt, Gen g)
{
	Seg *s;
	void *p;

	maintain();

	p = nextfree();
	H.na += Segsize;
	H.inuse += Segsize;
	s = a2s(p);
	s->a = p;
	s->e = p+Seguse-1;
	memset(s->card, Clean, sizeof(s->card));
	memset(s->crossing, 0, sizeof(s->crossing));
//	memset(s->a, 0, Segsize);  /* done at allocation time */
	if(s->mt != MTfree)
		fatal("bug");
	s->mt = mt;
	s->gen = g;
	s->link = 0;

	return s;
}

static Seg*
allocbigseg(MT mt, Gen g, u64 sz)
{
	Seg *s, *es;
	void *p;

	sz = roundup(sz, Segsize);
	p = mapmem(sz);
	remapsegmap(p, p+sz);
	H.na += sz;
	H.bigsz += sz;
	s = a2s(p);

	/* alloc pointer is bumped by caller in order to bound scan.
	   it is never bumped again.
	   end pointer points to end of concatenated segments. */
	s->a = p;
	s->e = p+sz-1;

	/* beginning Seg of big segment */
	memset(s->card, Clean, sizeof(s->card));
	memset(s->crossing, 0, sizeof(s->crossing));
	s->mt = mt;
	s->gen = g;
	s->link = 0;

	s++;
	es = a2s(p+sz);
	/* remaining Segs */
	while(s < es){
		memset(s->card, Clean, sizeof(s->card));
		memset(s->crossing, 0, sizeof(s->crossing));
		s->mt = mt;
		MTsetbigcont(s->mt);
		s->gen = g;
		s->a = s->e = 0;
		s->link = 0;

		if(!MTbig(s->mt))
			fatal("bug");
		s++;
	}

	return a2s(p);
}

/* like a2s, but if A is in interior of big segment,
   find its beginning segment */
static Seg*
lookseg(void *a)
{
	Seg *s;
	s = a2s(a);
	while(MTbigcont(s->mt))
		s--;
	return s;
}

static int
isliveseg(Seg *s)
{
	switch(MTtag(s->mt)){
	case Mdata:
	case Mcode:
	case Mweak:
	case Mbox:
	case Mmutable:
	case Mstack:
		return 1;
	default:
		return 0;
	}
}

static void
mclr(M *m)
{
	m->h = 0;
	m->t = 0;
	m->scan = 0;
}

static void
minit(M *m, Seg *s)
{
	s->link = 0;
	m->h = s2a(s);
	m->t = s2a(s);
	m->scan = s2a(s);
}

static void
minsert(M *m, Seg *s)
{
	Seg *t;
	void *p;

	if(m->h == 0){
		minit(m, s);
		return;
	}
	s->link = 0;
	t = a2s(m->t);
	p = s2a(s);
	t->link = p;
	m->t = p;
}

static void
importseg(MT mt, Gen g, void *p, uptr aoff, u8 *card, u8 *crossing)
{
	Seg *s;
	void *e;

	e = p+Segsize;
	remapsegmap(p, e);
	s = a2s(p);
	s->mt = mt;
	s->gen = g;
	if(aoff > Segsize)
		bug();
	s->a = p+aoff;
	s->e = p+Seguse-1; /* FIXME */
	memcpy(s->card, card, sizeof(s->card));
	memcpy(s->crossing, crossing, sizeof(s->crossing));
	minsert(&H.m[mt][g], s);
	H.na += Segsize;
	H.heapsz += Segsize;
	H.inuse += Segsize;

#if 0
	printf("load %10s %d %8x",
	       MTname[s->mt],
	       s->gen,
	       aoff);
	printf(" card");
	for(i = 0; i < Ncard; i++)
		printf(" %2x", s->card[i]);
	printf(" crossing");
	for(i = 0; i < Ncard; i++)
		printf(" %2x", s->crossing[i]);
	printf("\n");
#endif
}

static void
importbigseg(MT mt, Gen g, void *p, uptr aoff, u8 *card, u8 *crossing)
{
	Seg *s, *es;
	void *e;
	u64 sz;

	sz = roundup(aoff, Segsize);
	e = p+sz;
	remapsegmap(p, e);
	s = a2s(p);
	s->mt = mt;
	s->gen = g;
	s->a = p+aoff;
	s->e = p+aoff-1;
	memcpy(s->card, card, sizeof(s->card));
	memcpy(s->crossing, crossing, sizeof(s->crossing));
	minsert(&H.m[mt][g], s);
	s++;
	es = a2s(p+sz);
	while(s < es){
		memset(s->card, Clean, sizeof(s->card));
		memset(s->crossing, 0, sizeof(s->crossing));
		s->mt = mt;
		MTsetbigcont(s->mt);
		s->gen = g;
		s->a = s->e = 0;
		s->link = 0;
		s++;
	}
	H.na += sz;
	H.bigsz += sz;
}

u64
meminuse()
{
	return H.inuse;
}

int
isweak(Head *h)
{
	Seg *s;
	s = a2s(h);
	return MTtag(s->mt) == Mweak;
}

static Head*
__mal(MT mt, Gen g, u64 sz)
{
	M *m;
	Seg *s;
	Head *h;
	void *q;
	m = &H.m[mt][g];
again:
	s = a2s(m->t);
	if(s->a+sz <= s->e+1){
		h = s->a;
		s->a += sz;
		q = (void*)h+sz-1;
		while(card(h) != card(q)){
			s->crossing[card(q)] = 1;
			q -= Cardsize;
		}
		memset(h, 0, sz);
		return h;
	}
	minsert(m, allocseg(mt, g));
	goto again;
}

Head*
malbox()
{
	Head *h;
	h = __mal(MTbox, H.tg, sizeof(Box));
	Vsetkind(h, Qbox);
	return h;
}

Head*
malweak()
{
	Head *h;
	h = __mal(MTweak, H.tg, sizeof(Pair));
	Vsetkind(h, Qpair);
	return h;
}

static void*
_malbig(MT mt, u64 sz)
{
	Seg *s;
	s = allocbigseg(mt, H.tg, sz);
	minsert(&H.m[mt][H.tg], s);
	s->a += sz;
	return s2a(s);
}

static void*
_mal(Qkind kind, u64 sz)
{
	switch(kind){
	case Qbox:
		return __mal(MTbox, H.tg, sz);
	case Qctype:
	case Qvec:
	case Qlist:
	case Qtab:
		return __mal(MTmutable, H.tg, sz);
	case Qcode:
		return __mal(MTcode, H.tg, sz);
	default:
		return __mal(MTdata, H.tg, sz);
	}
}

Head*
malv(Qkind kind, Imm len)
{
	Head *h;
	len = roundup(len, Align);
	if(len > Seguse){
		if(kind == Qcode)
			h = _malbig(MTbigcode, len);
		else
			h = _malbig(MTbigdata, len);
	}else
		h = _mal(kind, len);
	Vsetkind(h, kind);
	return h;
}

void*
malstack(u32 sz)
{
	/* FIXME: either just say this is forbidden,
	   or think through the splitting of stacks
	   that span segment boundaries. */
	if(sz >= Segsize)
		bug();
	return __mal(MTstack, H.tg, sz);
}

static void*
curaddr(Val v)
{
	if(Vfwd(v))
		return Vfwdaddr(v);
	return v;
}

static u32
_qsz(Head *h)
{
	Cid *id;
	Closure *cl;
	Code *c;
	Str *s;
	Ctype *t;
	Vec *v;

	switch(Vkind(h)){
	case Qcid:
		id = (Cid*)h;
		return cidsize(id->len);
	case Qcl:
		cl = (Closure*)h;
		return clsize(cl->dlen);
	case Qstr:
		s = (Str*)h;
		switch(s->skind){
		case Sheap:
			return strsize(s->len);
		case Sperm:
			return sizeof(Strperm);
		}
		fatal("bug");
	case Qcode:
		c = (Code*)h;
		return c->sz;
	case Qvec:
		v = (Vec*)h;
		return vecsize(v->len);
/*
	case Qrec:
		r = (Rec*)h;
		return recsize(r->nf);
*/
	case Qctype:
		t = (Ctype*)h;
		switch(t->tkind){
		case Tvoid:
			return sizeof(Ctype);
		case Tbase:
			return sizeof(Ctypebase);
		case Tstruct:
		case Tunion:
			return sizeof(Ctypesu);
		case Tenum:
			return sizeof(Ctypeenum);
		case Ttypedef:
			return sizeof(Ctypedef);
		case Tptr:
			return sizeof(Ctypeptr);
		case Tarr:
			return sizeof(Ctypearr);
		case Tfun:
			return sizeof(Ctypefunc);
		case Tundef:
			return sizeof(Ctypeundef);
		case Tbitfield:
			return sizeof(Ctypebitfield);
		case Tconst:
			return sizeof(Ctypeconst);
		default:
			bug();
		}
	default:
		return qs[Vkind(h)].sz;
	}
	fatal("bug");
}

static u32
qsz(Head *h)
{
	return align(_qsz(h));
}

static u8
copy(Val *v)
{
	Head *h;
	Seg *t, *s, *es;
	Imm sz;
	Head *nh;
	unsigned dbg = alldbg;

	h = *v;
	if(h == 0)
		return Clean;
	if(h == Xnil)
		return Clean;
	if(Vfwd(h)){
		if(dbg) printf("copy: read fwd %p -> %p\n",
			      h, (void*)Vfwdaddr(h));
		*v = Vfwdaddr(h);
		/* it may have been moved to an older
		   generation.  so we check, in order
		   that we might get a more accurate
		   dirty card for the segment
		   containing V (and thus maybe fewer
		   scans later).  but (FIXME) it might
		   cost more to look up the
		   generation. */
		s = a2s(*v);
		return s->gen;
	}
	s = a2s(h);
	if(!MTold(s->mt))
		// this object does not move
		return s->gen;
	if(MTbig(s->mt)){
		t = s;
		es = a2s(s->e);
		while(t <= es){
			MTclrold(t->mt);
			t->gen = H.tg;
			t++;
		}
		minsert(&H.m[s->mt][H.tg], s);
		return H.tg;
	}
	sz = qsz(h);
	switch(Vkind(h)){
	case Qbox:
		nh = malbox();
		break;
	case Qpair:
		if(isweak(h))
			nh = malweak();
		else
			nh = malv(Qpair, sizeof(Pair));
		break;
	case Qcid:
	case Qcl:
	case Qcode:
	case Qctype:
	case Qstr:
	case Qvec:
	//case Qrec:
	default:
		nh = _mal(Vkind(h), sz);
		Vsetkind(nh, Vkind(h));
		break;
	}
	memcpy(nh, h, sz);
	Vsetfwd(h, (uptr)nh);
	if(dbg) printf("set fwd %p -> %p %p (%d)\n",
		    h, Vfwdaddr(h), nh, (int)Vfwd(h));
	*v = nh;
	return H.tg;
}

static void
gcopy(Val *v, u8 *min)
{
	u8 g;
	g = copy(v);
	if(g < *min)
		*min = g;
}

static u8
scan1(Head *h)
{
	u8 min;

	min = Clean;
	if(qs[Vkind(h)].scan == 0)
		return min;
	min = qs[Vkind(h)].scan(h);
	return min;
}

/* m->scan must always point to a location within the
   current segment to be scanned. */
static unsigned
scan(M *m)
{
	Head *h;
	unsigned rv;
	Seg *s;
	unsigned dbg = alldbg;

	if(m->scan == 0)
		return 0;
	s = lookseg(m->scan);
	rv = 0;
	while(1){
		if(!isliveseg(s))
			fatal("bug: mt of seg %p (%p) is %d", s, s2a(s), s->mt);
		while(m->scan < s->a){
			rv = 1;
			h = m->scan;
			if(dbg) printf("scanning %p (%s)\n", h, qs[Vkind(h)].id);
			m->scan += qsz(h);
			scan1(h);
		}
		if(s->link == 0)
			break;
		m->scan = s->link;
		s = a2s(s->link);
	}

	// approximate indication of whether a copy occurred in this call:
	//    1: maybe
	//    0: definitely not
	return rv;
}

static void
kleenescan(u32 tg)
{
	unsigned again, i;
	u64 b;
	b = usec();
	do{
		again = 0;
		for(i = 0; i < Nmt; i++){
			if(MTtag(i) == Mstack)
				/* these are scanned by copystack */
				continue;
			again |= scan(&H.m[i][tg]);
		}
	}while(again);
	stats.sweeptime += usec()-b;
}

static Pair*
lastpair()
{
	return cons(Xnil, Xnil);
}

Pair*
mkguard()
{
	Pair *p;
	p = lastpair();
	return cons(p, p);
}

static void
_instguard(Guard *g, Pair *p)
{
	g->gd[H.tg] = cons(p, g->gd[H.tg]);
}

void
instguard(Pair *p)
{
	_instguard(&H.guard, p);
}

Head*
pop1guard(Pair *g)
{
	Head *x, *y;
	if(car(g) == cdr(g))
		return 0;
	x = car(g);
	y = car(x);
	setcar(g, cdr(x));
	_setcar(x, Xnil);
	_setcdr(x, Xnil);
	return y;
}

Head*
pop1tguard(Pair *g)
{
	Head *m;
	while(1){
		m = pop1guard(g);
		if(m == 0)
			return 0;
		if(!Viskind(car(m), Qnil)){
			guard(m, g);
			return car(m);
		}
	}
}

static void
push1guard(Val o, Pair *t)
{
	Pair *p;
	p = lastpair();
	setcar(cdr(t), o);
	setcdr(cdr(t), p);
	setcdr(t, p);
}

void
guard(Val o, Pair *g)
{
	instguard(cons(o, g));
}

void
tguard(Val o, Pair *g)
{
	guard((Val)weakcons(o, Xnil), g);
}

static int
islive(Head *o)
{
	Seg *s;
	if(Vfwd(o))
		return 1;
	if(o == Xnil)
		return 1;
	s = a2s(o);
	if(!MTold(s->mt))
		return 1;
	return 0;
}

static void
updateguards(Guard *g)
{
	Head *phold, *pfinal, *p, *q, *o;
	Head *final, *w;
	u32 i;
	u64 b;

	b = usec();
	// move guarded objects and guards (and their containing cons) to
	// either pending hold or pending final list
	phold = Xnil;
	pfinal = Xnil;
	for(i = 0; i <= H.g; i++){
		p = (Head*)g->gd[i];
		g->gd[i] = (Pair*)Xnil;
		while(p != Xnil){
			q = cdr(p);
			o = caar(p);
			if(islive(o)){
				// object is accessible
				setcdr(p, phold);
				phold = p;
			}else{
				// object is inaccessible
				setcdr(p, pfinal);
				pfinal = p;
			}
			p = q;
		}
	}

	// move each pending final to final if guard is accessible
	while(1){
		final = Xnil;
		p = pfinal;
		pfinal = Xnil;
		while(p != Xnil){
			q = cdr(p);
			o = cdar(p);
			if(islive(o)){
				// guard is accessible
				setcdr(p, final);
				final = p;
			}else{
				setcdr(p, pfinal);
				pfinal = p;
			}
			p = q;
		}
		if(final == Xnil){
			p = pfinal;
			while(p != Xnil){
				_setcar(p, Xnil);
				p = cdr(p);
			}
			break;
		}
		w = final;
		while(w != Xnil){
			copy(&caar(w));
			push1guard(caar(w), curaddr(cdar(w)));
			_setcar(w, Xnil);
			w = cdr(w);
		}
		kleenescan(H.tg);
	}

	// forward pending hold to fresh guarded list
	p = phold;
	while(p != Xnil){
		o = cdar(p);
		if(islive(o))
			_instguard(g, cons(curaddr(caar(p)), curaddr(cdar(p))));
		_setcar(p, Xnil);
		p = cdr(p);
	}
	stats.guardtime += usec()-b;
}

void
gcenable()
{
	H.disable--;
}

void
gcdisable()
{
	H.disable++;
}

void
gcpoll(VM *vm)
{
	static int ingc;
	if(!H.disable && !ingc
	   && (H.na >= H.ma || H.free < LORES*H.inuse)){
		ingc++;
		gc(vm);
		ingc--;
	}
}

void
gcwb(Val v)
{
	Seg *s;
	s = a2s((Head*)v);
	s->card[card(v)] = Dirty;
}

static void
copykstack(Cont *k, u8 *min)
{
	copystack(&k->base, k->sz, &k->ra, k->cl, k->sz, min);
}

static void
copystack(void **basep, u32 stxsz, void **rap, Closure *cl, u32 fpo, u8 *min)
{
	void *p, *ra, *base;
	Seg *s;
	Imm i;
	Val *fp, *lp;
	u64 sz, lm, *mp, o;
	uptr coff;
	Code *cp;

	/*
	  if stack is in oldspace, move it.
	  no need to check for big (not permitted on stacks).
	*/
	base = *basep;
	s = a2s(base);
	if(MTold(s->mt)){
		p = malstack(stxsz);
		memcpy(p, base, stxsz);
		base = p;
		*basep = base;
	}

	ra = *rap;
	fp = base+fpo;
	while((void*)fp > base){
		cp = cl->code;
		sz = ra2size(ra, cp);
		lm = ra2mask(ra, cp);
		fp -= sz;	/* beginning of previous frame */

		if((void*)fp < base)
			bug();
		if((void*)fp >= base+stxsz)
			bug();

		/* get OLD pointer to closure, the one
		   that still references the code object
		   in which fp[Ora] points. */
		cl = valcl(fp[Ocl]);

		/* copy locations in live mask */
		lp = fp;
		if((lm>>(mwbits-1))&1){
			o = lm&~(1ULL<<(mwbits-1));
			mp = (u64*)strdata(cp->lm)+o;
			while(sz > 0){
				lm = *mp++;
				for(i = 0;
				    sz > 0 && i < mwbits;
				    i++, sz--, lp++)
					if((lm>>i)&1)
						gcopy(lp, min);
			}
		}else
			for(i = 0; i < sz; i++, lp++)
				if((lm>>i)&1)
					gcopy(lp, min);

		/* copy code and update ra */
		if(cp->kind == Cvm){
			coff = (void*)ra-(void*)cp;
			gcopy((Val*)&cp, min);
			ra = (void*)cp+coff;
			*rap = ra;
		}

		rap = (void**)(fp+Ora);
		ra = *rap;
	}

	if(fp != base)
		bug();
	if(cl == 0)
		bug();

	/* copy ra at base of stack if any */
	cp = cl->code;
	if(cp->kind == Cvm){
		coff = (void*)ra-(void*)cp;
		gcopy((Val*)&cp, min);
		ra = (void*)cp+coff;
		*rap = ra;
	}
}

static void
mark1old(Seg *s, u32 g)
{
	if(s->gen <= g)
		MTsetold(s->mt);
}

static void
markold(u32 g)
{
	Seg *s, *es;
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			mark1old(s, g);
		s = nextseg(s);
	}
}

static void
docards(Seg *s, void *p, void *e)
{
	u8 min, g, c;
	u32 sz;

	if(p >= e)
		fatal("bug");

	stats.ncard++;
	min = Clean;
	c = card(p);
	while(p < e){
		if(card(p) != c){
			/* crossed into next card */
			s->card[c] = min < s->gen ? min : Clean;
			c = card(p);
			min = Clean;
		}
		g = scan1(p);
		if(g < min)
			min = g;
		sz = qsz(p);
		p += sz;
		if(sz > Cardsize)
			stats.multicardscan++;
	}
	s->card[c] = min < s->gen ? min : Clean;
}

/* FIXME: is the real predicate here that the segment is not old? */
static void
scan1segcards(Seg *s, u32 g)
{
	int i, k;
	void *p;
	if(s->gen <= g)
		return;
	if(s->mt == MTstack)
		/* FIXME: this is a stupid special
		   case.  shouldn't it be the case that
		   cards are never set for stack
		   segments? */
		return;
	p = s2a(s);
	for(i = Ncard-1; i >= 0; i--){
		if(s->card[i] > g)
			continue;
		k = i;
		while(s->crossing[i])
			i--;
		stats.crossings[k-i]++;
		docards(s, p+i*Cardsize, min(p+(k+1)*Cardsize, s->a));
	}
}

static void
scancards(u32 g)
{
	Seg *s, *es;
	u64 b;
	b = usec();
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			scan1segcards(s, g);
		s = nextseg(s);
	}
	stats.cardtime += usec()-b;
}

static void
updateweak(u32 tg)
{
	M *m;
	Seg *s;
	Pair *p;
	u64 b;

	b = usec();
	m = &H.m[MTweak][tg];
	s = lookseg(m->h);
	while(1){
		p = s2a(s);
		while((void*)p < s->a){
			if(Vfwd(car(p)))
				_setcar(p, Vfwdaddr(car(p)));
			else if(!islive(car(p)))
				_setcar(p, Xnil);
			p++;
		}
		if(s->link == 0)
			break;
		s = a2s(s->link);
	}
	stats.weaktime += usec()-b;
}

static void
recycle1(Seg *s)
{
	if(!MTold(s->mt))
		return;
	if(MTbig(s->mt))
		freebigseg(s);
	else
		freeseg(s);
}

static void
recycle()
{
	Seg *s, *t, *es;
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		t = nextseg(s);
		if(isliveseg(s))
			recycle1(s);
		s = t;
	}
}

static void
resetalloc(MT mt, Gen g)
{
	minit(&H.m[mt][g], allocseg(mt, g));
}

static void
getalloc(MT mt, Gen g)
{
	if(H.m[mt][g].h == 0)
		resetalloc(mt, g);
	else
		minsert(&H.m[mt][g], allocseg(mt, g));
}

/*
	for generations 0...g copy live data to space for generation tg.
	if tg is g then create a new target space;
        else if tg is g+1 then use existing target space tg;
	else error.
*/
void
_gc(u32 g, u32 tg)
{
	u32 i, mt;
	VM **vmp, *vm;
	unsigned dbg = alldbg;
	u64 b;
	void *ra;
	Closure *cl;
	u32 fpo;
	Imm fsz;
	u8 min;

//	memset(&stats, 0, sizeof(stats));
	b = usec();
	maintain();
	H.ingc++;
	if(g != tg && g != tg-1)
		fatal("bug");
	if(tg >= Nsgen)
		return; // FIXME: silently do nothing...caller should know

	H.g = g;
	H.tg = tg;
	if(dbg) printf("gc(%u, %u)\n", g, tg);
	stats.inittime += usec()-b;

	b = usec();
	markold(g);
	for(i = 0; i <= g; i++)
		for(mt = 0; mt < Nmt; mt++)
			mclr(&H.m[mt][i]);

	if(g == tg){
		resetalloc(MTdata, tg);
		resetalloc(MTcode, tg);
		resetalloc(MTstack, tg);
		resetalloc(MTweak, tg);
		resetalloc(MTbox, tg);
		resetalloc(MTmutable, tg);
	}else{
		getalloc(MTdata, tg);
		getalloc(MTcode, tg);
		getalloc(MTstack, tg);
		getalloc(MTweak, tg);
		getalloc(MTbox, tg);
		getalloc(MTmutable, tg);
	}
	stats.oldtime += usec()-b;

	vmp = vms;
	while(vmp < vms+nvm){
		vm = *vmp++;
		switch(vm->cl->code->kind){
		case Cvm:
			/* e.g., via gcpoll insn in VM code */
			fsz = ra2size(vm->pc, vm->cl->code);
			vm->fp += fsz;
			fpo = (void*)vm->fp-vm->stk;
			min = Clean;
			copystack(&vm->stk, vm->stksz, (void**)&vm->pc, vm->cl, fpo, &min);
			copy((Val*)&vm->cl);
			copy((Val*)&vm->klink);
			vm->fp = vm->stk+fpo;
			vm->fp -= fsz;
			if((void*)vm->pc < codeinsn(curaddr(mkvalcode(vm->cl->code))))
				bug();
			if((void*)vm->pc >= codeend(curaddr(mkvalcode(vm->cl->code))))
				bug();
			break;
		case Ccfn:
		case Cccl:
			/* e.g., via C builtin like gc() */
			ra = vm->fp[Ora];
			cl = valcl(vm->fp[Ocl]);
			fpo = (void*)vm->fp-vm->stk;
			min = Clean;
			copystack(&vm->stk, vm->stksz, &ra, cl, fpo, &min);
			copy((Val*)&vm->cl);
			copy((Val*)&vm->klink);
			copy((Val*)&cl);
			vm->fp = vm->stk+fpo;
			vm->fp[Ora] = (Val)(uptr)ra;
			vm->fp[Ocl] = mkvalcl(cl);
			break;
		default:
			bug();
		}

		copy((Val*)&vm->top);
		copy(&vm->ac);
	}
	if(dbg) printf("copied vm roots\n");

	/* global roots */
	copy(&syms);
	copy(&typecache);
	copy((Val*)&litdom);
	copy((Val*)&cvalnull);
	copy((Val*)&cval0);
	copy((Val*)&cval1);
	copy((Val*)&cvalminus1);
	copy((Val*)&vabort);
	copy((Val*)&halt);
	copy((Val*)&stkunderflow);
	stats.roottime += usec()-b;

	scancards(g);
	if(dbg) printf("scanned cards\n");
	kleenescan(tg);
	if(dbg) printf("re-scanned tg data (after prot)\n");

	updateguards(&H.guard);
	if(dbg) printf("did updateguards\n");
	updateweak(tg);

	b = usec();
	recycle();
	if(dbg) printf("did recycle\n");
	stats.recycletime += usec()-b;

	b = usec();
	if(H.tg != 0){
		H.tg = 0;
		resetalloc(MTdata, H.tg);
		resetalloc(MTcode, H.tg);
		resetalloc(MTstack, H.tg);
		resetalloc(MTweak, H.tg);
		resetalloc(MTbox, H.tg);
		resetalloc(MTmutable, H.tg);
	}
	H.na = 0;
	if(dbg) printf("end of collection\n");
	H.ingc--;
	stats.resettime += usec()-b;
	b = usec();
	maintain();
	stats.finimainttime += usec()-b;
	if(dbg) printf("gc returning\n");
}

void
gc(VM *vm)
{
	int i;
	u32 g, tg;

	/* alas, gc is unsafe when C contexts are active */
	if(vm->level > 1)
		return;

	H.gctrip++;
	g = -1;
	for(i = Ngen-1; i >= 0; i--)
		if(H.gctrip%H.gcsched[i] == 0){
			g = i;
			break;
		}
	if(g == Ngen-1)
		tg = g;
	else
		tg = g+1;
	dogc(vm, g, tg);
}

void
fullgc(VM *vm)
{
	/* alas, gc is unsafe when C contexts are active */
	if(vm->level > 1)
		return;
	dogc(vm, Ngen-1, Ngen-1);
}

void
compact(VM *vm)
{
	dogc(vm, Ngen-1, Gstatic);
}

int
ismanagedrange(void *p, Imm len)
{
	Seg *s;
	void *e;

	if(p+len <= p)
		/* FIXME: bad output for bad input */
		return 1;

	e = (void*)roundup(p+len, Segsize);
	p = (void*)rounddown(p, Segsize);
	if(e <= segmap.lo)
		return 0;
	if(p >= segmap.hi)
		return 0;
	while(p < e){
		if(p >= segmap.lo && p < segmap.hi){
			s = a2s(p);
			if(s->mt != MThole)
				return 1;
		}
		p += Segsize;
	}
	return 0;
}

void
initmem()
{
	u32 i, gr;
	memset(&H, 0, sizeof(H));
	ncfn = 0;
	initsegmap();
	gr = 1;
	for(i = 0; i < Ngen; i++){
		H.gcsched[i] = gr;
		gr *= GCradix;
	}
	resetalloc(MTdata, 0);
	resetalloc(MTcode, 0);
	resetalloc(MTstack, 0);
	resetalloc(MTweak, 0);
	resetalloc(MTbox, 0);
	resetalloc(MTmutable, 0);
	H.na = 0;
	H.ma = GCthresh;

	/* we need nil now to initialize the guarded object lists */
	Vsetkind(&constnil, Qnil);
	Xnil = &constnil;

	for(i = 0; i < Nsgen; i++)
		H.guard.gd[i] = (Pair*)Xnil;
	H.disable = 0;
}

void
resetmem()
{
	u8 mt, g;
	u32 i;
	Seg *s, *es;
	u64 nseg;

	for(i = 0; i < Nsgen; i++)
		H.guard.gd[i] = (Pair*)Xnil;

	for(mt = 0; mt < Nmt; mt++)
		for(g = 0; g < Nsgen; g++)
			mclr(&H.m[mt][g]);

	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	nseg = es-s;
	while(s < es){
		if(s->mt == MThole){
			s++;
			continue;
		}
		unmapmem(s2a(s), Segsize);
		s++;
	}
	freesegmap(segmap.map, nseg);
	segmap.lo = 0;
	segmap.hi = 0;
	segmap.map = 0;
	segmap.free = 0;
	H.na = 0;
	H.ma = GCthresh;
	H.heapsz = 0;
	H.free = 0;
	H.inuse = 0;
	H.bigsz = 0;
}

void
finimem()
{
	_gc(Ngen-1, Ngen-1);
	/* FIXME: free all segments */
	/* FIXME: free static generation */
}


static void
gcstat1(MT mt, u64 *ns, u64 *nd)
{
	Gen g;
	for(g = G0; g <= Gstatic; g++){
		if(g == Nsgen)
			continue;
		printf("%10s %3s %8" PRIu64 " %8" PRIu64 "\n",
		       MTname[mt],
		       genname[g],
		       ns[g],
		       nd[g]);
	}
}

void
gcstats()
{
	Seg *s, *es;
	u64 ns[Nmt][Allgen];
	u64 nd[Nmt][Allgen];

	memset(ns, 0, sizeof(ns));
	memset(nd, 0, sizeof(ns));

	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		ns[s->mt][s->gen]++;
		if(isliveseg(s)){
#if 0
			if(s->card < s->gen)
				nd[s->mt][s->gen]++;
#endif
		}
		s = nextseg(s);
	}

	printf("%10s %3s %8s %8s\n",
	       "TYPE", "GEN", "#SEG", "#DIRTY");

	gcstat1(MTfree, ns[MTfree], nd[MTfree]);
	gcstat1(MTdata, ns[MTdata], nd[MTdata]);
	gcstat1(MTcode, ns[MTcode], nd[MTcode]);
	gcstat1(MTweak, ns[MTweak], nd[MTweak]);
	gcstat1(MTbox, ns[MTbox], nd[MTbox]);
	gcstat1(MTmutable, ns[MTmutable], nd[MTmutable]);
	gcstat1(MTbigdata, ns[MTbigdata], nd[MTbigdata]);
	gcstat1(MTbigcode, ns[MTbigcode], nd[MTbigcode]);

	printf(" inuse = %10" PRIu64 "\n", H.inuse);
	printf("  free = %10" PRIu64 "\n", H.free);
	printf("heapsz = %10" PRIu64 "\n", H.heapsz);
	printf(" bigsz = %10" PRIu64 "\n", H.bigsz);
	printf("smapsz = %10" PRIu64 "\n", H.smapsz);
	printf(" ncard = %10" PRIu32 "\n", stats.ncard);
	printf("   ngc = %10" PRIu32 "\n", H.gctrip);
}

void
gcstatistics(Tab *t)
{
	tabput(t, mkvalcid(mkcid0("inuse")),
	       mkvallitcval(Vuvlong, H.inuse));
	tabput(t, mkvalcid(mkcid0("free")),
	       mkvallitcval(Vuvlong, H.free));
	tabput(t, mkvalcid(mkcid0("heapsz")),
	       mkvallitcval(Vuvlong, H.heapsz));
	tabput(t, mkvalcid(mkcid0("bigsz")),
	       mkvallitcval(Vuvlong, H.bigsz));
	tabput(t, mkvalcid(mkcid0("smapsz")),
	       mkvallitcval(Vuvlong, H.smapsz));
	tabput(t, mkvalcid(mkcid0("collecttrip")),
	       mkvallitcval(Vuvlong, H.gctrip));
	tabput(t, mkvalcid(mkcid0("cardtime")),
	       mkvallitcval(Vuvlong, stats.cardtime));
	tabput(t, mkvalcid(mkcid0("roottime")),
	       mkvallitcval(Vuvlong, stats.roottime));
	tabput(t, mkvalcid(mkcid0("sweeptime")),
	       mkvallitcval(Vuvlong, stats.sweeptime));
	tabput(t, mkvalcid(mkcid0("guardtime")),
	       mkvallitcval(Vuvlong, stats.guardtime));
	tabput(t, mkvalcid(mkcid0("weaktime")),
	       mkvallitcval(Vuvlong, stats.weaktime));
	tabput(t, mkvalcid(mkcid0("oldtime")),
	       mkvallitcval(Vuvlong, stats.oldtime));
	tabput(t, mkvalcid(mkcid0("ncard")),
	       mkvallitcval(Vuvlong, stats.ncard));
	tabput(t, mkvalcid(mkcid0("inittime")),
	       mkvallitcval(Vuvlong, stats.inittime));
	tabput(t, mkvalcid(mkcid0("recycletime")),
	       mkvallitcval(Vuvlong, stats.recycletime));
	tabput(t, mkvalcid(mkcid0("resettime")),
	       mkvallitcval(Vuvlong, stats.resettime));
	tabput(t, mkvalcid(mkcid0("finimainttime")),
	       mkvallitcval(Vuvlong, stats.finimainttime));
	tabput(t, mkvalcid(mkcid0("crossings0")),
	       mkvallitcval(Vuvlong, stats.crossings[0]));
	tabput(t, mkvalcid(mkcid0("crossings1")),
	       mkvallitcval(Vuvlong, stats.crossings[1]));
	tabput(t, mkvalcid(mkcid0("crossings2")),
	       mkvallitcval(Vuvlong, stats.crossings[2]));
	tabput(t, mkvalcid(mkcid0("crossings3")),
	       mkvallitcval(Vuvlong, stats.crossings[3]));
	tabput(t, mkvalcid(mkcid0("crossings4")),
	       mkvallitcval(Vuvlong, stats.crossings[4]));
	tabput(t, mkvalcid(mkcid0("crossings5")),
	       mkvallitcval(Vuvlong, stats.crossings[5]));
	tabput(t, mkvalcid(mkcid0("crossings6")),
	       mkvallitcval(Vuvlong, stats.crossings[6]));
	tabput(t, mkvalcid(mkcid0("crossings7")),
	       mkvallitcval(Vuvlong, stats.crossings[7]));
	tabput(t, mkvalcid(mkcid0("multicardscan")),
	       mkvallitcval(Vuvlong, stats.multicardscan));
}

void
registercfn(char *name, void *addr)
{
	if(ncfn >= Maxcfnrec)
		bug();
	cfn[ncfn].name = name;
	cfn[ncfn].addr = addr;
	ncfn++;
}

static u32
lookcfnaddr(void *addr)
{
	u32 i;
	for(i = 0; i < ncfn; i++)
		if(cfn[i].addr == addr)
			return i;
	bug();
}

static u32
lookcfnname(char *name)
{
	u32 i;
	for(i = 0; i < ncfn; i++)
		if(!strcmp(cfn[i].name, name))
			return i;
	bug();
}

enum{
	Segx = (((uptr)-1)>>Segbits),
	Segconst = Segx,
	Segcfn   = Segx-1,
};

static uptr
mkreloc(uptr n, uptr off)
{
	switch(n){
	case Segconst:
	case Segcfn:
		return (n<<Segbits)|(off&(Segsize-1));
	default:
		if(n == 0 && off == 0)
			printf("jackpot!\n");
		return (n<<Segbits)|(off&(Segsize-1));
	}
}

static void*
resolveptr(uptr x, LSctx *ls)
{
	uptr n;
	uptr off;
	u32 idx;

	if(ls->mode != LSload)
		bug();
	if(x==0)
		return 0;
	n = x>>Segbits;
	off = x&(Segsize-1);
	switch(n){
	case Segconst:
		switch(off){
		case 0:
			return 0;
		case 1:
			return Xnil;
		default:
			bug();
		}
		break;
	case Segcfn:
		if(off > ls->ncsym)
			bug();
		idx = lookcfnname(ls->csym[off]);
		return cfn[idx].addr;
	default:
		if(n >= ls->nseg)
			bug();
		return ls->top+n*Segsize+off;
	}
}

static void
loadsaveptr(Val *pp, LSctx *ls)
{
	Val p;
	Seg *s;

	p = *pp;
	switch(ls->mode){
	case LSsave:
		if(p == 0)
			*pp = (Val)mkreloc(Segconst, 0);
		else if(p == Xnil)
			*pp = (Val)mkreloc(Segconst, 1);
		else{
			s = a2s(p);
			if(s == 0)
				bug();
			*pp = (Val)mkreloc(s->n, (uptr)p);
		}
		break;
	case LSload:
		*pp = resolveptr((uptr)p, ls);
		break;
	default:
		bug();
	}
}

static void
savecfn(void **fnp)
{
	if (*fnp)
		*fnp = (void*)mkreloc(Segcfn, lookcfnaddr(*fnp));
}

static u8
loadsaveas(Head *hd, LSctx *ls)
{
	As *as;
	as = (As*)hd;
	loadsaveptr((Val*)&as->mtab, ls);
	loadsaveptr((Val*)&as->name, ls);
	loadsaveptr((Val*)&as->get, ls);
	loadsaveptr((Val*)&as->put, ls);
	loadsaveptr((Val*)&as->ismapped, ls);
	loadsaveptr((Val*)&as->map, ls);
	loadsaveptr((Val*)&as->dispatch, ls);
	return 0;
}

static u8
loadsavebox(Head *hd, LSctx *ls)
{
	Box *box;
	box = (Box*)hd;
	loadsaveptr((Val*)&box->v, ls);
	return 0;
}

static u8
loadsavecl(Head *hd, LSctx *ls)
{
	unsigned i;
	Closure *cl;
	cl = (Closure*)hd;
	loadsaveptr((Val*)&cl->code, ls);
	for(i = 0; i < cl->dlen; i++)
		loadsaveptr(&cldisp(cl)[i], ls);
	return 0;
}

static u8
loadsavecode(Head *h, LSctx *ls)
{
	Imm i;
	Reloc *r;
	void **cp;
	void *p;
	Code *c;

	c = (Code*)h;
	loadsaveptr((Val*)&c->id, ls);
	if(ls->mode == LSsave){
		if(c->reloc)
			r = (Reloc*)strdata(c->reloc);
		else
			r = 0;
		loadsaveptr((Val*)&c->reloc, ls);
	}else{
		loadsaveptr((Val*)&c->reloc, ls);
		if(c->reloc)
			r = (Reloc*)strdata(c->reloc);
		else
			r = 0;
	}
	loadsaveptr((Val*)&c->lm, ls);
	loadsaveptr((Val*)&c->dbg, ls);
	loadsaveptr((Val*)&c->src, ls);
	loadsaveptr((Val*)&c->lex, ls);
	switch(c->kind){
	case Calien:
		loadsaveptr(&c->alien, ls);
		break;
	case Ccfn:
		if(ls->mode == LSsave)
			savecfn((void**)&c->cfn);
		else
			c->cfn = resolveptr((uptr)c->cfn, ls);
		break;
	case Cccl:
		if(ls->mode == LSsave)
			savecfn((void**)&c->ccl);
		else
			c->ccl = resolveptr((uptr)c->ccl, ls);
		break;
	case Cvm:
		if(ls->mode == LSload)
			setgo(c);
		break;
	case Cnative:
		bug();
		break;
	}
	if(r == 0)
		return 0;
	p = c+1;
	for(i = 0; i < c->nreloc; i++, r++){
		cp = (void**)(p+r->coff);
		loadsaveptr((Val*)cp, ls);
	}
	return 0;
}

/* order matters: copy the stack before the closure. */
static u8
loadsavecont(Head *hd, LSctx *ls)
{
	Cont *k;
	k = (Cont*)hd;
	k->link = 0;
	k->base = 0;
	k->cl = 0;
//	gsave((Val*)&k->link);
//	copykstack(k, &min);
//	gsave((Val*)&k->cl);
	return 0;
}

static u8
loadsaveprecode(Head *hd, LSctx *ls)
{
	Precode *c;
	c = (Precode*)hd;
	loadsaveptr((Val*)&c->id, ls);
	loadsaveptr((Val*)&c->lm, ls);
	loadsaveptr((Val*)&c->dbg, ls);
	loadsaveptr((Val*)&c->src, ls);
	return 0;
}

static u8
loadsavectype(Head *hd, LSctx *ls)
{
	Ctype *t;
	Ctypearr *ta;
	Ctypeconst *tc;
	Ctypedef *td;
	Ctypeenum *te;
	Ctypefunc *tf;
	Ctypesu *ts;
	Ctypeptr *tp;
	Ctypeundef *tu;
	Ctypebitfield *tw;

	t = (Ctype*)hd;
	switch(t->tkind){
	case Tvoid:
	case Tbase:
		break;
	case Tstruct:
	case Tunion:
		ts = (Ctypesu*)t;
		loadsaveptr((Val*)&ts->tag, ls);
		loadsaveptr((Val*)&ts->field, ls);
		loadsaveptr(&ts->attr, ls);
		break;
	case Tenum:
		te = (Ctypeenum*)t;
		loadsaveptr((Val*)&te->tag, ls);
		loadsaveptr((Val*)&te->sub, ls);
		loadsaveptr((Val*)&te->konst, ls);
		break;
	case Ttypedef:
		td = (Ctypedef*)t;
		loadsaveptr((Val*)&td->sub, ls);
		loadsaveptr((Val*)&td->tid, ls);
		break;
	case Tundef:
		tu = (Ctypeundef*)t;
		loadsaveptr((Val*)&tu->sub, ls);
		break;
	case Tconst:
		tc = (Ctypeconst*)t;
		loadsaveptr((Val*)&tc->sub, ls);
		break;
	case Tptr:
		tp = (Ctypeptr*)t;
		loadsaveptr((Val*)&tp->sub, ls);
		break;
	case Tarr:
		ta = (Ctypearr*)t;
		loadsaveptr(&ta->cnt, ls);
		loadsaveptr((Val*)&ta->sub, ls);
		break;
	case Tfun:
		tf = (Ctypefunc*)t;
		loadsaveptr((Val*)&tf->sub, ls);
		loadsaveptr((Val*)&tf->param, ls);
		break;
	case Tbitfield:
		tw = (Ctypebitfield*)t;
		loadsaveptr(&tw->cnt, ls);
		loadsaveptr(&tw->bit0, ls);
		loadsaveptr((Val*)&tw->sub, ls);
		break;
	default:
		bug();
	}
	return 0;
}

static u8
loadsavecval(Head *hd, LSctx *ls)
{
	Cval *cval;
	cval = (Cval*)hd;
	loadsaveptr((Val*)&cval->dom, ls);
	loadsaveptr((Val*)&cval->type, ls);
	return 0;
}

static u8
loadsavedom(Head *hd, LSctx *ls)
{
	Dom *dom;
	dom = (Dom*)hd;
	loadsaveptr((Val*)&dom->as, ls);
	loadsaveptr((Val*)&dom->ns, ls);
	loadsaveptr((Val*)&dom->name, ls);
	return 0;
}

static u8
loadsaveexpr(Head *hd, LSctx *ls)
{
	Expr *e;
	e = (Expr*)hd;
	loadsaveptr((Val*)&e->e1, ls);
	loadsaveptr((Val*)&e->e2, ls);
	loadsaveptr((Val*)&e->e3, ls);
	loadsaveptr((Val*)&e->e4, ls);
	loadsaveptr((Val*)&e->aux, ls);
	loadsaveptr((Val*)&e->skind, ls);
	loadsaveptr((Val*)&e->src, ls);
	return 0;
}

static u8
loadsavelist(Head *hd, LSctx *ls)
{
	List *l;
	l = (List*)hd;
	loadsaveptr((Val*)&l->v, ls);
	return 0;
}

static u8
loadsavens(Head *hd, LSctx *ls)
{
	Ns *ns;
	unsigned i;
	ns = (Ns*)hd;
	loadsaveptr((Val*)&ns->lookaddr, ls);
	loadsaveptr((Val*)&ns->looksym, ls);
	loadsaveptr((Val*)&ns->looktype, ls);
	loadsaveptr((Val*)&ns->enumtype, ls);
	loadsaveptr((Val*)&ns->enumsym, ls);
	loadsaveptr((Val*)&ns->name, ls);
	loadsaveptr((Val*)&ns->dispatch, ls);
	loadsaveptr((Val*)&ns->mtab, ls);
	for(i = 0; i < Vnallbase; i++)
		loadsaveptr((Val*)&ns->base[i], ls);
	return 0;
}

static u8
loadsavepair(Head *hd, LSctx *ls)
{
	Pair *pair;
	pair = (Pair*)hd;
	loadsaveptr(&pair->cdr, ls);
	loadsaveptr(&pair->car, ls);
	return 0;
}

static u8
loadsaverange(Head *hd, LSctx *ls)
{
	Range *range;
	range = (Range*)hd;
	loadsaveptr((Val*)&range->beg, ls);
	loadsaveptr((Val*)&range->len, ls);
	return 0;
}

static u8
loadsavetab(Head *hd, LSctx *ls)
{
	Tab *t;
	t = (Tab*)hd;
	loadsaveptr((Val*)&t->ht, ls);
	loadsaveptr((Val*)&t->tg, ls);
	loadsaveptr((Val*)&t->def, ls);
	loadsaveptr((Val*)&t->name, ls);
	loadsaveptr((Val*)&t->fmt, ls);
	if(ls->mode == LSsave){
		savecfn((void**)&t->equal);
		savecfn((void**)&t->hash);
	}else{
		t->equal = resolveptr((uptr)t->equal, ls);
		t->hash = resolveptr((uptr)t->hash, ls);
	}
	return 0;
}

static u8
loadsavevec(Head *hd, LSctx *ls)
{
	Vec *vec;
	Imm i;
	vec = (Vec*)hd;
	for(i = 0; i < vec->len; i++)
		loadsaveptr(&vecdata(vec)[i], ls);
	return 0;
}

static void
savesegment(void *p, uptr m)
{
	void *e;
	Head *h;
	LSctx ls;

	ls.mode = LSsave;
	e = p+m;
	while(p < e){
		h = p;
		if(qs[Vkind(h)].loadsave)
			qs[Vkind(h)].loadsave(h, &ls);
		p += qsz(h);
	}
}

static void
saveseg(void *p, Seg *s)
{
	savesegment(p, s->a-s2a(s));
}

static int
saveroot(int fd, Val p)
{
	Seg *s;
	uptr x;

	if(p == 0)
		bug();
	else if(p == Xnil)
		x = mkreloc(Segconst, 1);
	else{
		s = a2s(p);
		if(s == 0)
			bug();
		x = mkreloc(s->n, (uptr)p);
	}
	if(-1 == xwrite(fd, &x, sizeof(uptr)))
		return -1;
	return 0;
}

int
segsummary(int fd, Seg *s)
{
	void *p;
	uptr d;

	p = s2a(s);
	if(-1 == xwrite(fd, &s->mt, sizeof(u8)))
		goto fail;
	if(-1 == xwrite(fd, &s->gen, sizeof(u8)))
		goto fail;
	d = s->a-p;
	if(-1 == xwrite(fd, &d, sizeof(uptr)))
		goto fail;
	if(-1 == xwrite(fd, s->card, Ncard*sizeof(u8)))
		goto fail;
	if(-1 == xwrite(fd, s->crossing, Ncard*sizeof(u8)))
		goto fail;

#if 0
	printf("save %10s %d %8x",
	       MTname[s->mt],
	       s->gen,
	       d);
	printf(" card");
	for(i = 0; i < Ncard; i++)
		printf(" %2x", s->card[i]);
	printf(" crossing");
	for(i = 0; i < Ncard; i++)
		printf(" %2x", s->crossing[i]);
	printf("\n");
#endif
	return 0;
fail:
	fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
	return -1;
}

static void
loadseg(void *p, LSctx *ls)
{
	Seg *s;
	void *e;
	Head *h;

	s = a2s(p);
	e = s->a;
	while(p < e){
		h = p;
		if(qs[Vkind(h)].loadsave)
			qs[Vkind(h)].loadsave(h, ls);
		p += qsz(h);
	}
}

static void
rehash(void *p)
{
	Seg *s;
	void *e;
	Head *h;

	s = a2s(p);
	e = s->a;
	while(p < e){
		h = p;
		if(Viskind(h, Qtab))
			tabrehash(valtab(h));
		p += qsz(h);
	}
}

static int
skipmt(unsigned mt)
{
	switch(MTtag(mt)){
	case Mdata:
	case Mcode:
	case Mweak:
	case Mbox:
	case Mmutable:
		return 0;
	default:
		return 1;
	}
}

/* this function maps and seeks fd. no pipes or sockets */
int
saveheapfd(Tab *toplevel, int fd)
{
	u8 mt, g;
	u32 i, sn, nsp, sz;
	M *m;
	Seg *s, *t;
	int err;
	off_t off;
	void *op, *p;
	u8 magic;

	op = p = 0;

	/* number all segments */
	sn = 0;
	for(mt = 0; mt < Nmt; mt++){
		if(skipmt(mt))
			continue;
		for(g = 0; g < Nsgen; g++){
			m = &H.m[mt][g];
			if(m->h == 0)
				continue;
			s = a2s(m->h);
			while(1){
				s->n = sn++;
				if(MTbig(mt)){
					t = s+1;
					while(MTbigcont(t->mt)){
						t->n = sn++;
						t++;
					}
				}
				if(s->link == 0)
					break;
				s = a2s(s->link);
			}
		}
	}

	/* meta */
	magic = Heapdefined;
	if(-1 == xwrite(fd, &magic, sizeof(u8))){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	if(-1 == xwrite(fd, &heapversion, sizeof(u32))){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	sz = strlen(sysos)+1;
	if(-1 == xwrite(fd, &sz, sizeof(u32))){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	if(-1 == xwrite(fd, sysos, sz)){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	sz = strlen(sysarch)+1;
	if(-1 == xwrite(fd, &sz, sizeof(u32))){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	if(-1 == xwrite(fd, sysarch, sz)){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}

	/* segment descriptors */
	if(-1 == xwrite(fd, &sn, sizeof(u32))){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	for(mt = 0; mt < Nmt; mt++){
		if(skipmt(mt))
			continue;
		for(g = 0; g < Nsgen; g++){
			m = &H.m[mt][g];
			if(m->h == 0)
				continue;
			s = a2s(m->h);
			while(1){
				if(0 > segsummary(fd, s))
					goto fail;
				if(MTbig(mt)){
					t = s+1;
					while(MTbigcont(t->mt)){
						if(0 > segsummary(fd, t))
							goto fail;
						t++;
					}
				}
				if(s->link == 0)
					break;
				s = a2s(s->link);
			}
		}
	}

	/* scan pointers */
	nsp = 0;
	for(mt = 0; mt < Nmt; mt++){
		if(skipmt(mt))
			continue;
		for(g = 0; g < Nsgen; g++){
			m = &H.m[mt][g];
			if(m->h == 0)
				continue;
			nsp++;
		}
	}
	if(-1 == xwrite(fd, &nsp, sizeof(u32))){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	for(mt = 0; mt < Nmt; mt++){
		if(skipmt(mt))
			continue;
		for(g = 0; g < Nsgen; g++){
			m = &H.m[mt][g];
			if(m->h == 0)
				continue;
			if(-1 == xwrite(fd, &mt, sizeof(u8))){
				fprintf(stderr, "saveheapfd: write: %s\n",
					strerror(errno));
				goto fail;
			}
			if(-1 == xwrite(fd, &g, sizeof(u8))){
				fprintf(stderr, "saveheapfd: write: %s\n",
					strerror(errno));
				goto fail;
			}
			if(-1 == saveroot(fd, (Val)m->scan)){
				fprintf(stderr, "saveheapfd: write: %s\n",
					strerror(errno));
				goto fail;
			}
		}
	}

	/* roots */
	err = 0;
	err |= saveroot(fd, (Val)syms);
	err |= saveroot(fd, (Val)typecache);
	err |= saveroot(fd, (Val)litdom);
	err |= saveroot(fd, (Val)cvalnull);
	err |= saveroot(fd, (Val)cval0);
	err |= saveroot(fd, (Val)cval1);
	err |= saveroot(fd, (Val)cvalminus1);
	err |= saveroot(fd, (Val)vabort);
	err |= saveroot(fd, (Val)halt);
	err |= saveroot(fd, (Val)stkunderflow);
	err |= saveroot(fd, (Val)toplevel);
	for(i = 0; i < Nsgen; i++)
		err |= saveroot(fd, (Val)H.guard.gd[i]);
	if(err != 0){
		fprintf(stderr, "saveheapfd: saveroot: %s\n", strerror(errno));
		goto fail;
	}

	/* C symbols */
	if(-1 == xwrite(fd, &ncfn, sizeof(u32))){
		fprintf(stderr, "saveheapfd: write: %s\n", strerror(errno));
		goto fail;
	}
	for(i = 0; i < ncfn; i++)
		if(-1 == xwrite(fd, cfn[i].name, strlen(cfn[i].name)+1)){
			fprintf(stderr, "saveheapfd: write: %s\n",
				strerror(errno));
			goto fail;
		}

	off = lseek(fd, 0, SEEK_CUR);
	if(off == -1){
		fprintf(stderr, "saveheapfd: lseek: %s\n", strerror(errno));
		goto fail;
	}
	off = lseek(fd, roundup(off, Segsize), SEEK_SET);
	if(off == -1){
		fprintf(stderr, "saveheapfd: lseek: %s\n", strerror(errno));
		goto fail;
	}

	/* segments */
	for(mt = 0; mt < Nmt; mt++){
		if(skipmt(mt))
			continue;
		for(g = 0; g < Nsgen; g++){
			m = &H.m[mt][g];
			if(m->h == 0)
				continue;
			s = a2s(m->h);
			while(1){
				if(-1 == xwrite(fd, s2a(s), Segsize)){
					fprintf(stderr,
						"saveheapfd: write: %s\n",
						strerror(errno));
					goto fail;
				}
				if(MTbig(mt)){
					t = s+1;
					while(MTbigcont(t->mt)){
						if(-1 == xwrite(fd, s2a(t), Segsize)){
							fprintf(stderr,
								"saveheapfd: write: %s\n",
								strerror(errno));
							goto fail;
						}
						t++;
					}
				}
				if(s->link == 0)
					break;
				s = a2s(s->link);
			}
		}
	}

	op = p = mmap(0, sn*Segsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, off);
	if(p == MAP_FAILED){
		fprintf(stderr, "saveheapfd: mmap: %s\n", strerror(errno));
		goto fail;
	}

	/* relocate segments */
	for(mt = 0; mt < Nmt; mt++){
		if(skipmt(mt))
			continue;
		for(g = 0; g < Nsgen; g++){
			m = &H.m[mt][g];
			if(m->h == 0)
				continue;
			s = a2s(m->h);
			while(1){
				saveseg(p, s);
				p += Segsize;
				if(MTbig(mt)){
					t = s+1;
					while(MTbigcont(t->mt)){
						p += Segsize;
						t++;
					}
				}
				if(s->link == 0)
					break;
				s = a2s(s->link);
			}
		}
	}

	munmap(op, sn*Segsize);
	return 0;

fail:
	if(op && op != MAP_FAILED)
		munmap(op, sn*Segsize);
	return -1;
}

typedef
struct Scanptr
{
	u8 mt;
	u8 gen;
	uptr ptr;
} Scanptr;

Tab*
restoreheap(const char *file)
{
	int fd;
	u64 len;
	struct stat st;
	void *tmp, *op, *p, *top;
	u32 i, nseg, nsp;
	Seg *tseg, *s;
	uptr root[11+Nsgen];
	char **csym;
	u32 ncsym;
	LSctx ls;
	Scanptr *sptr, *sp;
	Tab *toplevel;
	u32 version, sz;
	char buf[32];
	u8 magic;

	tseg = 0;
	csym = 0;
	sptr = 0;
	len = 0;
	op = p = 0;
	fd = -1;

	if(file){
		fd = open(file, O_RDONLY);
		if(0 > fd){
			fprintf(stderr, "restoreheap: open: %s\n",
				strerror(errno));
			goto fail;
		}
		if(0 > fstat(fd, &st)){
			fprintf(stderr, "restoreheap: stat: %s\n",
				strerror(errno));
			goto fail;
		}
		len = st.st_size;
		tmp = mmap(0, len, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
		if(tmp == MAP_FAILED){
			fprintf(stderr, "restoreheap: stat: %s\n",
				strerror(errno));
			goto fail;
		}
		close(fd);
		fd = -1;
		op = p = mapmem(len); /* Segsize-aligned */
		memcpy(p, tmp, len);
		munmap(tmp, roundup(len, 4096));
	} else {
		if(findheapimage) {
			unsigned char *heapimage;

			heapimage = findheapimage(&len);

			if((!heapimage) || (heapimage[0] != Heapdefined))
				return 0;

			op = p = mapmem(len); /* Segsize-aligned; closer to
					 other segments than executable */
			memcpy(p, heapimage, len);
		}
	}

#define XRD(to, fr, sz) { 		\
	if(len < (sz)) goto fail;	\
	memcpy((to), (fr), (sz));		\
	(fr) += (sz);			\
	len -= (sz);			\
}

	/* meta */
	XRD(&magic, p, sizeof(u8));
	XRD(&version, p, sizeof(u32));
	if(version != heapversion){
		fprintf(stderr, "restoreheap: heap version mismatch "
			"(need %u, got %u)\n",
			heapversion, version);
		goto fail;
	}
	XRD(&sz, p, sizeof(u32));
	if(sz > sizeof(buf)){
		fprintf(stderr, "restoreheap: corrupt heap header\n");
		goto fail;
	}
	XRD(buf, p, sz);
	if(0 && strncmp(buf, sysos, sz)){
		fprintf(stderr, "restoreheap: os mismatch"
			"(need \"%s\", got \"%.*s\")\n",
			sysos, sz, buf);
		goto fail;
	}
	XRD(&sz, p, sizeof(u32));
	if(sz > sizeof(buf)){
		fprintf(stderr, "restoreheap: corrupt heap header\n");
		goto fail;
	}
	XRD(buf, p, sz);
	if(strncmp(buf, sysarch, sz)){
		fprintf(stderr, "restoreheap: architecture mismatch"
			"(need \"%s\", got \"%.*s\")\n",
			sysarch, sz, buf);
		goto fail;
	}

	/* segment descriptors */
	XRD(&nseg, p, sizeof(u32));
	tseg = emalloc(nseg*sizeof(Seg));
	for(i = 0, s = tseg; i < nseg; i++, s++){
		XRD(&s->mt, p, sizeof(u8));
		XRD(&s->gen, p, sizeof(u8));
		XRD(&s->a, p, sizeof(uptr)); /* relative */
		XRD(s->card, p, Ncard*sizeof(u8));
		XRD(s->crossing, p, Ncard*sizeof(u8));
	}

	/* scan pointers */
	XRD(&nsp, p, sizeof(u32));
	sptr = emalloc(nsp*sizeof(Scanptr));
	for(i = 0; i < nsp; i++){
		XRD(&sptr[i].mt, p, sizeof(u8));
		XRD(&sptr[i].gen, p, sizeof(u8));
		XRD(&sptr[i].ptr, p, sizeof(uptr));
	}

	/* roots */
	for(i = 0; i < sizeof(root)/sizeof(uptr); i++)
		XRD(&root[i], p, sizeof(uptr));

	/* C symbols */
	XRD(&ncsym, p, sizeof(u32));
	csym = emalloc(ncsym*sizeof(char*));
	for(i = 0; i < ncsym; i++){
		csym[i] = p;
		p += strlen(p)+1;
	}

	top = p = (void*)roundup(p, Segsize);

	ls.mode = LSload;
	ls.seg = tseg;
	ls.nseg = nseg;
	ls.csym = csym;
	ls.ncsym = ncsym;
	ls.top = top;

	/* segments */
	resetmem();
	initsegmap2(top, nseg*Segsize);
	i = 0;
	while(i < nseg){
		s = &tseg[i];
		if(MTbig(s->mt)){
			importbigseg(s->mt, s->gen, p,
				     (uptr)s->a, s->card, s->crossing);
			loadseg(p, &ls);
			p += Segsize;
			i++;
			while(i < nseg && MTbigcont(tseg[i].mt)){
				p += Segsize;
				i++;
			}
		}else{
			importseg(s->mt, s->gen, p,
				  (uptr)s->a, s->card, s->crossing);
			loadseg(p, &ls);
			p += Segsize;
			i++;
		}
	}

	/* update scan pointers */
	for(i = 0, sp = sptr; i < nsp; i++, sp++)
		H.m[sp->mt][sp->gen].scan = resolveptr(sp->ptr, &ls);

	/* update roots */
	syms = resolveptr(root[0], &ls);
	typecache = resolveptr(root[1], &ls);
	litdom = resolveptr(root[2], &ls);
	cvalnull = resolveptr(root[3], &ls);
	cval0 = resolveptr(root[4], &ls);
	cval1 = resolveptr(root[5], &ls);
	cvalminus1 = resolveptr(root[6], &ls);
	vabort = resolveptr(root[7], &ls);
	halt = resolveptr(root[8], &ls);
	stkunderflow = resolveptr(root[9], &ls);
	toplevel = resolveptr(root[10], &ls);
	for(i = 0; i < Nsgen; i++)
		H.guard.gd[i] = resolveptr(root[11+i], &ls);

#undef XRD

	if(H.m[MTdata][0].h == 0)
		resetalloc(MTdata, 0);
	if(H.m[MTcode][0].h == 0)
		resetalloc(MTcode, 0);
	if(H.m[MTstack][0].h == 0)
		resetalloc(MTstack, 0);
	if(H.m[MTweak][0].h == 0)
		resetalloc(MTweak, 0);
	if(H.m[MTbox][0].h == 0)
		resetalloc(MTbox, 0);
	if(H.m[MTmutable][0].h == 0)
		resetalloc(MTmutable, 0);

	/* rehash tables */
	p = top;
	i = 0;
	while(i < nseg){
		s = &tseg[i];
		rehash(p);
		p += Segsize;
		i++;
		if(MTbig(s->mt)){
			while(i < nseg && MTbigcont(tseg[i].mt)){
				p += Segsize;
				i++;
			}
		}
	}

	/* reset crap we don't have a better way to reset */
	tabput(toplevel, mkvalcid(mkcid0("$winders")), Xnil);
	tabput(toplevel, mkvalcid(mkcid0("$repllevel")), mkvalcval2(cval0));

	efree(tseg);
	efree(sptr);
	efree(csym);
	return toplevel;
fail:
	if(op && op != MAP_FAILED)
		munmap(op, roundup(len, 4096));
	close(fd);
	efree(tseg);
	efree(sptr);
	efree(csym);
	return 0;
}
