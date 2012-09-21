#include "sys.h"
#include "util.h"
#include "syscqct.h"

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
	Mode,
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

/* meta type values, must be consistent with Mtag ordering */
enum
{
	MThole    = (Mhole<<Ftag),
	MTnix     = (Mnix<<Ftag),
	MTfree    = (Mfree<<Ftag),
	MTdata    = (Mdata<<Ftag),
	MTode     = (Mode<<Ftag),
	MTcode    = (Mcode<<Ftag),
	MTweak    = (Mweak<<Ftag),
	MTbox     = (Mbox<<Ftag),
	MTmutable = (Mmutable<<Ftag),
	MTbigdata = (Mdata<<Ftag)|(1<<Fbig),
	MTbigode  = (Mode<<Ftag)|(1<<Fbig),
	MTbigcode = (Mcode<<Ftag)|(1<<Fbig),
	Nmt,
};

static char *MTname[] = {
	[MThole]    = "hole",
	[MTnix]     = "nix",
	[MTfree]    = "free",
	[MTdata]    = "data",
	[MTode]     = "ode",
	[MTcode]    = "code",
	[MTweak]    = "weak",
	[MTbox]     = "box",
	[MTmutable] = "mutable",
	[MTbigdata] = "bigdata",
	[MTbigode]  = "bigode",
	[MTbigcode] = "bigcode",
};

#define MTtag(mt)        ((mt)>>Ftag)
#define MTold(mt)        (((mt)>>Fold)&1)
#define MTbig(mt)        (((mt)>>Fbig)&1)
#define MTbigcont(mt)    (((mt)>>Fbigcont)&1)
#define MTsettag(mt,t)   ((mt) = ((t)<<Ftag)|((mt)&~(Ftag-1)))
#define MTsetold(mt)     ((mt) |= 1<<Fold)
#define MTsetbig(mt)     ((mt) |= 1<<Fbig)
#define MTsetbigcont(mt) ((mt) |= 1<<Fbigcont)
#define MTclrold(mt)     ((mt) &= ~(1<<Fold))
#define MTclrbig(mt)     ((mt) &= ~(1<<Fbig))
#define MTclrbigcont(mt) ((mt) &= ~(1<<Fbigcont))

/* #define to ensure 64-bit constants */
#if 0
#define Segbits   12
#define Segsize   (1ULL<<Segbits)
#define	GCthresh  2*1024*Segsize
#define Seghunk	  4*1024*Segsize
#else
#define Segbits   20
#define Segsize   (1ULL<<Segbits)
#define	GCthresh  4*Segsize
#define Seghunk	  4*Segsize
#endif

#define Cardbits  3
#define Ncard     (1ULL<<Cardbits)
#define Cardsize  (1ULL<<(Segbits-Cardbits))
#define Seguse    (Segsize-sizeof(void*))
#define Segmask   ~(Segsize-1)
#define Minheap   10*Seghunk
#define Align     4
#define card(a)   ((u8)(((uptr)(a)>>(Segbits-Cardbits))&(Ncard-1)))

/* n must be a power-of-2 */
#define roundup(l,n)   ((uptr)(((uptr)(l)+((n)-1))&~((n)-1)))
#define rounddown(l,n) ((uptr)(((uptr)(l))&~((n)-1)))
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
	Gfull=Gstatic,
	Ngen=Gstatic,
	Nsgen=Gstatic+1,
	Glock,
	Allgen,
	Clean=0xff,
	Dirty=0x00,
};

static char *genname[] = {
	"0",
	"1",
	"2",
	"3",
	"S",
	"?",
	"L",
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
	Pair *p;		/* protected objects */
	u32 nprotect;
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
struct Qtype
{
	char *id;
	u32 sz;
	u32 clearit;
	Freeheadfn free1;
	Val* (*iter)(Head *hd, Ictx *ictx);
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
	Guard ug;		/* user guard list */
	Guard sg;		/* system guard list */
	Pair *guards[Qnkind];	/* system per-type guardians */
	Pair *unlocked;		/* objects unlocked since last collection */
	unsigned disable;
	u32 gctrip, gcsched[Ngen], ingc;
} Heap;

typedef
struct Stats
{
	u32 ncard;
	u32 nlock;
	u32 nseg[Nmt][Nsgen];
	u64 roottime;
	u64 unlocktime;
	u64 locktime;
	u64 sweeptime;
	u64 guardtime;
	u64 promotetime;
	u64 cardtime;
	u64 oldtime;
	u64 weaktime;
	u64 inittime;
	u64 finaltime;
	u64 recycletime;
	u64 resettime;
	u64 finimainttime;
	u64 crossings[Ncard];
	u64 multicardscan;
} Stats;

static int freefd(Head*);
static int freestr(Head*);

static Val* iteras(Head*, Ictx*);
static Val* iterbox(Head*, Ictx*);
static Val* itercl(Head*, Ictx*);
static Val* itercval(Head*, Ictx*);
static Val* iterdom(Head*, Ictx*);
static Val* iterexpr(Head*, Ictx*);
static Val* iterfd(Head*, Ictx*);
static Val* iterns(Head*, Ictx*);
static Val* iterode(Head*, Ictx*);
static Val* iterpair(Head*, Ictx*);
static Val* iterrange(Head*, Ictx*);
static Val* iterrd(Head*, Ictx*);
static Val* iterrec(Head*, Ictx*);
static Val* itertab(Head*, Ictx*);
static Val* itervec(Head*, Ictx*);

static void copykstack(Val *stack, Imm len, Imm fp);

static Qtype qs[Qnkind] = {
	[Qas]	 = { "as", sizeof(As), 1, 0, iteras },
	[Qbox]	 = { "box", sizeof(Box), 0, 0, iterbox },
	[Qcid]   = { "cid", sizeof(Cid), 1, 0, 0 },
	[Qcl]	 = { "closure", sizeof(Closure), 1, 0, itercl },
	[Qcode]	 = { "code", sizeof(Code), 1, 0, 0 },
	[Qctype] = { "ctype", sizeof(Ctype), 1, 0, iterctype },
	[Qcval]  = { "cval", sizeof(Cval), 0, 0, itercval },
	[Qdom]	 = { "domain", sizeof(Dom), 0, 0, iterdom },
	[Qexpr]	 = { "expr", sizeof(Expr), 1, 0, iterexpr },
	[Qfd]	 = { "fd", sizeof(Fd), 0, freefd, iterfd },
	[Qlist]	 = { "list", sizeof(List), 0, 0, iterlist },
	[Qnil]	 = { "nil", sizeof(Head), 0, 0, 0 },
	[Qns]	 = { "ns", sizeof(Ns), 1, 0, iterns },
	[Qode]	 = { "ode", sizeof(Ode), 1, freeode, iterode },
	[Qpair]	 = { "pair", sizeof(Pair), 0, 0, iterpair },
	[Qrange] = { "range", sizeof(Range), 0, 0, iterrange },
	[Qrd]    = { "rd", sizeof(Rd), 0, 0, iterrd },
	[Qrec]	 = { "record", sizeof(Rec), 0, 0, iterrec },
	[Qstr]	 = { "string", sizeof(Str), 1, freestr, 0 },
	[Qtab]	 = { "table",  sizeof(Tab), 1, 0, itertab },
	[Qundef] = { "undef", sizeof(Head), 0, 0, 0 },
	[Qvec]	 = { "vector", sizeof(Vec), 0, 0, itervec },
};

static Segmap	segmap;
static Heap	H;
static unsigned	alldbg = 0;
static Stats	stats;

static int
freefd(Head *hd)
{
	Fd *fd;
	fd = (Fd*)hd;
	/* FIXME: should dovm close closure if there is one (as finalizer?) */
	if((fd->flags&Fclosed) == 0
	   && fd->flags&Ffn
	   && fd->u.fn.close)
		fd->u.fn.close(&fd->u.fn);
	return 1;
}

static int
freestr(Head *hd)
{
	Str *str;
	Strmmap *m;
	Strmalloc *a;
	str = (Str*)hd;
	switch(str->skind){
	case Smmap:
		m = (Strmmap*)str;
		xmunmap(m->s, m->mlen);
		break;
	case Smalloc:
		a = (Strmalloc*)str;
		efree(a->s);
		break;
	case Sheap:
	case Sperm:
		fatal("bug");
	}
	return 1;
}

static Val*
iteras(Head *hd, Ictx *ictx)
{
	/* FIXME: is it really necessary
	   to mark dispatch and the other
	   cached functions? */
	As *as;
	as = (As*)hd;
	switch(ictx->n++){
	case 0:
		return (Val*)&as->mtab;
	case 1:
		return (Val*)&as->name;
	case 2:
		return (Val*)&as->get;
	case 3:
		return (Val*)&as->put;
	case 4:
		return (Val*)&as->ismapped;
	case 5:
		return (Val*)&as->map;
	case 6:
		return (Val*)&as->dispatch;
	default:
		return GCiterdone;
	}
}

static Val*
iterbox(Head *hd, Ictx *ictx)
{
	Box *box;
	box = (Box*)hd;
	if(ictx->n > 0)
		return GCiterdone;
	ictx->n = 1;
	return &box->v;
}

static Val*
itercl(Head *hd, Ictx *ictx)
{
	Closure *cl;
	cl = (Closure*)hd;
	if(ictx->n > cl->dlen+2)
		return GCiterdone;
	if(ictx->n == cl->dlen+1){
		ictx->n++;
		return (Val*)&cl->id;
	}
	if(ictx->n == cl->dlen+2){
		ictx->n++;
		if(cl->xfn)
			return (Val*)&cl->xfn;
		else
			return GCiterdone;
	}
	if(ictx->n == cl->dlen){
		ictx->n++;
		return (Val*)&cl->code;
	}
	if(cl->code == kcode){
		copykstack(cldisp(cl), cl->dlen, cl->fp);
		ictx->n = cl->dlen+1;
		return (Val*)&cl->code;
	}
	return &cldisp(cl)[ictx->n++];
}

static Val*
iterode(Head *hd, Ictx *ictx)
{
	Ode *code;
	u32 n;
	code = (Ode*)hd;
	n = ictx->n++;
	if(n < code->ninsn)
		return (Val*)&code->src[n];
	else if(n == code->ninsn)
		return (Val*)&code->konst;
	else if(n == code->ninsn+1)
		return (Val*)&code->id;
	else
		return GCiterdone;
}

static Val*
itercval(Head *hd, Ictx *ictx)
{
	Cval *cval;
	cval = (Cval*)hd;

	switch(ictx->n++){
	case 0:
		return (Val*)&cval->dom;
	case 1:
		return (Val*)&cval->type;
	default:
		return GCiterdone;
	}
}

static Val*
iterdom(Head *hd, Ictx *ictx)
{
	Dom *dom;
	dom = (Dom*)hd;
	switch(ictx->n++){
	case 0:
		return (Val*)&dom->as;
	case 1:
		return (Val*)&dom->ns;
	case 2:
		return (Val*)&dom->name;
	default:
		return GCiterdone;
	}
}

static Val*
iterexpr(Head *hd, Ictx *ictx)
{
	Expr *e;
	e = (Expr*)hd;
	switch(ictx->n++){
	case 0:
		return (Val*)&e->e1;
	case 1:
		return (Val*)&e->e2;
	case 2:
		return (Val*)&e->e3;
	case 3:
		return (Val*)&e->e4;
	case 4:
		return (Val*)&e->aux;
	case 5:
		return (Val*)&e->skind;
	case 6:
		return (Val*)&e->src;
	default:
		return GCiterdone;
	}
}

static Val*
iterfd(Head *hd, Ictx *ictx)
{
	Fd *fd;
	fd = (Fd*)hd;
	switch(ictx->n++){
	case 0:
		return (Val*)&fd->name;
	case 1:
		if(fd->flags&Ffn)
			return GCiterdone;
		return (Val*)&fd->u.cl.close;
	case 2:
		return (Val*)&fd->u.cl.read;
	case 3:
		return (Val*)&fd->u.cl.write;
	default:
		return GCiterdone;
	}
}

static Val*
iterns(Head *hd, Ictx *ictx)
{
	/* FIXME: is it really necessary
	   to mark dispatch and the other
	   cached functions? */
	Ns *ns;
	unsigned n;
	enum { lastfield = 7 };

	ns = (Ns*)hd;
	n = ictx->n++;
	switch(n){
	case 0:
		return (Val*)&ns->lookaddr;
	case 1:
		return (Val*)&ns->looksym;
	case 2:
		return (Val*)&ns->looktype;
	case 3:
		return (Val*)&ns->enumtype;
	case 4:
		return (Val*)&ns->enumsym;
	case 5:
		return (Val*)&ns->name;
	case 6:
		return (Val*)&ns->dispatch;
	case lastfield:
		return (Val*)&ns->mtab;
	}
	n -= lastfield;
	if(n >= Vnallbase)
		return GCiterdone;
	return (Val*)&ns->base[n];
}

static Val*
iterpair(Head *hd, Ictx *ictx)
{
	Pair *pair;
	pair = (Pair*)hd;

	switch(ictx->n++){
	case 0:
		return &pair->cdr;
	case 1:
		if(isweak(hd))
			return GCiterdone;
		else
			return &pair->car;
	default:
		return GCiterdone;
	}
}

static Val*
iterrange(Head *hd, Ictx *ictx)
{
	Range *range;
	range = (Range*)hd;

	switch(ictx->n++){
	case 0:
		return (Val*)&range->beg;
	case 1:
		return (Val*)&range->len;
	default:
		return GCiterdone;
	}
}

static Val*
iterrd(Head *hd, Ictx *ictx)
{
	Rd *rd;
	rd = (Rd*)hd;
	switch(ictx->n++){
	case 0:
		return (Val*)&rd->name;
	case 1:
		return (Val*)&rd->fname;
	case 2:
		return (Val*)&rd->is;
	case 3:
		return (Val*)&rd->mk;
	case 4:
		return (Val*)&rd->fmt;
	case 5:
		return (Val*)&rd->get;
	case 6:
		return (Val*)&rd->set;
	default:
		return GCiterdone;
	}
}

static Val*
iterrec(Head *hd, Ictx *ictx)
{
	Rec *r;
	r = (Rec*)hd;
	if(ictx->n < r->nf)
		return &recdata(r)[ictx->n++];
	switch(ictx->n-r->nf){
	case 0:
		ictx->n++;
		return (Val*)&r->rd;
	default:
		return GCiterdone;
	}
}

static Val*
itertab(Head *hd, Ictx *ictx)
{
	Tab *t;
	t = (Tab*)hd;
	switch(ictx->n++){
	case 0:
		return (Val*)&t->ht;
	case 1:
		return (Val*)&t->tg;
	case 2:
		return (Val*)&t->def;
	default:
		return GCiterdone;
	}
}

static Val*
itervec(Head *hd, Ictx *ictx)
{
	Vec *vec;
	vec = (Vec*)hd;
	if(ictx->n >= vec->len)
		return GCiterdone;
	return &vecdata(vec)[ictx->n++];
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
	u64 nseg, onseg;
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

	/* FIXME: this should be unnecessary */
	s->p = 0;
	s->nprotect = 0;
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
	s->a = p;
	s->e = p+sz-1;

	/* beginning Seg of big segment */
	memset(s->card, Clean, sizeof(s->card));
	memset(s->crossing, 0, sizeof(s->crossing));
	s->mt = mt;
	s->gen = g;
	/* FIXME: this should be unnecessary */
	s->p = 0;
	s->nprotect = 0;
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

		/* FIXME: this should be unnecessary */
		s->p = 0;
		s->nprotect = 0;
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
	case Mode:
	case Mcode:
	case Mweak:
	case Mbox:
	case Mmutable:
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

u64
meminuse()
{
	return H.inuse;
}

static int
islocked(Seg *s, Head *h)
{
	Head *p;
	if(s->nprotect == 0)
		return 0;
	p = (Head*)s->p;
	while(p){
		if(cdar(p) == h)
			return 1;
		p = cdr(p);
	}
	return 0;
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

Head*
malode()
{
	Head *h;
	h = __mal(MTode, H.tg, sizeof(Ode));
	Vsetkind(h, Qode);
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
	if(len > Seguse){
		if(kind == Qcode)
			h = _malbig(MTbigcode, roundup(len, Align));
		else
			h = _malbig(MTbigdata, roundup(len, Align));
	}else
		h = _mal(kind, roundup(len, Align));
	Vsetkind(h, kind);
	return h;
}

Head*
malq(Qkind kind, u32 sz)
{
	Head *h;
	h = _mal(kind, sz);
	Vsetkind(h, kind);
	return h;
}

static void*
curaddr(Val v)
{
	if(Vfwd(v))
		return Vfwdaddr(v);
	return v;
}

static u32
qsz(Head *h)
{
	Cid *id;
	Closure *cl;
	Code *c;
	Rec *r;
	Str *s;
	Ctype *t;
	Vec *v;

	switch(Vkind(h)){
	case Qcid:
		id = (Cid*)h;
		return roundup(cidsize(id->len), Align);
	case Qcl:
		cl = (Closure*)h;
		return roundup(clsize(cl->dlen), Align);
	case Qstr:
		s = (Str*)h;
		switch(s->skind){
		case Sheap:
			return roundup(strsize(s->len), Align);
		case Smalloc:
			return sizeof(Strmalloc); /* FIXME: align? */
		case Smmap:
			return sizeof(Strmmap); /* FIXME: align? */
		case Sperm:
			return sizeof(Strperm); /* FIXME: align? */
		}
		fatal("bug");
	case Qcode:
		c = (Code*)h;
		return roundup(c->sz, Align);
	case Qvec:
		v = (Vec*)h;
		return roundup(vecsize(v->len), Align);
	case Qrec:
		r = (Rec*)h;
		return roundup(recsize(r->nf), Align);
	case Qctype:
		t = (Ctype*)h;
		switch(t->tkind){
		case Tvoid:
			return align(sizeof(Ctype));
		case Tbase:
			return align(sizeof(Ctypebase));
		case Tstruct:
		case Tunion:
			return align(sizeof(Ctypesu));
		case Tenum:
			return align(sizeof(Ctypeenum));
		case Ttypedef:
			return align(sizeof(Ctypedef));
		case Tptr:
			return align(sizeof(Ctypeptr));
		case Tarr:
			return align(sizeof(Ctypearr));
		case Tfun:
			return align(sizeof(Ctypefunc));
		case Tundef:
			return align(sizeof(Ctypeundef));
		case Tbitfield:
			return align(sizeof(Ctypebitfield));
		case Tconst:
			return align(sizeof(Ctypeconst));
		default:
			bug();
		}
	default:
		return qs[Vkind(h)].sz;
	}
	fatal("bug");
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
	if(Vfwd(h)){
		if(dbg)printf("copy: read fwd %p -> %p\n",
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
	if(islocked(s, h)){
		// protected objects do not move.
		if(dbg)printf("copy: object %p is protected\n", h);
		return Glock;
	}
	if(!MTold(s->mt) && s->gen != Glock){
		// this object does not move
		if(dbg)printf("copy: object %p not in from space (gen %d)\n",
			      h, s->gen);
		return s->gen;
	}
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
	case Qode:
		nh = malode();
		break;
	case Qpair:
		if(isweak(h))
			nh = malweak();
		else
			nh = malq(Qpair, sizeof(Pair));
		break;
	case Qcid:
	case Qcl:
	case Qcode:
	case Qctype:
	case Qstr:
	case Qvec:
	case Qrec:
		nh = malv(Vkind(h), sz);
		break;
	default:
		nh = malq(Vkind(h), sz);
		break;
	}
	memcpy(nh, h, sz);
	Vsetfwd(h, (uptr)nh);
	if(dbg)printf("set fwd %p -> %p %p (%d)\n",
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
scan1code(Code *c)
{
	u8 min;
	Imm i;
	Reloc *r;
	void *h, **cp;
	void *p;

	min = Clean;
	gcopy((Val*)&c->id, &min);
	gcopy((Val*)&c->reloc, &min);
	gcopy((Val*)&c->lm, &min);
	gcopy((Val*)&c->konst, &min);
	gcopy((Val*)&c->src, &min);
	r = (Reloc*)strdata(c->reloc);
	p = c;
	for(i = 0; i < c->nreloc; i++, r++){
		cp = (void**)(p+r->coff);
		h = *cp-r->ioff;
		gcopy((Val*)&h, &min);
		*cp = h+r->ioff;
	}
	return min;
}

static u8
scan1(Head *h)
{
	Ictx ictx;
	Head **c;
	u8 min, g;
	unsigned dbg = alldbg;

	min = Clean;
	if(Vkind(h) == Qcode)
		return scan1code((Code*)h);
	if(qs[Vkind(h)].iter == 0)
		return min;
	memset(&ictx, 0, sizeof(ictx));
	while(1){
		c = qs[Vkind(h)].iter(h, &ictx);
		if(c == (Val*)GCiterdone)
			break;
		if(dbg)printf("scan1 %p (%s) iter %p %p\n",
			    h, qs[Vkind(h)].id,
			    c, *c);
		g = copy(c);
		if(g < min)
			min = g;
	}
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
			if(dbg)printf("scanning %p (%s)\n", h, qs[Vkind(h)].id);
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
		for(i = 0; i < Nmt; i++)
			again |= scan(&H.m[i][tg]);
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
	_instguard(&H.ug, p);
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
		if(Vkind(car(m)) != Qnil){
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
quard(Val o)
{
	Pair *t;
	t = H.guards[Vkind(o)];
	if(t == 0)
		fatal("bug");
	_instguard(&H.sg, cons(o, t));
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
	s = a2s(o);
	if(islocked(s, o))
		return 1;
	if(!MTold(s->mt) && s->gen != Glock)
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
toprd(void *u, void *k, void *v)
{
	copy(v);
}

static void
copykstack(Val *stack, Imm len, Imm fp)
{
	Imm pcp, narg, m, i, clx;
	Insn *pc, *fsz, *fm, *ci;
	u64 sz, mask, *mp, o;
	Closure *cl;
	Imm shift;
	Code *cp;
	uptr coff;

	/* fp is normally relative to Maxstk */
	shift = Maxstk-len;
	fp -= shift;

	/* stack corresponds to Ikg op in call to callcc.
	   nothing to do in this frame. */

	while(1){
		narg = stkimm(stack[fp]);
		clx = fp+narg+2;
		cl = valcl(stack[clx]);
		copy(&stack[clx]);
		cl = valcl(stack[clx]);
		for(i = 0; i < narg; i++)
			copy(&stack[fp+1+i]);
		pcp = fp+narg+1;
		pc = stkp(stack[pcp]);
		if(pc == 0)
			break;

		/* copy live variables in this frame */
		fp = stkimm(stack[fp+narg+3]);
		fp -= shift;
		cl = valcl(stack[clx]);
		fsz = pc-1;
		fm = pc-3;
		if(fsz->kind != Ifsize || fm->kind != Ifmask)
			fatal("no live mask for pc %p cl %p", pc, cl);
		sz = fsz->cnt;
		mask = fm->cnt;
		m = fp-1;
		if((mask>>(mwbits-1))&1){
			o = mask&~(1ULL<<(mwbits-1));
			mp = (u64*)strdata(cl->code->lm)+o;
			while(sz > 0){
				mask = *mp++;
				for(i = 0; sz > 0 && i < mwbits; i++){
					if((mask>>i)&1)
						copy(&stack[m]);
					sz--;
					m--;
				}
			}
		}else
			for(i = 0; i < sz; i++){
				if((mask>>i)&1)
					copy(&stack[m]);
				m--;
			}

		/* copy code and update pc on stack */
		ci = pc-2;
		if(ci->kind != Icode)
			fatal("no code for pc %d cl %p", pc, cl);
		cp = (void*)ci-ci->cnt;
		coff = (uptr)pc-(uptr)cp;
		copy((Val*)&cp);
		pc = (Insn*)((uptr)cp+coff);
		stack[pcp] = (Val)(uptr)pc;
	}
}

/* assume current frame has no live refererences,
   i.e., we are at start of a call:
	fp points to beginning of frame for the current call
	sp should be fp
	pc is first insn in call
*/

static void
copystack(VM *vm)
{
	Imm ofp, fp, pcp, narg, m, i, clx;
	Insn *pc, *fsz, *fm, *ci;
	u64 sz, mask, *mp, o;
	Closure *cl;
	uptr coff;
	Code *cp;

	fp = vm->fp;
	if(fp == 0)
		return;

	/* copy current pc and code if we appear to be in a call
	   (e.g., via gc() or compact() rather than gcpoll */
	pc = vm->pc;
	ci = pc-2;
	if(ci->kind == Icode){
		cp = (void*)ci-ci->cnt;
		coff = (uptr)pc-(uptr)cp;
		copy((Val*)&cp);
		pc = (Insn*)((uptr)cp+coff);
		vm->pc = pc;
	}

	while(1){
		narg = stkimm(vm->stack[fp]);
		clx = fp+narg+2;
		copy(&vm->stack[clx]);
		for(i = 0; i < narg; i++)
			copy(&vm->stack[fp+1+i]);
		pcp = fp+narg+1;
		pc = stkp(vm->stack[pcp]);
		if(pc == 0)
			break;

		/* copy live variables in this frame */
		ofp = fp;
		fp = stkimm(vm->stack[fp+narg+3]);
		cl = valcl(vm->stack[clx]);
		fsz = pc-1;
		fm = pc-3;
		if(fsz->kind != Ifsize || fm->kind != Ifmask)
			fatal("no live mask for pc %d cl %p", pc, cl);
		sz = fsz->cnt;
		if(fp-ofp != sz+narg+4)
			fatal("frame botch ofp %lu fp %lu sz %llu narg %llu",
			      ofp, fp, sz, narg);
		mask = fm->cnt;
		m = fp-1;
		if((mask>>(mwbits-1))&1){
			o = mask&~(1ULL<<(mwbits-1));
			mp = (u64*)strdata(cl->code->lm)+o;
			while(sz > 0){
				mask = *mp++;
				for(i = 0; sz > 0 && i < mwbits; i++){
					if((mask>>i)&1)
						copy(&vm->stack[m]);
					sz--;
					m--;
				}
			}
		}else
			for(i = 0; i < sz; i++){
				if((mask>>i)&1)
					copy(&vm->stack[m]);
				m--;
			}

		/* copy code and update pc on stack */
		ci = pc-2;
		if(ci->kind != Icode)
			fatal("no code for pc %d cl %p", pc, cl);
		cp = (void*)ci-ci->cnt;
		coff = (uptr)pc-(uptr)cp;
		copy((Val*)&cp);
		pc = (Insn*)((uptr)cp+coff);
		vm->stack[pcp] = (Val)(uptr)pc;
	}
}

static void
mark1old(Seg *s, u32 g)
{
	if(s->gen <= g)
		MTsetold(s->mt);
	else if(s->gen == Glock && s->nprotect == 0)
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
		if(!Vdead((Head*)p)){
			g = scan1(p);
			if(g < min)
				min = g;
		}
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
	if(s->gen == Glock)
		/* no need to scan: scanlocked will do it */
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
scanunlocked()
{
	Pair *p;
	u64 b;
	b = usec();
	p = H.unlocked;
	while(p != (Pair*)Xnil){
		if(!Vfwd(car(p)))
			scan1(car(p));
		p = (Pair*)cdr(p);
	}
	H.unlocked = (Pair*)Xnil;
	stats.unlocktime += usec()-b;
}

static void
scan1locked(Seg *s)
{
	Head *p;
	if(s->nprotect == 0)
		return;
	copy((Val*)&s->p); /* retain list of locked objects! */
	p = (Head*)s->p;
	while(p){
		stats.nlock++;
		scan1(cdar(p));
		p = cdr(p);
	}
}

static void
scanlocked()
{
	Seg *s, *es;
	u64 b;
	b = usec();
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			scan1locked(s);
		s = nextseg(s);
	}
	stats.locktime += usec()-b;
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
promote1locked(Seg *s)
{
	if(!MTold(s->mt))
		return;
	if(s->nprotect == 0)
		return;
	s->gen = Glock;
	MTclrold(s->mt);
}

static void
promotelocked()
{
	Seg *s, *es;
	u64 b;
	b = usec();
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			promote1locked(s);
		s = nextseg(s);
	}
	stats.promotetime += usec()-b;
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
	u32 i, m, mt;
	VM **vmp, *vm;
	Head *h;
	unsigned dbg = alldbg;
	u64 b;

//	memset(&stats, 0, sizeof(stats));
	b = usec();
	maintain();
	H.ingc++;
	if(g != tg && g != tg-1)
		fatal("bug");
	if(tg >= Nsgen)
		return; // FIXME: silently do nothing...caller should know

	/* force full collection when there are newly unlocked objects.
	   the problem is ensuring that live references to the unlocked
	   objects in older generations remain valid. */
	if(H.unlocked != (Pair*)Xnil)
		g = tg = Gfull;

	H.g = g;
	H.tg = tg;
	if(dbg)printf("gc(%u,%u)\n", g, tg);
	stats.inittime += usec()-b;

	b = usec();
	markold(g);
	for(i = 0; i <= g; i++)
		for(mt = 0; mt < Nmt; mt++)
			mclr(&H.m[mt][i]);

	if(g == tg){
		resetalloc(MTdata, tg);
		resetalloc(MTode, tg);
		resetalloc(MTcode, tg);
		resetalloc(MTweak, tg);
		resetalloc(MTbox, tg);
		resetalloc(MTmutable, tg);
	}else{
		getalloc(MTdata, tg);
		getalloc(MTode, tg);
		getalloc(MTcode, tg);
		getalloc(MTweak, tg);
		getalloc(MTbox, tg);
		getalloc(MTmutable, tg);
	}
	stats.oldtime += usec()-b;

	b = usec();
	vmp = vms;
	while(vmp < vms+Maxvms){
		vm = *vmp++;
		if(vm == 0)
			continue;
		copystack(vm);
		for(m = 0; m < vm->edepth; m++)
			copy((Val*)&vm->err[m].cl);
		copy((Val*)&vm->top->env->var);
		hforeachp(vm->top->env->rd, toprd, 0);
		copy(&vm->ac);
		copy((Val*)&vm->cl);
	}
	if(dbg)printf("copied vm roots\n");

	/* global roots */
	copy(&syms);
	copy(&typecache);

	// add per-type guards as roots
	for(i = 0; i < Qnkind; i++)
		copy((Val*)&H.guards[i]);
	if(dbg)printf("copied guard roots\n");

	stats.roottime += usec()-b;

	scanunlocked();
	scancards(g);
	if(dbg)printf("scanned cards\n");
	scanlocked();
	kleenescan(tg);
	if(dbg)printf("re-scanned tg data (after prot)\n");

	updateguards(&H.ug);
	updateguards(&H.sg);
	if(dbg)printf("did updateguards\n");
	updateweak(tg);
	promotelocked();

	b = usec();
	// call built-in finalizers
	for(i = 0; i < Qnkind; i++)
		if(H.guards[i])
			while((h = pop1guard(H.guards[i]))){
				if(dbg)printf("freeing object %p (%s)\n",
					      h, qs[Vkind(h)].id);
				Vsetdead(h, 1);
				qs[i].free1(h);
			}
	stats.finaltime += usec()-b;
	b = usec();
	recycle();
	if(dbg)printf("did recycle\n");
	stats.recycletime += usec()-b;

	b = usec();
	if(H.tg != 0){
		H.tg = 0;
		resetalloc(MTdata, H.tg);
		resetalloc(MTode, H.tg);
		resetalloc(MTcode, H.tg);
		resetalloc(MTweak, H.tg);
		resetalloc(MTbox, H.tg);
		resetalloc(MTmutable, H.tg);
	}
	H.na = 0;
	if(dbg)printf("end of collection\n");
	H.ingc--;
	stats.resettime += usec()-b;
	b = usec();
	maintain();
	stats.finimainttime += usec()-b;
	if(dbg)printf("gc returning\n");
}

void
gc(VM *vm)
{
	int i;
	u32 g, tg;

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
compact(VM *vm)
{
	dogc(vm, Ngen-1, Gstatic);
}

void*
gclock(void *v)
{
	Seg *s;
	Head *h;
	Pair *p;
	Cval *cv;

	if(v == 0)
		return v;
	h = v;
	if(Vfwd(h))
		fatal("bug");
	s = a2s(h);
	if(islocked(s, h)){
		/* find locked object record and bump count */
		p = s->p;
		while(p){
			if(cdar(p) == h){
				cv = (Cval*)caar(p);
				cvalu(cv)++;
				return h;
			}
			p = (Pair*)cdr(p);
		}
		fatal("lost track of locked object");
	}else{
		/* segment descriptor may move during allocation */
		p = s->p;
		p = cons(cons(mkvalcval(0, 0, 1), h), p);
		s = a2s(h);
		s->p = p;
		s->nprotect++;
		return h;
	}
}

void*
gcunlock(void *v)
{
	Seg *s;
	Head *h, *p, **r;
	Cval *cv;

	if(v == 0)
		return v;
	h = v;
	if(Vfwd(h))
		fatal("bug");
	s = a2s(h);
	if(!islocked(s, h))
		return h;
	r = (Head**)&s->p;
	p = *r;
	while(p){
		if(cdar(p) == h){
			cv = (Cval*)caar(p);
			cvalu(cv)--;
			if(cvalu(cv) == 0){
				*r = cdr(p);
				s->nprotect--;
				H.unlocked = cons(h, H.unlocked);
				return h;
			}else
				return h;
		}
		r = &cdr(p);
		p = *r;
	}
	fatal("lost track of locked object");
	return 0;
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
	if(e >= segmap.hi)
		return 0;
	while(p < e){
		if(p >= segmap.lo && p < segmap.hi){
			s = a2s(p);
			if(s->mt != MThole)
				return 0;
		}
		p += Segsize;
	}
	return 1;
}

void
initmem()
{
	u32 i, gr;

	initsegmap();
	gr = 1;
	for(i = 0; i < Ngen; i++){
		H.gcsched[i] = gr;
		gr *= GCradix;
	}
	resetalloc(MTdata, 0);
	resetalloc(MTode, 0);
	resetalloc(MTcode, 0);
	resetalloc(MTweak, 0);
	resetalloc(MTbox, 0);
	resetalloc(MTmutable, 0);
	H.na = 0;
	H.ma = GCthresh;

	/* we need nil now to initialize the guarded object lists */
	Xnil = gclock(malq(Qnil, sizeof(Head)));

	for(i = 0; i < Qnkind; i++)
		if(qs[i].free1)
			H.guards[i] = mkguard();
	for(i = 0; i < Nsgen; i++)
		H.ug.gd[i] = H.sg.gd[i] = (Pair*)Xnil;
	H.disable = 0;
	H.unlocked = (Pair*)Xnil;
}

void
finimem()
{
	_gc(Ngen-1, Ngen-1); /* collect non-quarded objects */
	_gc(Ngen-1, Ngen-1); /* collect quarded objects */
	/* FIXME: free all segments */
	/* FIXME: free static generation */
}


static void
gcstat1(MT mt, u64 *ns, u64 *np, u64 *nd)
{
	Gen g;
	for(g = G0; g <= Glock; g++){
		if(g == Nsgen)
			continue;
		printf("%10s %3s %8" PRIu64 " %8" PRIu64 " %8" PRIu64 "\n",
		       MTname[mt],
		       genname[g],
		       ns[g],
		       np[g],
		       nd[g]);
	}
}

void
gcstats()
{
	Seg *s, *es;
	u64 ns[Nmt][Allgen];
	u64 np[Nmt][Allgen];
	u64 nd[Nmt][Allgen];

	memset(ns, 0, sizeof(ns));
	memset(np, 0, sizeof(ns));
	memset(nd, 0, sizeof(ns));

	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		ns[s->mt][s->gen]++;
		if(isliveseg(s)){
			np[s->mt][s->gen] += s->nprotect;
#if 0
			if(s->card < s->gen)
				nd[s->mt][s->gen]++;
#endif
		}
		s = nextseg(s);
	}

	printf("%10s %3s %8s %8s %8s\n",
	       "TYPE", "GEN", "#SEG", "#LOCKED", "#DIRTY");

	gcstat1(MTfree, ns[MTfree], np[MTfree], nd[MTfree]);
	gcstat1(MTdata, ns[MTdata], np[MTdata], nd[MTdata]);
	gcstat1(MTode, ns[MTode], np[MTode], nd[MTode]);
	gcstat1(MTcode, ns[MTcode], np[MTcode], nd[MTcode]);
	gcstat1(MTweak, ns[MTweak], np[MTweak], nd[MTweak]);
	gcstat1(MTbox, ns[MTbox], np[MTbox], nd[MTbox]);
	gcstat1(MTmutable, ns[MTmutable], np[MTmutable], nd[MTmutable]);
	gcstat1(MTbigdata, ns[MTbigdata], np[MTbigdata], nd[MTbigdata]);
	gcstat1(MTbigode, ns[MTbigode], np[MTbigode], nd[MTbigode]);
	gcstat1(MTbigcode, ns[MTbigcode], np[MTbigcode], nd[MTbigcode]);

	printf(" inuse = %10" PRIu64 "\n", H.inuse);
	printf("  free = %10" PRIu64 "\n", H.free);
	printf("heapsz = %10" PRIu64 "\n", H.heapsz);
	printf(" bigsz = %10" PRIu64 "\n", H.bigsz);
	printf("smapsz = %10" PRIu64 "\n", H.smapsz);
	printf(" ncard = %10" PRIu32 "\n", stats.ncard);
	printf(" nlock = %10" PRIu32 "\n", stats.nlock);
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
	tabput(t, mkvalcid(mkcid0("unlocktime")),
	       mkvallitcval(Vuvlong, stats.unlocktime));
	tabput(t, mkvalcid(mkcid0("locktime")),
	       mkvallitcval(Vuvlong, stats.locktime));
	tabput(t, mkvalcid(mkcid0("sweeptime")),
	       mkvallitcval(Vuvlong, stats.sweeptime));
	tabput(t, mkvalcid(mkcid0("guardtime")),
	       mkvallitcval(Vuvlong, stats.guardtime));
	tabput(t, mkvalcid(mkcid0("promotetime")),
	       mkvallitcval(Vuvlong, stats.promotetime));
	tabput(t, mkvalcid(mkcid0("weaktime")),
	       mkvallitcval(Vuvlong, stats.weaktime));
	tabput(t, mkvalcid(mkcid0("oldtime")),
	       mkvallitcval(Vuvlong, stats.oldtime));
	tabput(t, mkvalcid(mkcid0("ncard")),
	       mkvallitcval(Vuvlong, stats.ncard));
	tabput(t, mkvalcid(mkcid0("inittime")),
	       mkvallitcval(Vuvlong, stats.inittime));
	tabput(t, mkvalcid(mkcid0("finaltime")),
	       mkvallitcval(Vuvlong, stats.finaltime));
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
