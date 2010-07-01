#include "sys.h"
#include "util.h"
#include "syscqct.h"

typedef
enum
{
	Mpersist = 1,
	Mmal,
	Mmax,
} Mkind;

enum
{
	Segsize = 64*1024,
	Segmask = ~(Segsize-1)
};

typedef struct Seg Seg;
struct Seg
{
	void *addr, *scan, *a, *e;
	Mkind kind;
	Seg *link;
};

typedef
struct Qtype
{
	char *id;
	u32 sz;
	u32 clearit;
	Freeheadfn free1;
	Head* (*iter)(Head *hd, Ictx *ictx);
} Qtype;

typedef
struct Heap
{
	Seg *m;
	Seg *p;
} Heap;

static int freecl(Head*);
static int freefd(Head*);
static int freerec(Head*);
static int freestr(Head*);
static int freetab(Head*);
static int freevec(Head*);

static Head *iteras(Head*, Ictx*);
static Head *iterbox(Head*, Ictx*);
static Head *itercl(Head*, Ictx*);
static Head *itercode(Head*, Ictx*);
static Head *itercval(Head*, Ictx*);
static Head *iterdom(Head*, Ictx*);
static Head *iterfd(Head*, Ictx*);
static Head *iterrd(Head*, Ictx*);
static Head *iterrec(Head*, Ictx*);
static Head *iterns(Head*, Ictx*);
static Head *iterpair(Head*, Ictx*);
static Head *iterrange(Head*, Ictx*);
static Head *itertab(Head*, Ictx*);
static Head *itervec(Head*, Ictx*);
static Head *iterxtn(Head*, Ictx*);

static Qtype qs[Qnkind] = {
	[Qas]	 = { "as", sizeof(As), 1, 0, iteras },
	[Qbox]	 = { "box", sizeof(Box), 0, 0, iterbox },
	[Qcval]  = { "cval", sizeof(Cval), 0, 0, itercval },
	[Qcl]	 = { "closure", sizeof(Closure), 1, freecl, itercl },
	[Qcode]	 = { "code", sizeof(Code), 1, freecode, itercode },
	[Qdom]	 = { "domain", sizeof(Dom), 0, 0, iterdom },
	[Qfd]	 = { "fd", sizeof(Fd), 0, freefd, iterfd },
	[Qlist]	 = { "list", sizeof(List), 0, freelist, iterlist },
	[Qns]	 = { "ns", sizeof(Ns), 1, 0, iterns },
	[Qpair]	 = { "pair", sizeof(Pair), 0, 0, iterpair },
	[Qrange] = { "range", sizeof(Range), 0, 0, iterrange },
	[Qrd]    = { "rd", sizeof(Rd), 0, 0, iterrd },
	[Qrec]	 = { "record", sizeof(Rec), 0, freerec, iterrec },
	[Qstr]	 = { "string", sizeof(Str), 1, freestr, 0 },
	[Qtab]	 = { "table",  sizeof(Tab), 1, freetab, itertab },
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

static Head*
iteras(Head *hd, Ictx *ictx)
{
	/* FIXME: is it really necessary
	   to mark dispatch and the other
	   cached functions? */
	As *as;
	as = (As*)hd;
	switch(ictx->n++){
	case 0:
		return (Head*)as->mtab;
	case 1:
		return (Head*)as->name;
	case 2:
		return (Head*)as->get;
	case 3:
		return (Head*)as->put;
	case 4:
		return (Head*)as->map;
	case 5:
		return (Head*)as->dispatch;
	default:
		return GCiterdone;
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
	if(ictx->n > 0)
		return GCiterdone;
	ictx->n++;
	return (Head*)(code->konst);
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

static Head*
iterfd(Head *hd, Ictx *ictx)
{
	Fd *fd;
	fd = (Fd*)hd;
	switch(ictx->n++){
	case 0:
		return (Head*)fd->name;
	case 1:
		if(fd->flags&Ffn)
			return GCiterdone;
		return (Head*)fd->u.cl.close;
	case 2:
		return (Head*)fd->u.cl.read;
	case 3:
		return (Head*)fd->u.cl.write;
	default:
		return GCiterdone;
	}
}

static Head*
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
	case 6:
		return (Head*)ns->dispatch;
	case lastfield:
		return (Head*)ns->mtab;
	}
	n -= lastfield;
	if(n >= Vnbase) /* assume elements at+above nbase are aliases */
		return GCiterdone;
	return (Head*)ns->base[n];
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

static Head*
iterrd(Head *hd, Ictx *ictx)
{
	Rd *rd;
	rd = (Rd*)hd;
	switch(ictx->n++){
	case 0:
		return (Head*)rd->name;
	case 1:
		return (Head*)rd->fname;
	case 2:
		return (Head*)rd->is;
	case 3:
		return (Head*)rd->mk;
	case 4:
		return (Head*)rd->fmt;
	case 5:
		return (Head*)rd->get;
	case 6:
		return (Head*)rd->set;
	default:
		return GCiterdone;
	}
}

static Head*
iterrec(Head *hd, Ictx *ictx)
{
	Rec *r;
	r = (Rec*)hd;
	if(ictx->n < r->nf)
		return valhead(r->field[ictx->n++]);
	switch(ictx->n-r->nf){
	case 0:
		ictx->n++;
		return (Head*)r->rd;
	default:
		return GCiterdone;
	}
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
	if(tab->weak){
		/* skip ahead */
		ictx->n = nxt;
		return 0;
	}
	idx = ictx->n++;
	return valhead(x->key[idx]);
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
			return valhead(xtn->attr);
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
			return valhead(xtn->cnt);
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
	case Txaccess:
		switch(ictx->n++){
		case 0:
			return (Head*)xtn->link;
		case 1:
			return (Head*)xtn->get;
		case 2:
			return (Head*)xtn->put;
		default:
			return GCiterdone;
		}
	}
	return 0;
}

static Seg*
mkseg(Mkind kind)
{
	Seg *s;
	s = emalloc(sizeof(Seg));
	s->addr = emalloc(Segsize);
	s->a = s->scan = s->addr;
	s->e = s->addr+Segsize;
	s->kind = kind;
	hputp(segtab, s->addr, s);
	return s;
}

static void
freeseg(Seg *s)
{
	hdelp(segtab, s);
	efree(s->addr);
	efree(s);
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

void
gcpoll()
{
}

void
gcwb(Val v)
{
}

static void
copy(Head *h)
{
	Seg *s;
	s = lookseg(h);
	if(s->kind == Mpersist)
		return;
}

void
gc(VM *vm)
{

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
	m = H.m;
	goto again;
}

void
initmem()
{
	segtab = mkhtp();
	H.m = mkseg(Mmal);
	H.p = mkseg(Mpersist);
}

void
finimem()
{
	freeht(segtab);
}
