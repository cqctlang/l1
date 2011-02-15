#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
enum
{
	Mhole,
	Mnix,
	Mweak,
	Mfree,
	Mdata,
	Mcode, /* FIXME: Nmt definition depends on this being last */
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

/* meta type values */
enum
{
	MThole    = (Mhole<<Ftag),
	MTnix     = (Mnix<<Ftag),
	MTfree    = (Mfree<<Ftag),
	MTdata    = (Mdata<<Ftag),
	MTcode    = (Mcode<<Ftag),
	MTweak    = (Mweak<<Ftag),
	MTbigdata = (Mdata<<Ftag)|(1<<Fbig),
	MTbigcode = (Mcode<<Ftag)|(1<<Fbig),
	Nmt,
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
#define Segsize   4096ULL
#define Seguse    (Segsize-sizeof(void*))
#define Segmask   ~(Segsize-1)
#define	GCthresh  2*1024*Segsize
#define Seghunk	  4*1024*Segsize
#define Minheap   Seghunk
#define Align     4

/* n must be a power-of-2 */
#define roundup(l,n)   (((uintptr_t)(l)+((n)-1))&~((n)-1))
#define rounddown(l,n) (((uintptr_t)(l))&~((n)-1))

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
	Gfull=G3,
	Gstatic,
	Ngen=Gstatic,
	Nsgen=Gstatic+1,
	Glock,
	Clean=0xff,
	Dirty=0x00,
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
	u8 card;
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
	void *scan;
} M;

typedef
struct Guard
{
	Pair *gd[Nsgen];	/* guarded objects: (obj . guardian) pairs  */
} Guard;

typedef
struct Heap
{
	void *d, *c, *w;	/* data, code, weak allocation segments */
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
} Stats;

static int freecl(Head*);
static int freefd(Head*);
static int freestr(Head*);

static Val* iteras(Head*, Ictx*);
static Val* iterbox(Head*, Ictx*);
static Val* itercl(Head*, Ictx*);
static Val* itercode(Head*, Ictx*);
static Val* itercval(Head*, Ictx*);
static Val* iterdom(Head*, Ictx*);
static Val* iterfd(Head*, Ictx*);
static Val* iterns(Head*, Ictx*);
static Val* iterpair(Head*, Ictx*);
static Val* iterrange(Head*, Ictx*);
static Val* iterrd(Head*, Ictx*);
static Val* iterrec(Head*, Ictx*);
static Val* itertab(Head*, Ictx*);
static Val* itervec(Head*, Ictx*);
static Val* iterxtn(Head*, Ictx*);

static Qtype qs[Qnkind] = {
	[Qas]	 = { "as", sizeof(As), 1, 0, iteras },
	[Qbox]	 = { "box", sizeof(Box), 0, 0, iterbox },
	[Qcval]  = { "cval", sizeof(Cval), 0, 0, itercval },
	[Qcl]	 = { "closure", sizeof(Closure), 1, freecl, itercl },
	[Qcode]	 = { "code", sizeof(Code), 1, freecode, itercode },
	[Qdom]	 = { "domain", sizeof(Dom), 0, 0, iterdom },
	[Qfd]	 = { "fd", sizeof(Fd), 0, freefd, iterfd },
	[Qlist]	 = { "list", sizeof(List), 0, 0, iterlist },
	[Qnil]	 = { "nil", sizeof(Head), 0, 0, 0 },
	[Qns]	 = { "ns", sizeof(Ns), 1, 0, iterns },
	[Qnull]	 = { "null", sizeof(Head), 0, 0, 0 },
	[Qpair]	 = { "pair", sizeof(Pair), 0, 0, iterpair },
	[Qrange] = { "range", sizeof(Range), 0, 0, iterrange },
	[Qrd]    = { "rd", sizeof(Rd), 0, 0, iterrd },
	[Qrec]	 = { "record", sizeof(Rec), 0, 0, iterrec },
	[Qstr]	 = { "string", sizeof(Str), 1, freestr, 0 },
	[Qtab]	 = { "table",  sizeof(Tab), 1, 0, itertab },
	[Qundef] = { "undef", sizeof(Head), 0, 0, 0 },
	[Qvec]	 = { "vector", sizeof(Vec), 0, 0, itervec },
	[Qxtn]	 = { "typename", sizeof(Xtypename), 1, 0, iterxtn },
};

static Segmap	segmap;
static Heap	H;
static unsigned	alldbg = 0;
static Stats	stats;

static int
freecl(Head *hd)
{
	Closure *cl;
	cl = (Closure*)hd;
	efree(cl->display);
	efree(cl->id);
	return 1;
}

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
	// printf("freestr(%.*s)\n", (int)str->len, str->s);
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
		return (Val*)&as->map;
	case 5:
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
	if(ictx->n > cl->dlen+1)
		return GCiterdone;
	if(ictx->n == cl->dlen+1){
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
	return &cl->display[ictx->n++];
}

static Val*
itercode(Head *hd, Ictx *ictx)
{
	Code *code;
	code = (Code*)hd;
	if(ictx->n > 0)
		return GCiterdone;
	ictx->n++;
	return (Val*)&code->konst;
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

static Val*
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
			return (Val*)&xtn->tag;
		case 1:
			return (Val*)&xtn->field;
		case 2:
			return &xtn->attr;
		default:
			return GCiterdone;
		}
	case Tenum:
		switch(ictx->n++){
		case 0:
			return (Val*)&xtn->tag;
		case 1:
			return (Val*)&xtn->link;
		case 2:
			return (Val*)&xtn->konst;
		default:
			return GCiterdone;
		}
	case Tundef:
	case Tptr:
		if(ictx->n++ > 0)
			return GCiterdone;
		else
			return (Val*)&xtn->link;
	case Tarr:
		switch(ictx->n++){
		case 0:
			return &xtn->cnt;
		case 1:
			return (Val*)&xtn->link;
		default:
			return GCiterdone;
		}
	case Tfun:
		switch(ictx->n++){
		case 0:
			return (Val*)&xtn->link;
		case 1:
			return (Val*)&xtn->param;
		default:
			return GCiterdone;
		}
		break;
	case Ttypedef:
		switch(ictx->n++){
		case 0:
			return (Val*)&xtn->link;
		case 1:
			return (Val*)&xtn->tid;
		default:
			return GCiterdone;
		}
	case Tbitfield:
		switch(ictx->n++){
		case 0:
			return &xtn->cnt;
		case 1:
			return &xtn->bit0;
		case 2:
			return (Val*)&xtn->link;
		default:
			return GCiterdone;
		}
	case Tconst:
		switch(ictx->n++){
		case 0:
			return (Val*)&xtn->link;
		default:
			return GCiterdone;
		}
	case Txaccess:
		switch(ictx->n++){
		case 0:
			return (Val*)&xtn->link;
		case 1:
			return (Val*)&xtn->get;
		case 2:
			return (Val*)&xtn->put;
		default:
			return GCiterdone;
		}
	}
	return 0;
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

static void*
mapmem(u64 sz)
{
	void *p;
	p = mmap(0, sz, PROT_READ|PROT_WRITE|PROT_EXEC,
		 MAP_ANON|MAP_PRIVATE, -1, 0);
	if(p == MAP_FAILED)
		fatal("out of memory");
	if((uintptr_t)p%Segsize)
		fatal("unaligned segment");
	return p;
}

static void
unmapmem(void *a, u64 sz)
{
	if(0 > munmap(a, sz))
		fatal("munmap: %s", strerror(errno));
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

void
gcstats()
{
	printf(" inuse = %10" PRIu64 "\n", H.inuse);
	printf("  free = %10" PRIu64 "\n", H.free);
	printf("heapsz = %10" PRIu64 "\n", H.heapsz);
	printf(" bigsz = %10" PRIu64 "\n", H.bigsz);
	printf("smapsz = %10" PRIu64 "\n", H.smapsz);
	printf("segmap = %10" PRIu64 "\n",
	       (segmap.hi-segmap.lo)/Segsize*sizeof(Seg));
	printf(" ncard = %10" PRIu32 "\n", stats.ncard);
	printf(" nlock = %10" PRIu32 "\n", stats.nlock);
}

#define max(a,b) ((a)>(b)?(a):(b))

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
//		printf("growing free area from %ld to ... ", H.free);
		sz1 = H.free < LORES*H.inuse ? (TARGRES-1)*H.inuse : 0;
		sz2 = H.heapsz < Minheap ? Minheap-H.heapsz : 0;
		grow(max(sz1, sz2));
//		printf("%ld\n", H.free);
//		printf("\n");
	}else if(H.heapsz > Minheap && H.free > HIRES*H.inuse){
//		printf("shrinking free area from %ld to ... ", H.free);
		shrink(TARGRES*H.inuse);
//		printf("%ld\n", H.free);
		if(H.heapsz > Minheap && H.free > HIRES*H.inuse)
			fatal("bug");
//		printf("\n");
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
	s->card = Clean;
	memset(s->a, 0, Segsize);
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
	s->card = Clean;
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
		s->card = Clean;
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
	case Mcode:
	case Mweak:
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

static void*
minit(M *m, Seg *s)
{
	s->link = 0;
	m->h = s2a(s);
	m->t = s2a(s);
	m->scan = s2a(s);
	return s2a(s);
}

static void*
minsert(M *m, Seg *s)
{
	Seg *t;
	void *p;

	if(m->h == 0)
		return minit(m, s);
	s->link = 0;
	t = a2s(m->t);
	p = s2a(s);
	t->link = p;
	m->t = p;
	return p;
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

Head*
malweak()
{
	Seg *s;
	Head *h;
	u32 sz;
	sz = qs[Qpair].sz;
again:
	s = a2s(H.w);
	if(s->a+sz <= s->e-1){
		h = s->a;
		s->a += sz;
		Vsetkind(h, Qpair);
		return h;
	}
	H.w = minsert(&H.m[MTweak][H.tg], allocseg(MTweak, H.tg));
	goto again;
}

Head*
malcode()
{
	Seg *s;
	Head *h;
	u32 sz;
	sz = qs[Qcode].sz;
again:
	s = a2s(H.c);
	if(s->a+sz <= s->e-1){
		h = s->a;
		s->a += sz;
		Vsetkind(h, Qcode);
		return h;
	}
	H.c = minsert(&H.m[MTcode][H.tg], allocseg(MTcode, H.tg));
	goto again;
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
_mal(u64 sz)
{
	Seg *s;
	void *h;
again:
	s = a2s(H.d);
	if(s->a+sz <= s->e-1){
		h = s->a;
		s->a += sz;
		return h;
	}
	H.d = minsert(&H.m[MTdata][H.tg], allocseg(MTdata, H.tg));
	goto again;
}

Head*
malv(Qkind kind, Imm len)
{
	Head *h;
	if(len > Seguse)
		h = _malbig(MTbigdata, roundup(len, Align));
	else
		h = _mal(roundup(len, Align));
	Vsetkind(h, kind);
	return h;
}

Head*
malq(Qkind kind)
{
	Head *h;
	h = _mal(qs[kind].sz);
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
	Str *s;
	Rec *r;
	Vec *v;
	switch(Vkind(h)){
	case Qstr:
		s = (Str*)h;
		switch(s->skind){
		case Sheap:
			return roundup(strsize(s->len), Align);
		case Smalloc:
			return sizeof(Strmalloc);
		case Smmap:
			return sizeof(Strmmap);
		case Sperm:
			return sizeof(Strperm);
		}
		fatal("bug");
	case Qvec:
		v = (Vec*)h;
		return roundup(vecsize(v->len), Align);
	case Qrec:
		r = (Rec*)h;
		return roundup(recsize(r->nf), Align);
	default:
		return qs[Vkind(h)].sz;
	}
	fatal("bug");
}

static u8
copy(Val *v)
{
	Head *h;
	Seg *s, *es;
	Imm sz;
	Head *nh;
	unsigned dbg = alldbg;

	h = *v;
	if(h == 0)
		return Clean;
	if((uintptr_t)h&1)
		return Clean; // stack immediate
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
	if(!MTold(s->mt)){
		if(dbg)printf("copy: object %p not in from space (gen %d)\n",
			      h, s->gen);
		return s->gen; // objects in older generations do not move
	}
	if(MTbig(s->mt)){
		minsert(&H.m[s->mt][H.tg], s);
		es = a2s(s->e);
		while(s <= es){
			MTclrold(s->mt);
			s->gen = H.tg;
			s++;
		}
		return H.tg;
	}
	sz = qsz(h);
	switch(Vkind(h)){
	case Qcode:
		nh = malcode();
		break;
	case Qpair:
		if(isweak(h))
			nh = malweak();
		else
			nh = malq(Qpair);
		break;
	case Qstr:
	case Qvec:
	case Qrec:
		nh = malv(Vkind(h), sz);
		break;
	default:
		nh = malq(Vkind(h));
		break;
	}
	if(dbg)printf("copy %s %p to %p\n",
		      qs[Vkind(h)].id,
		      h, nh);
	memcpy(nh, h, sz);
	Vsetfwd(h, (uintptr_t)nh);
	if(dbg)printf("set fwd %p -> %p %p (%d)\n",
		    h, Vfwdaddr(h), nh, (int)Vfwd(h));
	*v = nh;
	return H.tg;
}

static u8
scan1(Head *h)
{
	Ictx ictx;
	Head **c;
	u8 min, g;
	unsigned dbg = alldbg;

	min = Clean;
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
	do{
		again = 0;
		for(i = 0; i < Nmt; i++)
			again |= scan(&H.m[i][tg]);
	}while(again);
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
	setcar(x, Xnil);
	setcdr(x, Xnil);
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
		if(final == Xnil)
			break;
		w = final;
		while(w != Xnil){
			copy(&caar(w));
			push1guard(caar(w), curaddr(cdar(w)));
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
		p = cdr(p);
	}
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
	s->card = Dirty;
}

static void
toproot(void *u, char *k, void *v)
{
	Val *p;
	p = v;
	copy(v);
}

static void
toprd(void *u, void *k, void *v)
{
	Val *p;
	p = v;
	copy(v);
}

static void
copystack(VM *vm)
{
	Imm pc, fp, sp, narg, m, i, clx;
	u64 sz, mask;
	Closure *cl;

	fp = vm->fp;
	if(fp == 0)
		return;
	pc = vm->pc;
	sp = vm->sp;
	cl = vm->clx;
	while(fp != 0){
		if(pc < 2)
			fatal("no way to find livemask pc %llu", pc);
		if(cl->code->insn[pc-1].kind != Ilive
		   || cl->code->insn[pc-2].kind != Ilive)
			fatal("no live mask for pc %d cl %p", pc, cl);
		sz = cl->code->insn[pc-1].cnt;
		mask = cl->code->insn[pc-2].cnt;
		if(fp-sp < sz)
			fatal("frame size is too large fp %llu sp %llu",
			      fp, sp);
		m = fp-1;
		for(i = 0; i < sz; i++){
			if((mask>>i)&1)
				copy(&vm->stack[m]);
			m--;
		}
		for(i = 0; i < fp-sp-sz; i++){
			copy(&vm->stack[m]);
			m--;
		}
		narg = stkimm(vm->stack[fp]);
		pc = stkimm(vm->stack[fp+narg+1]);
		clx = fp+narg+2;
		cl = valcl(vm->stack[fp+narg+2]);
		sp = fp;
		fp = stkimm(vm->stack[fp+narg+3]);
	}
	// initial frame of stack
	for(i = 0; i < narg; i++)
		copy(&vm->stack[sp+1+i]);
	copy(&vm->stack[clx]);
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

static u8
scancard(Seg *s)
{
	void *p;
	u8 min, g;

	if(MTold(s->mt))
		fatal("wtf?");
	stats.ncard++;
	min = Clean;
	p = s2a(s);
	while(p < s->a){
		if(!Vdead((Head*)p)){
			g = scan1(p);
			if(g < min)
				min = g;
		}
		p += qsz(p);
	}
	return min;
}

/* FIXME: is the real predicate here that the segment is not old? */
static void
scan1card(Seg *s, u32 g)
{
	u8 sg;
	if(s->gen <= g)
		return;
	if(s->gen == Glock)
		/* no need to scan: scanlocked will do it */
		return;
	if(s->card > g)
		return;
	sg = scancard(s);
	if(sg < s->gen)
		s->card = sg;
	else
		s->card = Clean;
}

static void
scancards(u32 g)
{
	Seg *s, *es;
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			scan1card(s, g);
		s = nextseg(s);
	}
}

static void
scanunlocked()
{
	Pair *p;
	p = H.unlocked;
	while(p != (Pair*)Xnil){
		if(!Vfwd(car(p)))
			scan1(car(p));
		p = (Pair*)cdr(p);
	}
	H.unlocked = (Pair*)Xnil;
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
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			scan1locked(s);
		s = nextseg(s);
	}
}

static void
updateweak(u32 tg)
{
	M *m;
	Seg *s;
	Pair *p;

	m = &H.m[MTweak][tg];
	s = lookseg(m->h);
	while(1){
		p = s2a(s);
		while((void*)p < s->a){
			if(Vfwd(car(p)))
				_setcar(p, Vfwdaddr(car(p)));
			else
				_setcar(p, Xnil);
			p++;
		}
		if(s->link == 0)
			break;
		s = a2s(s->link);
	}
}

static void
promote1locked(Seg *s)
{
	if(s->nprotect == 0)
		return;
	s->gen = Glock;
	MTclrold(s->mt);
}

static void
promotelocked()
{
	Seg *s, *es;
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			promote1locked(s);
		s = nextseg(s);
	}
}

static void
reloccode(Code *c)
{
	u32 i;
	u64 b;
	u64 *p;
	void **a;
	p = c->reloc;
	b = (u64)c->insn;
	for(i = 0; i < c->nreloc; i++){
		a = (void**)(b+p[i]);
		*a = curaddr(*a);
	}
}

static void
reloc1(Seg *s, u32 tg)
{
	Code *c;
	Head *h, *p;

	if(s->mt == MTbigcode)
		fatal("unimplemented");
	if(s->mt != MTcode)
		return;
	if(s->gen == tg){
		c = s2a(s);
		while((void*)c < s->a){
			if(!Vdead((Head*)c))
				reloccode(c);
			c++;
		}
		return;
	}
	if(s->gen == Glock){
		p = (Head*)s->p;
		while(p){
			h = cdar(p);
			if(!Vdead(h))
				reloccode((Code*)h);
			p = cdr(p);
		}
		return;
	}
}

static void
reloc(u32 tg)
{
	Seg *s, *es;
	s = a2s(segmap.lo);
	es = a2s(segmap.hi);
	while(s < es){
		if(isliveseg(s))
			reloc1(s, tg);
		s = nextseg(s);
	}
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

static void*
resetalloc(MT mt, Gen g)
{
	return minit(&H.m[mt][g], allocseg(mt, g));
}

static void*
getalloc(MT mt, Gen g)
{
	if(H.m[mt][g].h)
		return H.m[mt][g].t;
	else
		return resetalloc(mt, g);
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

	memset(&stats, 0, sizeof(stats));
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

	markold(g);
	for(i = 0; i <= g; i++)
		for(mt = 0; mt < Nmt; mt++)
			mclr(&H.m[mt][i]);

	if(g == tg){
		H.d = resetalloc(MTdata, tg);
		H.c = resetalloc(MTcode, tg);
		H.w = resetalloc(MTweak, tg);
	}else{
		H.d = getalloc(MTdata, tg);
		H.c = getalloc(MTcode, tg);
		H.w = getalloc(MTweak, tg);
	}

	vmp = vms;
	while(vmp < vms+Maxvms){
		vm = *vmp++;
		if(vm == 0)
			continue;
		copystack(vm);
		for(m = 0; m < vm->edepth; m++)
			copy(&vm->err[m].cl);
		hforeach(vm->top->env->var, toproot, 0);
		hforeachp(vm->top->env->rd, toprd, 0);
		copy(&vm->ac);
		copy(&vm->cl);
		copy((Val*)&vm->clx);
	}
	if(dbg)printf("copied vm roots\n");

	// add per-type guards as roots
	for(i = 0; i < Qnkind; i++)
		copy((Val*)&H.guards[i]);
	if(dbg)printf("copied guard roots\n");
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
	reloc(tg);
	if(dbg)printf("did reloc\n");

	// call built-in finalizers
	for(i = 0; i < Qnkind; i++)
		if(H.guards[i])
			while((h = pop1guard(H.guards[i]))){
				if(dbg)printf("freeing object %p (%s)\n",
					      h, qs[Vkind(h)].id);
				Vsetdead(h, 1);
				qs[i].free1(h);
			}
	recycle();
	if(dbg)printf("did recycle\n");

	if(H.tg != 0){
		H.tg = 0;
		H.d = resetalloc(MTdata, H.tg);
		H.c = resetalloc(MTcode, H.tg);
		H.w = resetalloc(MTweak, H.tg);
	}
	H.na = 0;
	if(dbg)printf("end of collection\n");
	H.ingc--;
	maintain();
	if(dbg)printf("gc returning\n");
}

void
gc(VM *vm)
{
	int i;
	u32 g, tg;

	H.gctrip++;
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
				cv->val++;
				return h;
			}
			p = (Pair*)cdr(p);
		}
		fatal("lost track of locked object");
	}else{
		s->p = cons(cons(mkvalcval(0, 0, 1), h), s->p);
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
			cv->val--;
			if(cv->val == 0){
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
	H.d = resetalloc(MTdata, 0);
	H.c = resetalloc(MTcode, 0);
	H.w = resetalloc(MTweak, 0);
	H.na = 0;
	H.ma = GCthresh;

	/* we need nil now to initialize the guarded object lists */
	Xnil = gclock(malq(Qnil));

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
	u32 i;

	_gc(Ngen-1, Ngen-1);  // hopefully free all outstanding objects
	for(i = 0; i < Qnkind; i++)
		H.guards[i] = 0;
	_gc(Ngen-1, Ngen-1);  // hopefully free the guardians
	/* FIXME: free all segments */
	/* FIXME: free static generation */
}
