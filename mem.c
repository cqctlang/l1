#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
enum
{
	Mdata,
	Mcode,
	Nm,
} Mkind;

enum
{
	Segsize = 4096,
	Segmask = ~(Segsize-1),
	GCthresh = 10*1024*Segsize,
//	GCthresh = Segsize,
	GCradix = 4,
};

typedef
enum
{
	G0,
	G1,
	G2,
	G3,
	Ngen,
	Gprot,
	Gfrom,
	Gjunk,
	Clean=0xff,
	Dirty=0x00,
} Gen;

typedef struct Seg Seg;
struct Seg
{
	void *addr, *scan, *a, *e;
	u8 card;
	Pair *p;		/* protected objects */
	u32 nprotect;
	Gen gen;
	Seg *link;
};

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
struct GCstat
{
	u64 na;
	u64 ta;
	u64 inuse;
	u64 ngc;
	u64 guards;
	u64 nprotect;
	u64 nseg;
} GCstat;

typedef
struct M
{
	Seg *h, *t;		/* segment list head and tail */
	Seg *s;			/* first segment needing scan */
	Gen gen;
} M;

typedef
struct Guard
{
	Pair *gd[Ngen];		/* guarded objects: (obj . guardian) pairs  */
} Guard;

typedef
struct Heap
{
	Seg *d, *c;		/* current data and code segment */
	M m[Nm][Ngen];		/* metatypes */
	M prot;			/* protected segments */
	u32 g, tg;		/* collect generation and target generation */
	u64 na;			/* bytes allocated since last gc */
	u64 ta;			/* bytes allocated since beginning */
	u64 ma;			/* bytes allocated to trigger collect */
	u64 inuse;		/* bytes currently allocated */
	Guard ug;		/* user guard list */
	Guard sg;		/* system guard list */
	Pair *guards[Qnkind];	/* system per-type guardians */
	u64 ngc;		/* number of gcs */
	u64 nseg;		/* number of segments */
	unsigned disable;
	u32 gctrip, gcsched[Ngen];
} Heap;

static int freecl(Head*);
static int freefd(Head*);
static int freerec(Head*);
static int freestr(Head*);
static int freetab(Head*);
static int freevec(Head*);

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
	[Qlist]	 = { "list", sizeof(List), 0, freelist, iterlist },
	[Qnil]	 = { "nil", sizeof(Head), 0, 0, 0 },
	[Qns]	 = { "ns", sizeof(Ns), 1, 0, iterns },
	[Qnull]	 = { "null", sizeof(Head), 0, 0, 0 },
	[Qpair]	 = { "pair", sizeof(Pair), 0, 0, iterpair },
	[Qrange] = { "range", sizeof(Range), 0, 0, iterrange },
	[Qrd]    = { "rd", sizeof(Rd), 0, 0, iterrd },
	[Qrec]	 = { "record", sizeof(Rec), 0, freerec, iterrec },
	[Qstr]	 = { "string", sizeof(Str), 1, freestr, 0 },
	[Qtab]	 = { "table",  sizeof(Tab), 1, freetab, itertab },
	[Qundef] = { "undef", sizeof(Head), 0, 0, 0 },
	[Qvec]	 = { "vector", sizeof(Vec), 0, freevec, itervec },
	[Qxtn]	 = { "typename", sizeof(Xtypename), 1, 0, iterxtn },
};

static HT	*segtab;
static Heap	H;
static unsigned	alldbg = 0;

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
freerec(Head *hd)
{
	Rec *r;
	r = (Rec*)hd;
	efree(r->field);
	return 1;
}

static int
freestr(Head *hd)
{
	Str *str;
	str = (Str*)hd;
	// printf("freestr(%.*s)\n", (int)str->len, str->s);
	switch(str->skind){
	case Smmap:
		xmunmap(str->s, str->mlen);
		break;
	case Smalloc:
		efree(str->s);
		break;
	case Sperm:
		break;
	}
	return 1;
}

void
freetabx(Tabx *x)
{
	efree(x->val);
	efree(x->key);
	efree(x->idx);
	efree(x);
}

static int
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
			efree(pk);
		}
	}
	freetabx(x);
	return 1;
}

static int
freevec(Head *hd)
{
	Vec *vec;
	vec = (Vec*)hd;
	efree(vec->vec);
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
	if(ictx->n > cl->dlen)
		return GCiterdone;
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
		return &pair->car;
	case 1:
		return &pair->cdr;
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
		return &r->field[ictx->n++];
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
		return &x->val[idx];
	}
	idx = ictx->n++;
	return &x->key[idx];
}

static Val*
itervec(Head *hd, Ictx *ictx)
{
	Vec *vec;
	vec = (Vec*)hd;
	if(ictx->n >= vec->len)
		return GCiterdone;
	return &vec->vec[ictx->n++];
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
mapseg()
{
	uintptr_t a;
	void *p;
	p = mmap(0, Segsize, PROT_READ|PROT_WRITE, MAP_ANON|MAP_PRIVATE, -1, 0);
	if(p == MAP_FAILED)
		fatal("out of memory");
	a = (uintptr_t)p;
	if(a&~Segmask)
		fatal("unaligned segment");
	return p;
}

static Seg*
mkseg(Mkind kind)
{
	Seg *s;
	s = emalloc(sizeof(Seg));
	s->addr = mapseg();
	s->a = s->scan = s->addr;
	s->e = s->addr+Segsize;
	s->card = Clean;
	hputp(segtab, s->addr, s);
	H.na += Segsize;
	H.ta += Segsize;
	H.inuse += Segsize;
	H.nseg++;
	return s;
}

static void
freeseg(Seg *s)
{
	hdelp(segtab, s->addr);
	munmap(s->addr, Segsize);
	efree(s);
	H.inuse -= Segsize;
	H.nseg--;
}

static Seg*
lookseg(void *a)
{
	Seg *s;
	uintptr_t v;

	v = (uintptr_t)a;
	v &= Segmask;
	s = hgetp(segtab, (void*)v);
	if(s == 0)
		fatal("lookseg bug");
	return s;
}

static void
mclr(M *m)
{
	m->h = 0;
	m->t = 0;
	m->s = 0;
}

static Seg*
minit(M *m, Seg *s)
{
	s->gen = m->gen;
	s->link = 0;
	m->h = s;
	m->t = s;
	m->s = s;
	return s;
}

static Seg*
minsert(M *m, Seg *s)
{
	if(m->h == 0)
		return minit(m, s);
	s->gen = m->gen;
	s->link = 0;
	m->t->link = s;
	m->t = s;
	return s;
}

static void
mmove(M *a, M *b)
{
	Seg *s;
	s = b->h;
	while(s){
		s->gen = a->gen;
		s = s->link;
	}
	if(a->h == 0){
		a->h = b->h;
		a->t = b->t;
	}else if(b->h){
		a->t->link = b->h;
		a->t = b->t;
	}
	b->h = b->t = 0;
	a->s = b->s = 0; // mmoved Ms are never scanned
}

static void
mfree(M *a)
{
	Seg *s, *t;
	s = a->h;
	while(s){
		t = s->link;
		freeseg(s);
		s = t;
	}
}

u64
protected()
{
	Seg *p;
	u32 i;
	u64 m;

	m = 0;
	for(i = 0; i < Nm; i++){
		p = H.m[i][G0].h;
		while(p){
			m += p->nprotect;
			p = p->link;
		}
	}
	p = H.prot.h;
	while(p){
		m += p->nprotect;
		p = p->link;
	}

	return m;
}

static u64
guarded()
{
	u32 i;
	u64 m;
	Pair *p;
	m = 0;
	for(i = 0; i < Ngen; i++){
		p = H.sg.gd[i];
		while(p != (Pair*)Xnil){
			m++;
			p = (Pair*)p->cdr;
		}
	}
	for(i = 0; i < Ngen; i++){
		p = H.ug.gd[i];
		while(p != (Pair*)Xnil){
			m++;
			p = (Pair*)p->cdr;
		}
	}
	return m;
}

Str*
gcstat()
{
	GCstat s;

	memset(&s, 0, sizeof(s));
	s.na = H.na;
	s.ta = H.ta;
	s.inuse = H.inuse;
	s.ngc = H.ngc;
	s.guards = guarded();
	s.nprotect = protected();
	s.nseg = H.nseg;
	return mkstr((char*)&s, sizeof(s));
}

u64
meminuse()
{
	return H.inuse;
}

Head*
malcode()
{
	Seg *s;
	Head *h;
	u32 sz;
	sz = qs[Qcode].sz;
again:
	s = H.c;
	if(s->a+sz <= s->e){
		h = s->a;
		s->a += sz;
		Vsetkind(h, Qcode);
		return h;
	}
	H.c = minsert(&H.m[Mcode][s->gen], mkseg(Mcode));
	goto again;
}

Head*
mal(Qkind kind)
{
	Seg *s;
	Head *h;
	u32 sz;
	sz = qs[kind].sz;
again:
	s = H.d;
	if(s->a+sz <= s->e){
		h = s->a;
		s->a += sz;
		Vsetkind(h, kind);
		return h;
	}
	H.d = minsert(&H.m[Mdata][s->gen], mkseg(Mdata));
	goto again;
}

static void*
curaddr(Val v)
{
	if(Vfwd(v))
		return Vfwdaddr(v);
	return v;
}

static u8
copy(Val *v)
{
	Head *h;
	Seg *s;
	u32 sz;
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
		s = lookseg(*v);
		return s->gen;
	}
	if(Vprot(h)){
		if(dbg)printf("copy: object %p is protected\n", h);
		return Gprot; // protected objects do not move
	}
	s = lookseg(h);
	if(s->gen != Gfrom){
		if(dbg)printf("copy: object %p not in from space (gen %d)\n",
			      h, s->gen);
		return s->gen; // objects in older generations do not move
	}
	sz = qs[Vkind(h)].sz;
	if(Vkind(h) == Qcode)
		nh = malcode();
	else{
		nh = mal(Vkind(h));
		if(dbg)printf("copy %s %p to %p\n",
			      qs[Vkind(h)].id,
			      h, nh);
	}
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

static unsigned
scan(M *m)
{
	Head *h, **c;
	Seg *s;
	Ictx ictx;
	unsigned dbg = alldbg;

	s = m->s;
	c = 0;
	while(s){
		m->s = s;
		while(s->scan < s->a){
			h = s->scan;
			if(dbg)printf("scanning %p (%s)\n", h, qs[Vkind(h)].id);
			s->scan += qs[Vkind(h)].sz;
			if(qs[Vkind(h)].iter == 0)
				continue;
			memset(&ictx, 0, sizeof(ictx));
			while(1){
				c = qs[Vkind(h)].iter(h, &ictx);
				if(c == (Val*)GCiterdone)
					break;
				if(dbg)printf("iter %p (%s) -> %p %p\n",
					    h, qs[Vkind(h)].id,
					    c, *c);
				copy(c);
			}
		}
		s = s->link;
	}

	// approximate indication of whether a copy occurred in this call:
	//    1: maybe
	//    0: definitely not
	return (c != 0); // approximate
}

static void
kleenescan(u32 tg)
{
	unsigned again, i;
	do{
		again = 0;
		for(i = 0; i < Nm; i++)
			again |= scan(&H.m[i][tg]);
	}while(again);
}

#define car(p)  (((Pair*)p)->car)
#define cdr(p)  (((Pair*)p)->cdr)
#define caar(p) (car(car(p)))
#define cadr(p) (car(cdr(p)))
#define cdar(p) (cdr(car(p)))
#define cddr(p) (cdr(cdr(p)))
#define setcar(p,x) { gcwb((Val)p); car(p) = (Head*)(x); }
#define setcdr(p,x) { gcwb((Val)p); cdr(p) = (Head*)(x); }

static Pair*
cons(void *a, void *d)
{
	Pair *p;
	p = (Pair*)mal(Qpair);
	// setc*r would invoke write barrier
	p->car = a;
	p->cdr = d;
	return p;
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
pop1guard(Pair *t)
{
	Head *x, *y;
	if(car(t) == cdr(t))
		return 0;
	x = car(t);
	y = car(x);
	setcar(t, cdr(x));
	setcar(x, Xnil);
	setcdr(x, Xnil);
	return y;
}

void
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

static int
isfwd(Head *o)
{
	Seg *s;
	if(Vfwd(o))
		return 1;
	s = lookseg(o);
	if(s->gen != Gfrom)  // should this also exclude Gjunk, Gprot, etc.?
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
			if(isfwd(o) || Vprot(o)){
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
			if(isfwd(o) || Vprot(o)){
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
		if(isfwd(o) || Vprot(o))
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
	if(!H.disable && !ingc && H.na >= H.ma){
		ingc++;
		gc(vm);
		ingc--;
	}
}

void
gcwb(Val v)
{
	Seg *s;
	s = lookseg((Head*)v);
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
reloc1(Code *c)
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
reloc()
{
	Seg *s;
	Head *h, *p;
	Code *c;

	s = H.m[Mcode][H.tg].h;
	while(s){
		c = s->addr;
		while((void*)c < s->a){
			if(!Vdead((Head*)c))
				reloc1(c);
			c++;
		}
		s = s->link;
	}

	s = H.prot.h;
	while(s){
		p = (Head*)s->p;
		while(p){
			h = car(p);
			if(Vkind(h) == Qcode && !Vdead(h))
				reloc1((Code*)h);
			p = cdr(p);
		}
		s = s->link;
	}
}

static void
copystack(VM *vm)
{
	Imm pc, fp, sp, narg, m, i, clx;
	u64 sz, mask;
	Closure *cl;

//	fvmbacktrace(vm);
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

static u8
scancard(Seg *s)
{
	void *p;
	u8 min, g;

	min = Clean;
	p = s->addr;
	while(p < s->a){
		if(!Vdead((Head*)p)){
			g = scan1(p);
			if(g < min)
				min = g;
		}
		p += qs[Vkind((Head*)p)].sz;
	}
	return min;
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
	u32 i, mt, m;
	VM **vmp, *vm;
	Seg *s, *t, *tj, *tp, **r;
	Head *h, *p;
	M junk, fr;
	u8 sg;
	unsigned dbg = alldbg;

	if(g != tg && g != tg-1)
		fatal("bug");
	if(tg >= Ngen)
		return; // FIXME: silently do nothing...caller should know
	H.g = g;
	H.tg = tg;

	if(dbg)printf("gc(%u,%u)\n", g, tg);
	mclr(&fr);
	fr.gen = Gfrom;
	for(i = 0; i <= g; i++)
		for(mt = 0; mt < Nm; mt++)
			mmove(&fr, &H.m[mt][i]);

	// move inactive protected segments to Gfrom.
	// do this before we scan pointers to objects on these segments.
	r = &H.prot.h;
	s = *r;
	while(s){
		t = s->link;
		if(s->nprotect == 0){
			minsert(&fr, s);
			*r = t;
		}else
			r = &s->link;
		s = t;
	}

	junk.gen = Gjunk;
	mclr(&junk);
	if(g == tg){
		H.d = minit(&H.m[Mdata][tg], mkseg(Mdata));
		H.c = minit(&H.m[Mcode][tg], mkseg(Mcode));
	}else{
		if(H.m[Mdata][tg].h)
			H.d = H.m[Mdata][tg].t;
		else
			H.d = minit(&H.m[Mdata][tg], mkseg(Mdata));
		if(H.m[Mcode][tg].h)
			H.c = H.m[Mcode][tg].t;
		else
			H.c = minit(&H.m[Mcode][tg], mkseg(Mcode));
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

	// add pointers to collected generations in dirty cards
	for(i = g+1; i < Ngen; i++)
		for(mt = 0; mt < Nm; mt++){
			s = H.m[mt][i].h;
			while(s){
				if(s->card <= g){
					sg = scancard(s);
					if(sg < i)
						s->card = sg;
					else
						s->card = Clean;
				}
				s = s->link;
			}
		}
	if(dbg)printf("scanned cards\n");

	// sweep through roots
	kleenescan(tg);

	// scan protected objects
	s = H.prot.h;
	while(s){
		copy((Val*)&s->p);      // retain list of protected objects!
		p = (Head*)s->p;
		while(p){
			if(dbg)printf("scanning protected object %s %p\n",
				      qs[Vkind(car(p))].id, car(p));
			scan1(car(p));  // manual scan of protected object
			p = cdr(p);
		}
		s = s->link;
	}
	// reserve segments with newly protected objects.
	s = fr.h;
	tj = tp = 0;
	while(s){
		t = s->link;
		if(s->nprotect){
			// retain list of protected objects!
			copy((Val*)&s->p);
			p = (Head*)s->p;
			while(p){
				if(dbg)printf("scan protected object %s %p\n",
					      qs[Vkind(car(p))].id, car(p));
				// manual scan of protected object
				scan1(car(p));
				p = cdr(p);
			}
			s->link = tp;
			tp = s;
		}else{
			// queue for transfer to junk segment list
			s->link = tj;
			tj = s;
		}
		s = t;
	}
	kleenescan(tg);
	if(dbg)printf("re-scanned tg data (after prot)\n");

	updateguards(&H.ug);
	updateguards(&H.sg);
	if(dbg)printf("did updateguards\n");

	reloc();
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

	// stage unused segments for recycling and preserve protected ones
	while(tj){
		t = tj->link;
		minsert(&junk, tj);
		tj = t;
	}
	while(tp){
		t = tp->link;
		minsert(&H.prot, tp);
		tp = t;
	}

	// recycle segments
	s = junk.h;
	while(s){
		t = s->link;
		freeseg(s);
		s = t;
	}

	if(H.tg != 0){
		H.tg = 0;
		H.d = minit(&H.m[Mdata][H.tg], mkseg(Mdata));
		H.c = minit(&H.m[Mcode][H.tg], mkseg(Mcode));
	}
	H.na = 0;
	H.ngc++;
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

void*
gcprotect(void *v)
{
	Seg *s;
	Head *h;

	if(v == 0)
		return v;
	h = v;
	if(Vfwd(h))
		fatal("bug");
	if(Vprot(h))
		// FIXME: with a counter rather than a bit, we could
		// allow this.
		fatal("gcprotect on already protected object %p", h);
	s = lookseg(h);
	s->p = cons(h, s->p);
	s->nprotect++;
	Vsetprot(h, 1);
	return h;
}

void*
gcunprotect(void *v)
{
	Seg *s;
	Head *h, *p, **r;

	if(v == 0)
		return v;
	h = v;
	if(Vfwd(h))
		fatal("bug");
	if(!Vprot(h))
		fatal("gcunprotect on already unprotected object %p", h);
	s = lookseg(h);
	// FIXME: generation safe?
	r = (Head**)&s->p;
	p = *r;
	while(p){
		if(car(p) == h){
			*r = cdr(p);
			break;
		}
		r = &cdr(p);
		p = *r;
	}
	s->nprotect--;
	Vsetprot(h, 0);
	return h;
}

void
initmem(u64 gcrate)
{
	u32 i, mt, gr;

	segtab = mkhtp();
	gr = 1;
	for(i = 0; i < Ngen; i++){
		for(mt = 0; mt < Nm; mt++)
			H.m[mt][i].gen = i;
		H.gcsched[i] = gr;
		gr *= GCradix;
	}
	H.d = minit(&H.m[Mdata][0], mkseg(Mdata));
	H.c = minit(&H.m[Mcode][0], mkseg(Mcode));
	mclr(&H.prot);
	H.prot.gen = Gprot;
	H.na = H.ta = 0;
	if(gcrate)
		H.ma = gcrate;
	else
		H.ma = GCthresh;

	/* we need nil now to initialize the guarded object lists */
	Xnil = gcprotect(mal(Qnil));

	for(i = 0; i < Qnkind; i++)
		if(qs[i].free1)
			H.guards[i] = mkguard();
	for(i = 0; i < Ngen; i++)
		H.ug.gd[i] = H.sg.gd[i] = (Pair*)Xnil;
	H.disable = 0;
}

void
finimem()
{
	u32 i, mt, n;
	Seg *s;

	_gc(Ngen-1, Ngen-1);  // hopefully free all outstanding objects
	for(i = 0; i < Qnkind; i++)
		H.guards[i] = 0;
	_gc(Ngen-1, Ngen-1);  // hopefully free the guardians
	s = H.prot.h;
	n = 0;
	while(s){
		n += s->nprotect;
		s = s->link;
	}

	/* FIXME: we have not freed or unprotected Xnil */
	if(n != 1)
		printf("finimem: %u protected objects (expected 1)!\n", n);

	for(i = 0; i < Ngen; i++)
		for(mt = 0; mt < Nm; mt++)
			mfree(&H.m[mt][i]);
	mfree(&H.prot);
	freeht(segtab);
}
