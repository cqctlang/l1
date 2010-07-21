#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
enum
{
	Mmal,
	Mcode,
	Mmax,
} Mkind;

enum
{
	Segsize = 4096,
	Segmask = ~(Segsize-1),
	GCthresh = 1024*Segsize,
};

typedef struct Seg Seg;
struct Seg
{
	void *addr, *scan, *a, *e;
	Mkind kind;
	Pair *g;		/* protected objects */
	u32 nprotect;
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
struct Heap
{
	Seg *m;			/* current alloc segment */
	u64 na;			/* allocated bytes since last gc */
	u64 ta;			/* allocated bytes since beginning */
	u64 inuse;		/* currently allocated bytes */
	u64 ma;			/* allocated bytes threshold */
	Seg *t;			/* head of alloc segments */
	Seg *p;			/* head of protect segments */
	Seg *c, *cc;		/* head and current code segments */
	Pair *g;		/* guarded objects */
	Pair *guards[Qnkind];
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

static void
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
	if(tab->weak){
		/* skip ahead */
		ictx->n = nxt;
		return 0;
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
	s->kind = kind;
	hputp(segtab, s->addr, s);
	H.na += Segsize;
	H.ta += Segsize;
	H.inuse += Segsize;
	return s;
}

static void
freeseg(Seg *s)
{
	hdelp(segtab, s);
	munmap(s->addr, Segsize);
	efree(s);
	H.inuse -= Segsize;
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

u64
meminuse()
{
	return H.inuse;
}

Head*
malcode()
{
	Seg *m;
	Head *h;
	u32 sz;
	sz = qs[Qcode].sz;
again:
	m = H.cc;
	if(m->a+sz <= m->e){
		h = m->a;
		m->a += sz;
		Vsetkind(h, Qcode);
		return h;
	}
	H.cc = mkseg(Mcode);
	m->link = H.cc;
	goto again;
}

Head*
mal(Qkind kind)
{
	Seg *m;
	Head *h;
	u32 sz;
	sz = qs[kind].sz;
again:
	m = H.m;
	if(m->a+sz <= m->e){
		h = m->a;
		m->a += sz;
		Vsetkind(h, kind);
		return h;
	}
	H.m = mkseg(Mmal);
	m->link = H.m;
	goto again;
}

static void*
curaddr(Val v)
{
	if(Vfwd(v))
		return Vfwdaddr(v);
	return v;
}

static void
copy(Val *v)
{
	Head *h;
	Seg *s;
	u32 sz;
	Head *nh;

	h = *v;
	if(h == 0)
		return;
	if((uintptr_t)h&1)
		/* pointer tag: stack immediate */
		return;
	if(Vfwd(h)){
		// printf("read fwd %p -> %p\n", h, (void*)Vfwdaddr(h));
		*v = Vfwdaddr(h);
		return;
	}
	if(Vprot(h))
		return;
	s = lookseg(h);
	sz = qs[Vkind(h)].sz;
	if(Vkind(h) == Qcode)
		nh = malcode();
	else
		nh = mal(Vkind(h));
	memcpy(nh, h, sz);
	Vsetfwd(h, (uintptr_t)nh);
//	printf("set fwd %p -> %p %p (%d)\n", h, Vfwdaddr(h), nh, (int)Vfwd(h));
	*v = nh;
}

static void
scan1(Head *h)
{
	Ictx ictx;
	Head **c;
	if(qs[Vkind(h)].iter == 0)
		return;
	memset(&ictx, 0, sizeof(ictx));
	while(1){
		c = qs[Vkind(h)].iter(h, &ictx);
		if(c == (Val*)GCiterdone)
			break;
		copy(c);
	}
}

static void
scan(Seg *s)
{
	Head *h, **c;
	Ictx ictx;

	if(s == 0)
		return;
	while(s->scan < s->a){
		h = s->scan;
		s->scan += qs[Vkind(h)].sz;
		if(qs[Vkind(h)].iter == 0)
			continue;
		memset(&ictx, 0, sizeof(ictx));
		while(1){
			c = qs[Vkind(h)].iter(h, &ictx);
			if(c == (Val*)GCiterdone)
				break;
			copy(c);
		}
	}
	scan(s->link);
}

static Pair*
cons(void *a, void *d)
{
	Pair *p;
	p = (Pair*)mal(Qpair);
	p->car = a;
	p->cdr = d;
	return p;
}

static Pair*
lastpair()
{
	return cons(0, 0);
}

Pair*
mkguard()
{
	Pair *p;
	p = lastpair();
	return cons(p, p);
}

void
instguard(Val o, Pair *t)
{
	H.g = cons(cons(o, t), H.g);
}

Head*
pop1guard(Pair *t)
{
	Pair *x;
	Head *y;
	if(t->car == t->cdr)
		return 0;
	x = (Pair*)t->car;
	y = x->car;
	t->car = x->cdr;
	x->car = 0;
	x->cdr = 0;
	return y;
}

void
push1guard(Val o, Pair *t)
{
	Pair *p;
	p = lastpair();
	((Pair*)t->cdr)->car = o;
	((Pair*)t->cdr)->cdr = (Head*)p;
	t->cdr = (Head*)p;
}

void
quard(Val o)
{
	Pair *t;
	t = H.guards[Vkind(o)];
	if(t == 0)
		fatal("bug");
	instguard(o, t);
}

static void
updateguards()
{
	Pair *phold, *pfinal, *final, *p, *q, **r, *w;
	Head *o;
	Seg *b;

	// move guarded objects and guards (and their containing cons) to
	// either pending hold or pending final list
	phold = 0;
	pfinal = 0;
	p = H.g;
	while(p){
		q = (Pair*)p->cdr;
		o = ((Pair*)p->car)->car;
		if(Vfwd(o) || Vprot(o)){
			// object is accessible
			p->cdr = (Head*)phold;
			phold = p;
		}else{
			// object is inaccessible
			// printf("inaccessible: %p\n", ((Pair*)p->car)->car);
			p->cdr = (Head*)pfinal;
			pfinal = p;
		}
		p = q;
	}
	H.g = 0;

	// move each pending final to final if guard is accessible
	while(1){
		final = 0;
		r = &pfinal;
		p = pfinal;
		if(p == 0)
			break;
		q = (Pair*)p->cdr;
		o = ((Pair*)p->car)->cdr;
		if(Vfwd(o) || Vprot(o)){
			// guard is accessible
			*r = q;
			p->cdr = (Head*)final;
			final = p;
		}
		if(final == 0)
			break;
		b = H.m;
		w = final;
		while(w){
			copy(&((Pair*)w->car)->car);
			push1guard(((Pair*)w->car)->car,
				   curaddr(((Pair*)w->car)->cdr));
			w = (Pair*)w->cdr;
		}
		scan(b);
		r = (Pair**)&p->cdr;
		p = q;
	}

	// forward pending hold to fresh guarded list
	p = phold;
	while(p){
		o = ((Pair*)p->car)->cdr;
		if(Vfwd(o) || Vprot(o))
			// ...
			instguard(curaddr(((Pair*)p->car)->car),
				  curaddr(((Pair*)p->car)->cdr));
		p = (Pair*)p->cdr;
	}
}

void
gcpoll()
{
	if(H.na >= H.ma)
		gc();
}

void
gcwb(Val v)
{
}

static void
toproot(void *u, char *k, void *v)
{
	Val *p;
	p = v;
	// printf("toproot %20s %p %p\n", k, p, *p);
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
	Head *h;
	Pair *g;
	Code *p;

	s = H.c;
	while(s){
		p = s->addr;
		while((void*)p < s->a){
			reloc1(p);
			p++;
		}
		s = s->link;
	}

	s = H.p;
	while(s){
		g = s->g;
		while(g){
			h = g->car;
			if(Vkind(h) == Qcode)
				reloc1((Code*)h);
			g = (Pair*)g->cdr;
		}
		s = s->link;
	}
}

void
gc()
{
	u32 i, m;
	VM **vmp, *vm;
	Seg *s, *t, *f, *c, *b, **r;
	Head *h;
	Pair *g;

	f = H.t;
	c = H.c;
	H.t = H.m = mkseg(Mmal);
	H.c = H.cc = mkseg(Mcode);

	vmp = vms;
	while(vmp < vms+Maxvms){
		vm = *vmp++;
		if(vm == 0)
			continue;
		for(m = vm->sp; m < Maxstk; m++)
			copy(&vm->stack[m]);
		for(m = 0; m < vm->edepth; m++)
			copy(&vm->err[m].cl);
		hforeach(vm->top->env->var, toproot, 0);
		// FIXME: vm->top->env->rd
		copy(&vm->ac);
		copy(&vm->cl);
		copy((Val*)&vm->clx);
	}
	for(i = 0; i < Qnkind; i++)
		copy((Val*)&H.guards[i]);

	scan(H.t);

	// scan code (FIXME: why can't this simply be done before scan(H.t)?
	b = H.m;
	scan(H.c);
	scan(b);

	// reserve segments with newly protected objects
	r = &f;
	s = *r;
	while(s){
		t = s->link;
		if(s->nprotect){
			*r = s->link;
			s->link = H.p;
			H.p = s;
		}else
			r = &s->link;
		s = t;
	}

	r = &c;
	s = *r;
	while(s){
		t = s->link;
		if(s->nprotect){
			*r = s->link;
			s->link = H.p;
			H.p = s;
		}else
			r = &s->link;
		s = t;
	}


	// scan protected objects
	b = H.m;
	s = H.p;
	while(s){
		copy((Val*)&s->g);      // retain list of protected objects!
		g = s->g;
		while(g){
			scan1(g->car);  // manual scan of protected object
			g = (Pair*)g->cdr;
		}
		s = s->link;
	}
	scan(b);

	updateguards();

	reloc();

	// call built-in finalizers
	for(i = 0; i < Qnkind; i++){
		if(H.guards[i] == 0)
			continue;
		while(1){
			h = pop1guard(H.guards[i]);
			if(h == 0)
				break;
			// printf("finalizing %s %p\n", qs[i].id, h);
			qs[i].free1(h);
		}
	}

	// free remaining from and code segments
	s = f;
	while(s){
		f = s->link;
		freeseg(s);
		s = f;
	}
	s = c;
	while(s){
		c = s->link;
		freeseg(s);
		s = c;
	}


	// free unused protected segments
	r = &H.p;
	s = *r;
	while(s){
		t = s->link;
		if(s->nprotect == 0){
			*r = s->link;
			freeseg(s);
		}else
			r = &s->link;
		s = t;
	}

	H.na = 0;
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
	s->g = cons(h, s->g);
	s->nprotect++;
	Vsetprot(h, 1);
	return h;
}

void*
gcunprotect(void *v)
{
	Seg *s;
	Head *h;
	Pair *p, **r;

	if(v == 0)
		return v;
	h = v;
	if(Vfwd(h))
		fatal("bug");
	if(!Vprot(h))
		fatal("gcunprotect on already unprotected object %p", h);
	s = lookseg(h);
	r = &s->g;
	p = *r;
	while(p){
		if(p->car == h){
			*r = (Pair*)p->cdr;
			break;
		}
		r = (Pair**)&p->cdr;
		p = *r;
	}
	s->nprotect--;
	Vsetprot(h, 0);
	return h;
}

void
initmem(u64 gcrate)
{
	u32 i;
	segtab = mkhtp();
	H.t = H.m = mkseg(Mmal);
	H.c = H.cc = mkseg(Mcode);
	H.p = 0;
	H.na = H.ta = 0;
	if(gcrate)
		H.ma = gcrate;
	else
		H.ma = GCthresh;
	for(i = 0; i < Qnkind; i++)
		if(qs[i].free1)
			H.guards[i] = mkguard();
}

void
finimem()
{
	freeht(segtab);
	// FIXME: release all segments
}
