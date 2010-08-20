#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "x86.h"

char *qname[Qnkind] = {
	[Qundef]=	"undefined",
	[Qnil]=		"nil",
	[Qnull]=	"null",
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
	[Qrec]=		"record",
	[Qrd]=		"record descriptor",
	[Qstr]=		"string",
	[Qtab]=		"table",
	[Qvec]=		"vector",
	[Qxtn]=		"ctype",
};

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

unsigned isunsigned[Vnbase] = {
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

static void vmsetcl(VM *vm, Val val);
static Xtypename* dolooktype(VM *vm, Xtypename *xtn, Ns *ns);
static Xtypename* mkvoidxtn(void);
static Xtypename* mkbasextn(Cbase name, Rkind rep);
static Xtypename* mkptrxtn(Xtypename *t, Rkind rep);
static Xtypename* mkconstxtn(Xtypename *t);
static Xtypename* mktypedefxtn(Str *tid, Xtypename *t);
static Xtypename* mkundefxtn(Xtypename *t);
static Env* mktopenv(void);
static void l1_sort(VM *vm, Imm argc, Val *argv, Val *rv);
static int issym(Vec *sym);
static void setgo(Insn *i, Imm lim);

void *GCiterdone;
Val Xundef;
Val Xnil;
Val Xnulllist;
static Dom *litdom;
static Tab *finals;
static Closure *halt;
Cval *cvalnull, *cval0, *cval1, *cvalminus1;

VM *vms[Maxvms];
static unsigned nvms;
static unsigned long long tick;

static char *opstr[Iopmax+1] = {
	[Iadd] = "+",
	[Iand] = "&",
	[Idiv] = "/",
	[Iinv] = "~",
	[Imod] = "%",
	[Imul] = "*",
	[Ineg] = "-",
	[Inot] = "!",
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

static u32 nohash(Val);
static u32 hashcval(Val);
static u32 hashptr(Val);
static u32 hashconst(Val);
static u32 hashrange(Val);
static u32 hashstr(Val);
static u32 hashvec(Val);
static u32 hashxtn(Val);

static int eqcval(Val, Val);
static int eqptr(Val, Val);
static int eqtrue(Val, Val);

static int equalcval(Val, Val);
static int equallistv(Val a, Val b);
static int equalrange(Val, Val);
static int equalstrv(Val, Val);
static int equalvecv(Val a, Val b);
static int equalxtn(Xtypename*, Xtypename*);
static int equalxtnv(Val, Val);

typedef struct Hashop {
	u32 (*hash)(Val);
	int (*eq)(Val, Val);
} Hashop;

static Hashop hashop[Qnkind] = {
	[Qundef] = { nohash, 0 },
	[Qnil]	 = { hashconst, eqtrue },
	[Qnull]  = { hashconst, eqtrue },
	[Qas]	 = { hashptr, eqptr },
	[Qbox]	 = { nohash, 0 },
	[Qcl]	 = { hashptr, eqptr },
	[Qcode]	 = { nohash, 0 },
	[Qcval]	 = { hashcval, eqcval },
	[Qdom]	 = { hashptr, eqptr },
	[Qfd]	 = { hashptr, eqptr },
	[Qlist]	 = { hashlist, equallistv },
	[Qns]	 = { hashptr, eqptr },
	[Qpair]	 = { hashptr, eqptr },
	[Qrange] = { hashrange, equalrange },
	[Qrd]    = { hashptr, eqptr },
	[Qrec]   = { hashptr, eqptr },
	[Qstr]	 = { hashstr, equalstrv },
	[Qtab]	 = { hashptr, eqptr },
	[Qvec]	 = { hashvec, equalvecv },
	[Qxtn]	 = { hashxtn, equalxtnv },
};

static Code *kcode, *cccode;

Imm
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
	if(Vkind(v) != Qcval)
		fatal("valimm on non-cval");
	cv = (Cval*)v;
	return cv->val;
}

Val
valboxed(Val v)
{
	Box *b;
	if(Vkind(v) != Qbox)
		fatal("valboxed on non-box");
	b = (Box*)v;
	return b->v;
}

Head*
valhead(Val v)
{
	Imm imm;

	if(v == 0)
		return 0;

	imm = (Imm)(uintptr_t)v;
	if((imm&1) != 0)
		/* stack immediate */
		return 0;

	switch(Vkind(v)){
	case Qundef:
	case Qnil:
	case Qnull:
		return 0;
		break;
	default:
		return (Head*)v;
		break;
	}
}

static Imm
typesize(VM *vm, Xtypename *xtn)
{
	Cval *cv;
	Str *es;
	if(xtn == 0)
		vmerr(vm, "attempt to compute size of undefined type");
	switch(xtn->tkind){
	case Tvoid:
		vmerr(vm, "attempt to compute size of void type");
	case Tbase:
		return repsize[xtn->rep];
	case Ttypedef:
		return typesize(vm, xtn->link);
	case Tstruct:
	case Tunion:
		cv = valcval(attroff(xtn->attr));
		return cv->val;
	case Tenum:
		return typesize(vm, xtn->link);
	case Tptr:
		return repsize[xtn->rep];
	case Tarr:
		if(Vkind(xtn->cnt) != Qcval)
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
	case Txaccess:
		return typesize(vm, xtn->link);
	}
	fatal("bug");
	return 0; /* not reached */
}

Code*
newcode()
{
	return (Code*)malcode();
}

Closure*
mkcl(Code *code, unsigned long entry, unsigned len, char *id)
{
	Closure *cl;
	cl = (Closure*)mal(Qcl);
	cl->code = code;
	cl->entry = entry;
	cl->dlen = len;
	cl->display = emalloc(cl->dlen*sizeof(Val));
	cl->id = xstrdup(id);
	quard((Val)cl);
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

Fd*
mkfdfn(Str *name, int flags, Xfd *xfd)
{
	Fd *fd;
//	if(read == 0)
//		flags &= ~Fread;
//	if(write == 0)
//		flags &= ~Fwrite;
	fd = (Fd*)mal(Qfd);
	fd->name = name;
	fd->u.fn = *xfd;
	fd->flags = flags|Ffn;
	quard((Val)fd);
	return fd;
}

Fd*
mkfdcl(Str *name, int flags,
       Closure *read,
       Closure *write,
       Closure *close)
{
	Fd *fd;
	if(read == 0)
		flags &= ~Fread;
	if(write == 0)
		flags &= ~Fwrite;
	fd = (Fd*)mal(Qfd);
	fd->name = name;
	fd->u.cl.read = read;
	fd->u.cl.write = write;
	fd->u.cl.close = close;
	fd->flags = flags&~Ffn;
	quard((Val)fd);
	return fd;
}

int
eqval(Val v1, Val v2)
{
	if(Vkind(v1) != Vkind(v2))
		return 0;
	return hashop[Vkind(v1)].eq(v1, v2);
}

u32
hashval(Val v)
{
	return hashop[Vkind(v)].hash(v);
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
	fatal("bad type of key (%d) to table operation", Vkind(val));
	return 0; /* not reached */
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
	switch(Vkind(val)){
	case Qnil:
		return hashptr32shift(Xnil);
	case Qnull:
		return hashptr32shift(Xnulllist);
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static int
eqtrue(Val a, Val b)
{
	USED(a);
	USED(b);
	return 1;
}

static u32
hashcval(Val val)
{
	Cval *cv;
	cv = valcval(val);
	return hash6432shift(cv->val)^hashxtn(mkvalxtn(cv->type));
}

static int
eqcval(Val a, Val b)
{
	Cval *cva, *cvb;
	cva = valcval(a);
	cvb = valcval(b);
	if(cva->val!=cvb->val)
		return 0;
	return equalxtn(cva->type, cvb->type);
}

static int
equalcval(Val a, Val b)
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
equalrange(Val a, Val b)
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
equalstrc(Str *a, char *b)
{
	if(a->len != strlen(b))
		return 0;
	return memcmp(a->s, b, a->len) ? 0 : 1;
}

static int
equalstr(Str *a, Str *b)
{
	if(a->len != b->len)
		return 0;
	return memcmp(a->s, b->s, a->len) ? 0 : 1;
}

static int
equalstrv(Val a, Val b)
{
	return equalstr(valstr(a), valstr(b));
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
		return hash6432shift(xtn->basename^xtn->rep);
	case Ttypedef:
		return hashstr((Val)xtn->tid)>>xtn->tkind;
	case Tstruct:
	case Tunion:
	case Tenum:
		return hashstr((Val)xtn->tag)>>xtn->tkind;
	case Tundef:
	case Tptr:
		return xtn->rep^hashxtn((Val)xtn->link)>>xtn->tkind;
	case Tarr:
		x = hashxtn((Val)xtn->link)>>xtn->tkind;
		if(Vkind(xtn->cnt) == Qcval)
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
		x ^= hashcval(xtn->cnt);
		return x;
	case Tconst:
		return hashxtn((Val)xtn->link)>>xtn->tkind;
	case Txaccess:
		x = hashxtn((Val)xtn->link)>>xtn->tkind;
		x ^= hashptr((Val)xtn->get);
		x ^= hashptr((Val)xtn->put);
		return x;
	}
	fatal("bug");
	return 0; /* not reached */
}

static int
equalxtn(Xtypename *a, Xtypename *b)
{
	Imm m;

	if(a->tkind != b->tkind)
		return 0;

	switch(a->tkind){
	case Tvoid:
		return 1;
	case Tbase:
		return (a->basename == b->basename) && (a->rep == b->rep);
	case Ttypedef:
		return equalstr(a->tid, b->tid);
	case Tstruct:
	case Tunion:
	case Tenum:
		return equalstr(a->tag, b->tag);
	case Tundef:
	case Tptr:
		return (a->rep == b->rep) && equalxtn(a->link, b->link);
	case Tarr:
		if(Vkind(a->cnt) != Vkind(b->cnt))
			return 0;
		if(Vkind(a->cnt) == Qcval){
			if(!equalcval(a->cnt, b->cnt))
				return 0;
		}
		return equalxtn(a->link, b->link);
	case Tfun:
		if(a->param->len != b->param->len)
			return 0;
		if(!equalxtn(a->link, b->link))
			return 0;
		for(m = 0; m < a->param->len; m++)
			if(!equalxtn(valxtn(vecref(valvec(vecref(a->param, m)),
						   Typepos)),
				     valxtn(vecref(valvec(vecref(b->param, m)),
						   Typepos))))
				return 0;
		return 1;
	case Tbitfield:
		if(!equalcval(a->cnt, b->cnt))
			return 0;
		return equalxtn(a->link, b->link);
	case Tconst:
		return equalxtn(a->link, b->link);
	case Txaccess:
		return (eqptr((Val)a->get, (Val)b->get)
			&& eqptr((Val)a->put, (Val)b->put)
			&& equalxtn(a->link, b->link));
	}
	fatal("bug");
	return 0; /* not reached */
}

static int
equalxtnv(Val a, Val b)
{
	return equalxtn(valxtn(a), valxtn(b));
}

Str*
mkstr0(char *s)
{
	Str *str;
	str = (Str*)mal(Qstr);
	str->len = strlen(s);
	str->s = emalloc(str->len);
	memcpy(str->s, s, str->len);
	str->skind = Smalloc;
	quard((Val)str);
	return str;
}

Str*
mkstr(char *s, Imm len)
{
	Str *str;
	str = (Str*)mal(Qstr);
	str->len = len;
	str->s = emalloc(str->len);
	memcpy(str->s, s, str->len);
	str->skind = Smalloc;
	quard((Val)str);
	return str;
}

Str*
mkstrk(char *s, Imm len, Skind skind)
{
	Str *str;
	str = (Str*)mal(Qstr);
	str->s = s;
	str->len = len;
	str->skind = skind;
	if(skind == Smmap)
		str->mlen = len;
	quard((Val)str);
	return str;
}

Str*
mkstrn(Imm len)
{
	Str *str;
	str = (Str*)mal(Qstr);
#if 0
	if(len >= PAGESZ){
		str->len = len;
		str->s = mmap(0, PAGEUP(len), PROT_READ|PROT_WRITE,
			      MAP_NORESERVE|MAP_PRIVATE|MAP_ANON, -1, 0);
		if(str->s == (void*)(-1))
			vmerr(vm, "out of memory");
		str->mlen = PAGEUP(len);
		str->skind = Smmap;
	}else{
		str->len = len;
		str->s = emalloc(str->len);
		str->skind = Smalloc;
	}
#else
	str->len = len;
	str->s = emalloc(str->len);
	str->skind = Smalloc;
#endif
	quard((Val)str);
	return str;
}

static Str*
strcopy(Str *s)
{
	return mkstr(s->s, s->len);
}

char*
str2cstr(Str *str)
{
	char *s;
	s = emalloc(str->len+1);
	memcpy(s, str->s, str->len);
	return s;
}

Str*
strslice(Str *str, Imm beg, Imm end)
{
	return mkstr(str->s+beg, end-beg);
}

static Str*
strconcat(Str *s1, Str *s2)
{
	Str *s;
	s = mkstrn(s1->len+s2->len);
	memcpy(s->s, s1->s, s1->len);
	memcpy(s->s+s1->len, s2->s, s2->len);
	return s;
}

static int
listlenpair(Val v, Imm *rv)
{
	Imm m;
	Pair *p;

	m = 0;
	while(Vkind(v) == Qpair){
		m++;
		p = valpair(v);
		v = p->cdr;
	}
	if(Vkind(v) != Qnull)
		return 0;
	*rv = m;
	return 1;
}

static int
listlen(Val v, Imm *rv)
{
	List *lst;
	if(Vkind(v) == Qpair || Vkind(v) == Qnull)
		return listlenpair(v, rv);
	if(Vkind(v) == Qlist){
		lst = vallist(v);
		*rv = listxlen(lst->x);
		return 1;
	}
	return 0;
}

static int
equallistv(Val a, Val b)
{
	return equallist(vallist(a), vallist(b));
}

Vec*
mkvec(Imm len)
{
	Vec *vec;
	vec = (Vec*)mal(Qvec);
	vec->len = len;
	vec->vec = emalloc(len*sizeof(Val));
	quard((Val)vec);
	return vec;
}

static Vec*
mkvecinit(Imm len, Val v)
{
	Vec *vec;
	Imm i;

	vec = mkvec(len);
	for(i = 0; i < len; i++)
		vec->vec[i] = v;
	return vec;
}

Val
vecref(Vec *vec, Imm idx)
{
	return vec->vec[idx];
}

void
_vecset(Vec *vec, Imm idx, Val v)
{
	vec->vec[idx] = v;
}

void
vecset(Vec *vec, Imm idx, Val v)
{
	gcwb(vec->vec[idx]);
	_vecset(vec, idx, v);
}

static u32
hashvec(Val v)
{
	Vec *a;
	u32 i, len, m;
	a = valvec(v);
	m = Vkind(v);
	len = a->len;
	for(i = 0; i < len; i++)
		m ^= hashval(a->vec[i]);
	return m;
}

static int
equalvec(Vec *a, Vec *b)
{
	u32 len, m;
	len = a->len;
	if(len != b->len)
		return 0;
	for(m = 0; m < len; m++)
		if(!eqval(a->vec[m], b->vec[m]))
			return 0;
	return 1;
}

static int
equalvecv(Val a, Val b)
{
	return equalvec(valvec(a), valvec(b));
}

static Vec*
veccopy(Vec *old)
{
	Vec *new;
	new = mkvec(old->len);
	memcpy(new->vec, old->vec, old->len*sizeof(Val));
	return new;
}

static Tabx*
mktabx(u32 sz)
{
	Tabx *x;
	x = emalloc(sizeof(Tabx));
	x->sz = sz;
	x->lim = 2*sz/3;
	x->val = emalloc(x->lim*sizeof(Val)); /* must be 0 or Xundef */
	x->key = emalloc(x->lim*sizeof(Val)); /* must be 0 or Xundef */
	x->idx = emalloc(x->sz*sizeof(Tabidx*));
	return x;
}

static Tab*
_mktab(Tabx *x)
{
	Tab *tab;
	tab = (Tab*)mal(Qtab);
	tab->x = x;
	tab->weak = 0;
	quard((Val)tab);
	return tab;
}

Tab*
mktab(void)
{
	return _mktab(mktabx(Tabinitsize));
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
	kind = Vkind(keyv);
	op = &hashop[kind];
	idx = op->hash(keyv)&(x->sz-1);
	p = &x->idx[idx];
	tk = *p;
	while(tk){
		kv = x->key[tk->idx];
		if(Vkind(kv) == kind && op->eq(keyv, kv)){
			if(prev)
				*prev = p;
			return tk;
		}
		p = &tk->link;
		tk = *p;
	}
	return 0;
}

Val
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
			idx = hashop[Vkind(kv)].hash(kv)&(nx->sz-1);
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
dotabput(Tab *tab, Val keyv, Val val)
{
	Tabidx *tk;
	u32 idx;
	Tabx *x;

	x = tab->x;
	tk = _tabget(tab, keyv, 0);
	if(tk){
		gcwb(x->val[tk->idx]);
		x->val[tk->idx] = val;
		return;
	}

	if(x->nxt >= x->lim){
		tabexpand(tab);
		x = tab->x;
	}

	tk = emalloc(sizeof(Tabidx));

	/* FIXME: snapshot-at-beginning seems to imply that it does
	   not matter whether gc can see these new values in this
	   epoch. right? */
	tk->idx = x->nxt;
	x->key[tk->idx] = keyv;
	x->val[tk->idx] = val;
	idx = hashop[Vkind(keyv)].hash(keyv)&(x->sz-1);
	tk->link = x->idx[idx];
	x->idx[idx] = tk;
	x->nxt++;
	tab->cnt++;
}

void
tabput(Tab *tab, Val keyv, Val val)
{
	dotabput(tab, keyv, val);
}

void
tabdel(Tab *tab, Val keyv)
{
	Tabidx *tk, **ptk;
	Tabx *x;

	x = tab->x;
	tk = _tabget(tab, keyv, &ptk);
	if(tk == 0)
		return;
	gcwb(x->key[tk->idx]);
	gcwb(x->val[tk->idx]);
	x->key[tk->idx] = Xundef;
	x->val[tk->idx] = Xundef;
	*ptk = tk->link;
	efree(tk);
	tab->cnt--;
}

/* create a new vector of len 2N, where N is number of elements in TAB.
   elements 0..N-1 are the keys of TAB; N..2N-1 are the associated vals. */
static Vec*
tabenum(Tab *tab)
{
	Vec *vec;
	Tabidx *tk;
	u32 i, m;
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
	u32 i, m;
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
	u32 i, m;
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

static void
tabpop(Tab *tab, Val *rv)
{
	Tabidx *tk;
	Tabx *x;
	u32 i;
	Vec *vec;

	if(tab->cnt == 0)
		return; 	/* nil */

	tk = 0;
	x = tab->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		if(!tk)
			continue;
		x->idx[i] = tk->link;
		tab->cnt--;
		break;
	}

	vec = mkvec(2);
	_vecset(vec, 0, x->key[tk->idx]);
	_vecset(vec, 1, x->val[tk->idx]);
	gcwb(x->key[tk->idx]);
	gcwb(x->val[tk->idx]);
	x->key[tk->idx] = Xundef;
	x->val[tk->idx] = Xundef;
	efree(tk);
	*rv = mkvalvec(vec);
}

static Tab*
tabcopy(Tab *tab)
{
	Tabidx *tk;
	Tab *rv;
	Tabx *x;
	u32 i, m;

	rv = mktab();
	m = 0;
	x = tab->x;
	for(i = 0; i < x->sz && m < tab->cnt; i++){
		tk = x->idx[i];
		while(tk){
			tabput(rv, x->key[tk->idx], x->val[tk->idx]);
			m++;
			tk = tk->link;
		}
	}
	return rv;
}

static Rec*
mkrec(Rd *rd)
{
	Imm m;
	Rec *r;
	r = (Rec*)mal(Qrec);
	r->rd = rd;
	r->nf = rd->nf;
	r->field = emalloc(r->nf*sizeof(Val));
	for(m = 0; m < r->nf; m++)
		r->field[m] = Xnil;
	quard((Val)r);
	return r;
}

static void
recis(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Rd *rd;
	Str *mn;
	Rec *r;

	rd = valrd(disp[0]);
	mn = valstr(disp[1]);
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %.*s",
		      (int)mn->len, mn->s);
	if(Vkind(argv[0]) != Qrec){
		*rv = mkvalcval2(cval0);
		return;
	}
	r = valrec(argv[0]);
	if(r->rd == rd)
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
recmk(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Rd *rd;
	Str *mn;
	Rec *r;
	Imm m;

	rd = valrd(disp[0]);
	mn = valstr(disp[1]);
	if(argc != 0 && argc != rd->nf)
		vmerr(vm, "wrong number of arguments to %.*s",
		      (int)mn->len, mn->s);
	r = mkrec(rd);
	for(m = 0; m < argc; m++)
		r->field[m] = argv[m];
	*rv = mkvalrec(r);
}

static void
recfmt(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Rd *rd;
	Str *mn;
	Rec *r;
	unsigned len;
	char *buf;

	rd = valrd(disp[0]);
	mn = valstr(disp[1]);

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %.*s",
		      (int)mn->len, mn->s);
	if(Vkind(argv[0]) != Qrec)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, mn->s, (int)rd->name->len, rd->name->s);
	r = valrec(argv[0]);
	len = 1+rd->name->len+1+16+1+1;
	buf = emalloc(len);
	snprint(buf, len, "<%.*s %p>", (int)rd->name->len, rd->name->s, r);
	*rv = mkvalstr(mkstr0(buf));
	efree(buf);
}

static void
recget(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Rd *rd;
	Rec *r;
	Cval *ndx;
	Str *mn;

	rd = valrd(disp[0]);
	mn = valstr(disp[1]);
	ndx = valcval(disp[2]);

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %.*s",
		      (int)mn->len, mn->s);
	if(Vkind(argv[0]) != Qrec)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, mn->s, (int)rd->name->len, rd->name->s);
	r = valrec(argv[0]);
	if(r->rd != rd)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, mn->s, (int)rd->name->len, rd->name->s);

	/* weak test for compatible record descriptor */
	if(r->nf <= ndx->val)
		vmerr(vm, "attempt to call %.*s on incompatible %.*s record",
		      (int)mn->len, mn->s, (int)rd->name->len, rd->name->s);

	*rv = r->field[ndx->val];
}

static void
recset(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Rec *r;
	Rd *rd;
	Cval *ndx;
	Str *mn;

	rd = valrd(disp[0]);
	mn = valstr(disp[1]);
	ndx = valcval(disp[2]);

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to %.*s",
		      (int)mn->len, mn->s);
	if(Vkind(argv[0]) != Qrec)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, mn->s, (int)rd->name->len, rd->name->s);
	r = valrec(argv[0]);
	if(r->rd != rd)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, mn->s, (int)rd->name->len, rd->name->s);

	/* weak test for compatible record descriptor */
	if(r->nf <= ndx->val)
		vmerr(vm, "attempt to call %.*s on incompatible %.*s record",
		      (int)mn->len, mn->s, (int)rd->name->len, rd->name->s);

	gcwb(r->field[ndx->val]);
	r->field[ndx->val] = argv[1];
	*rv = argv[1];
}

static Rd*
mkrd(VM *vm, Str *name, List *fname, Closure *fmt)
{
	Rd *rd;
	Imm n;
	Closure *cl;
	unsigned len;
	Str *f;
	Val mn;
	char *buf;

	rd = hgets(vm->top->env->rd, name->s, (unsigned)name->len);
	if(rd == 0){
		rd = (Rd*)mal(Qrd);
		hputs(vm->top->env->rd,
		      xstrndup(name->s, (unsigned)name->len),
		      (unsigned)name->len, rd);
	}else{
		gcwb(mkvalstr(rd->name));
		gcwb(mkvallist(rd->fname));
		gcwb(mkvalcl(rd->is));
		gcwb(mkvalcl(rd->mk));
		gcwb(mkvalcl(rd->fmt));
		gcwb(mkvaltab(rd->get));
		gcwb(mkvaltab(rd->set));
	}

	listlen(mkvallist(fname), &rd->nf);
	rd->name = name;
	rd->fname = fname;

	len = 3+name->len+1;
	buf = emalloc(len);

	snprint(buf, len, "is%.*s", (int)name->len, name->s);
	rd->is = mkccl(buf, recis, 2, mkvalrd(rd), mkvalstr(mkstr0(buf)));
	snprint(buf, len, "%.*s", (int)name->len, name->s);
	rd->mk = mkccl(buf, recmk, 2, mkvalrd(rd), mkvalstr(mkstr0(buf)));
	if(fmt == 0){
		snprint(buf, len, "fmt%.*s", (int)name->len, name->s);
		rd->fmt = mkccl(buf, recfmt, 2,
				mkvalrd(rd), mkvalstr(mkstr0(buf)));
	}else
		rd->fmt = fmt;
	efree(buf);

	rd->get = mktab();
	rd->set = mktab();
	for(n = 0; n < rd->nf; n++){
		f = valstr(listref(vm, fname, n));
		len = name->len+3+f->len+1;
		buf = emalloc(len);

		/* get method */
		snprint(buf, len, "%.*s%.*s",
			 (int)name->len, name->s, (int)f->len, f->s);
		mn = mkvalstr(mkstr0(buf));
		cl = mkccl(buf, recget, 3,
			   mkvalrd(rd), mn, mkvallitcval(Vuint, n));
		tabput(rd->get, mkvalstr(f), mkvalcl(cl));

		/* set method */
		snprint(buf, len, "%.*sset%.*s",
			 (int)name->len, name->s, (int)f->len, f->s);
		mn = mkvalstr(mkstr0(buf));
		cl = mkccl(buf, recset, 3,
			   mkvalrd(rd), mn, mkvallitcval(Vuint, n));
		tabput(rd->set, mkvalstr(f), mkvalcl(cl));

		efree(buf);
	}
	return rd;
}

static Xtypename*
mkxtn(void)
{
	Xtypename *xtn;
	xtn = (Xtypename*)mal(Qxtn);
	return xtn;
}

static As*
mkas(void)
{
	As *as;
	as = (As*)mal(Qas);
	return as;
}

static Dom*
mkdom(Ns *ns, As *as, Str *name)
{
	Dom *dom;
	dom = (Dom*)mal(Qdom);
	dom->ns = ns;
	dom->as = as;
	dom->name = name;
	return dom;
}

static Ns*
mkns(void)
{
	Ns *ns;
	ns = (Ns*)mal(Qns);
	return ns;
}

static Env*
mkenv(void)
{
	Env *env;
	env = emalloc(sizeof(Env));
	env->var = mkhts();
	env->rd = mkhts();
	return env;
}

int
envbinds(Env *env, char *id)
{
	if(hgets(env->var, id, strlen(id)))
		return 1;
	else
		return 0;
}

Val*
envgetbind(Env *env, char *id)
{
	Val *v;

	v = hgets(env->var, id, strlen(id));
	if(!v){
		v = emalloc(sizeof(Val));
		*v = Xundef;
		hputs(env->var, xstrdup(id), strlen(id), v);
	}
	return v;
}

Val*
envget(Env *env, char *id)
{
	return hgets(env->var, id, strlen(id));
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
	if(Vkind(*v) == Qundef)
		return 0;
	*val = *v;
	return 1;
}

static void
freebinding(void *u, char *id, void *v)
{
	USED(u);
	efree(id);
	efree((Val*)v);
}

static void
freerd(void *u, char *id, void *v)
{
	USED(u);
	USED(v);
	efree(id);
}

void
freeenv(Env *env)
{
	hforeach(env->var, freebinding, 0);
	hforeach(env->rd, freerd, 0);
	freeht(env->var);
	freeht(env->rd);
	efree(env);
}

Toplevel*
mktoplevel(Xfd *in, Xfd *out, Xfd *err)
{
	Toplevel *top;

	top = emalloc(sizeof(Toplevel));
	top->env = mktopenv();

	/* persistent storage for stdio xfds */
	top->in = *in;
	top->out = *out;
	top->err = *err;

	builtinfd(top->env, "stdin",
		  mkfdfn(mkstr0("<stdin>"), Fread, &top->in));
	builtinfd(top->env, "stdout",
		  mkfdfn(mkstr0("<stdout>"), Fwrite, &top->out));
	builtinfd(top->env, "stderr",
		  mkfdfn(mkstr0("<stderr>"), Fwrite, &top->err));
	return top;
}

void
freetoplevel(Toplevel *top)
{
	freeenv(top->env);
	efree(top);
}

Cval*
mkcval(Dom *dom, Xtypename *type, Imm val)
{
	Cval *cv;
	cv = (Cval*)mal(Qcval);
	cv->dom = dom;
	cv->type = type;
	cv->val = val;
	return cv;
}

Cval*
mklitcval(Cbase base, Imm val)
{
	return mkcval(litdom, litdom->ns->base[base], val);
}

Val
mkvalcval(Dom *dom, Xtypename *t, Imm imm)
{
	return (Val)mkcval(dom, t, imm);
}

Val
mkvallitcval(Cbase base, Imm imm)
{
	return (Val)mklitcval(base, imm);
}

Val
mkvalcval2(Cval *cv)
{
	return (Val)cv;
}

Val
mkvalbox(Val boxed)
{
	Box *box;
	box = (Box*)mal(Qbox);
	box->v = boxed;
	return (Val)box;
}

Val
mkvalpair(Val car, Val cdr)
{
	Pair *pair;
	pair = (Pair*)mal(Qpair);
	pair->car = car;
	pair->cdr = cdr;
	return (Val)pair;
}

Range*
mkrange(Cval *beg, Cval *len)
{
	Range *r;
	r = (Range*)mal(Qrange);
	r->beg = beg;
	r->len = len;
	return r;
}

Val
mkvalrange(Cval *beg, Cval *len)
{
	Range *r;
	r = mkrange(beg, len);
	return (Val)r;
}

static void
putbox(Val box, Val boxed)
{
	Box *b;
	b = (Box*)box;
	gcwb(b->v);
	b->v = boxed;
}

static void
putval(VM *vm, Val v, Location *loc)
{
	Val *dst;

	switch(LOCKIND(loc->loc)){
	case Lreg:
		switch(LOCIDX(loc->loc)){
		case Rac:
			vm->ac = v;
			break;
		case Rsp:
		case Rfp:
		case Rpc:
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		dst = &vm->stack[(vm->fp+1)+LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Llocal:
		dst = &vm->stack[(vm->fp-1)-LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Ldisp:
		dst = &vm->clx->display[LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			putbox(*dst, v);
		else
			*dst = v;
		break;
	case Ltopl:
		dst = loc->var->val;
		*dst = v;
		break;
	default:
		fatal("bug");
	}
}

Src*
addr2line(Code *code, Imm pc)
{
	return code->insn[pc].src;
}

static void
printsrc(Xfd *xfd, Closure *cl, Imm pc)
{
	Code *code;
	Src *src;
	char *fn;

	code = cl->code;
	if(cl->cfn || cl->ccl){
		cprintf(xfd, "%20s\t(builtin %s)\n", cl->id,
			cl->cfn ? "function" : "closure");
		return;
	}

	src = addr2line(code, pc);
	if(src == 0){
		cprintf(xfd, "%20s\t(no source information)\n", cl->id);
		return;
	}
	fn = src->filename;
	if(fn == 0)
		fn = "<stdin>";
	cprintf(xfd, "%20s\t(%s:%u)\n", cl->id, fn, src->line);
}

void
fvmbacktrace(VM *vm)
{
	Imm pc, fp, narg;
	Closure *cl;
	Xfd *xfd;

	xfd = &vm->top->out;

	pc = vm->pc-1;		/* vm loop increments pc after fetch */
	fp = vm->fp;
	cl = vm->clx;
	while(fp != 0){
		if(strcmp(cl->id, "$halt")){
//			cprintf(xfd, "fp=%05lld pc=%08lld ", fp, pc);
			printsrc(xfd, cl, pc);
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
	fvmbacktrace(vm);
}

void
vmerr(VM *vm, char *fmt, ...)
{
	va_list args;
	cprintf(&vm->top->out, "error: ");
	va_start(args, fmt);
	cvprintf(&vm->top->out, fmt, args);
	va_end(args);
	cprintf(&vm->top->out, "\n");
	fvmbacktrace(vm);
	nexterror(vm);
}

static Val
getval(VM *vm, Location *loc)
{
	Val p;

	switch(LOCKIND(loc->loc)){
	case Lreg:
		switch(LOCIDX(loc->loc)){
		case Rac:
			return vm->ac;
		case Rsp:
		case Rfp:
		case Rcl:
		case Rpc:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		p = vm->stack[(vm->fp+1)+LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxed(p);
		else
			return p;
	case Llocal:
		p = vm->stack[(vm->fp-1)-LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxed(p);
		else
			return p;
	case Ldisp:
		p = vm->clx->display[LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxed(p);
		else
			return p;
	case Ltopl:
		p = *(loc->var->val);
		if(p == Xundef)
			vmerr(vm, "reference to unbound variable: %s",
			      loc->var->id);
		return p;
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Cval*
getcval(VM *vm, Location *loc)
{
	Val p;

	switch(LOCKIND(loc->loc)){
	case Lreg:
		switch(LOCIDX(loc->loc)){
		case Rac:
			return valcval(vm->ac);
		case Rsp:
		case Rfp:
		case Rpc:
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		p = vm->stack[(vm->fp+1)+LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxedcval(p);
		return valcval(p);
	case Llocal:
		p = vm->stack[(vm->fp-1)-LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxedcval(p);
		return valcval(p);
	case Ldisp:
		p = vm->clx->display[LOCIDX(loc->loc)];
		if(LOCBOX(loc->loc))
			return valboxedcval(p);
		return valcval(p);
	case Ltopl:
		p = *(loc->var->val);
		if(Vkind(p) == Qundef)
			vmerr(vm, "reference to unbound variable: %s",
			      loc->var->id);
		return valcval(p);
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Val
getvalrand(VM *vm, Operand *r)
{
	Val v;
	switch(r->okind){
	case Oloc:
		return getval(vm, &r->u.loc);
	case Okon:
		v = r->u.kon;
		/* any mutable vals must be copied */
		switch(Vkind(v)){
		case Qstr:
			v = mkvalstr(strcopy(valstr(v)));
			break;
		default:
			break;
		}
		return v;
	case Onil:
		return Xnil;
	default:
		fatal("bug");
	}
	return 0; /* not reached */
}

static Cval*
getcvalrand(VM *vm, Operand *r)
{
	switch(r->okind){
	case Oloc:
		return getcval(vm, &r->u.loc);
	case Okon:
		/* FIXME: check for cval? */
		return valcval(r->u.kon);
	default:
		fatal("bug");
	}
	return 0; /* not reached */
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
str2imm(VM *vm, Xtypename *xtn, Str *str)
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
		vmerr(vm, "attempt to access memory with incomplete type");
	}
	return 0; /* not reached */
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
	return 0; /* not reached */
}

/* transform representation of VAL used for type OLD to type NEW.
   OLD and NEW must be chased types with defined representations. */
static Imm
_rerep(Imm val, Xtypename *old, Xtypename *new)
{
	/* FIXME: non-trivial cases are : real <-> int,
	   real <-> alternate real
	   integer truncation
	   (so div and shr work)
	*/
	switch((new->rep<<5)|old->rep){
		#include "rerep.switch" /* re-cast val */
	}
 	return val;
}


static Imm
rerep(Imm val, Xtypename *old, Xtypename *new)
{
	old = chasetype(old);
	new = chasetype(new);
	if(old->rep == Rundef || new->rep == Rundef)
		fatal("undef!");
	return _rerep(val, old, new);
}

Cval*
typecast(VM *vm, Xtypename *xtn, Cval *cv)
{
	Xtypename *old, *new;
	old = chasetype(cv->type);
	new = chasetype(xtn);
	if(new->rep == Rundef){
		/* steal pointer representation from old */
		if(old->rep == Rundef)
			/* this is possible? */
			fatal("whoa.");
		if(old->tkind != Tptr)
			vmerr(vm, "attempt to cast to type "
			      "with undefined representation");
		new->rep = old->rep;
	}
	return mkcval(cv->dom, xtn, _rerep(cv->val, old, new));
}

Cval*
domcastbase(VM *vm, Dom *dom, Cval *cv)
{
	Xtypename *xtn, *old, *new;
	Str *es;

	xtn = cv->type;
	if(dom == litdom)
		xtn = chasetype(xtn);
	if(xtn->tkind != Tbase)
		vmerr(vm, "operand must be of base type");

	/* FIXME: do we really want to lookup the type in the new domain? */
	xtn = dom->ns->base[xtn->basename];
	if(xtn == 0){
		es = fmtxtn(cv->type);
		vmerr(vm, "cast to domain that does not define %.*s",
		      (int)es->len, es->s);
	}
	old = chasetype(cv->type);
	new = chasetype(xtn);
	if(old->rep == Rundef || new->rep == Rundef)
		vmerr(vm, " attempt to cast to type "
		      "with undefined representation");
	return mkcval(dom, xtn, rerep(cv->val, old, new));
}

Cval*
domcast(VM *vm, Dom *dom, Cval *cv)
{
	Xtypename *xtn, *old, *new;
	Str *es;

	/* FIXME: do we really want to lookup the type in the new domain? */
	if(dom == litdom)
		xtn = dolooktype(vm, chasetype(cv->type), dom->ns);
	else
		xtn = dolooktype(vm, cv->type, dom->ns);
	if(xtn == 0){
		es = fmtxtn(cv->type);
		vmerr(vm, "cast to domain that does not define %.*s",
		      (int)es->len, es->s);
	}
	old = chasetype(cv->type);
	new = chasetype(xtn);
	if(old->rep == Rundef || new->rep == Rundef)
		vmerr(vm, " attempt to cast to type "
		      "with undefined representation");
	return mkcval(dom, xtn, rerep(cv->val, old, new));
}

static void
dompromote(VM *vm, ikind op, Cval *op1, Cval *op2, Cval **rv1, Cval **rv2)
{
	Xtypename *b1, *b2;
	static char *domerr
		= "attempt to combine cvalues of incompatible domains";

	if(op1->dom == op2->dom)
		goto out;

	if(op1->dom == litdom)
		op1 = domcast(vm, op2->dom, op1);
	else if(op2->dom == litdom)
		op2 = domcast(vm, op1->dom, op2);
	else{
		b1 = chasetype(op1->type);
		b2 = chasetype(op2->type);
		if(b1->tkind != Tptr && b2->tkind != Tptr){
			op1 = domcastbase(vm, litdom, op1);
			op2 = domcastbase(vm, litdom, op2);
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
			if(equalxtn(p1, p2))
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
	return 0; /* not reached */
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
	vm->pc += 2; // skip livemask
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
xstrcmp(VM *vm, ikind op, Str *s1, Str *s2)
{
	int x;

	switch(op){
	case Icmpeq:
		return equalstr(s1, s2);
	case Icmpneq:
		return !equalstr(s1, s2);
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
	return 0; /* not reached */
}

static void
xunop(VM *vm, ikind op, Operand *op1, Operand *dst)
{
	Val v;
	Cval *cv, *cvr;
	Imm imm, nv;

	v = getvalrand(vm, op1);
	if(Vkind(v) != Qcval)
		vmerr(vm, "incompatible operand for unary %s", opstr[op]);
	cv = intpromote(vm, valcval(v));
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
		return; /* not reached */
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
	return 0; /* not reached */
}

static Cval*
xcvalptralu(VM *vm, ikind op, Cval *op1, Cval *op2,
	    Xtypename *t1, Xtypename *t2)
{
	Xtypename *sub, *pt;
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
		return mkcval(litdom, litdom->ns->base[Vlong], n);
	}

	/* exactly one operand is a pointer */

	if(t1->tkind == Tptr){
		sub = chasetype(t1->link);
		ptr = op1;
		n = op2->val;
	}else if(op == Isub){
		vmerr(vm, "invalid right-hand pointer operand to -");
		return 0; /* not reached */
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
	pt = chasetype(ptr->type);
	n = truncimm(n, pt->rep);
	return mkcval(ptr->dom, ptr->type, n);
}

static Cval*
xcvalalu1dom(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	Imm i1, i2, rv;
	Xtypename *t1, *t2;

	if(op1->dom != op2->dom)
		fatal("bug");
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
		if(isunsigned[t1->basename] && isunsigned[t2->basename])
			rv = i1/i2;
		else if(isunsigned[t1->basename])
			rv = i1/(s64)i2;
		else if(isunsigned[t2->basename])
			rv = (s64)i1/i2;
		else
			rv = (s64)i1/(s64)i2;
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

Cval*
xcvalalu(VM *vm, ikind op, Cval *op1, Cval *op2)
{
	dompromote(vm, op, op1, op2, &op1, &op2);
	return xcvalalu1dom(vm, op, op1, op2);
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
                    implementation-defined.
		    here, "implementation-defined" means whatever
		    your compiler says.  gcc and microsoft
		    performs sign extension.
	*/
	if(isunsigned[op1->type->basename])
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
	else
		switch(op){
		case Ishl:
			rv = (s64)i1<<i2;
			break;
		case Ishr:
			rv = (s64)i1>>i2;
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

	if(Vkind(v1) == Qcval && Vkind(v2) == Qcval){
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
			return; /* not reached */
		}
		putcvalrand(vm, cvr, dst);
		return;
	}

	if(Vkind(v1) == Qstr && Vkind(v2) == Qstr){
		s1 = valstr(v1);
		s2 = valstr(v2);
dostr:
		if(op == Iadd)
			putvalrand(vm, mkvalstr(strconcat(s1, s2)), dst);
		else{
			nv = xstrcmp(vm, op, s1, s2);
			if(nv)
				putvalrand(vm, mkvalcval2(cval1), dst);
			else
				putvalrand(vm, mkvalcval2(cval0), dst);
		}
		return;
	}

	if(Vkind(v1) == Qstr && Vkind(v2) == Qcval){
		cv2 = valcval(v2);
		if(isstrcval(cv2)){
			s2 = stringof(vm, cv2);
			s1 = valstr(v1);
			goto dostr;
		}
		/* fall through */
	}

	if(Vkind(v2) == Qstr && Vkind(v1) == Qcval){
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

static int
zeroval(Val v)
{
	Cval *cv;

	switch(Vkind(v)){
	case Qcval:
		cv = valcval(v);
		return cv->val == 0;
	default:
		return 0;
	}
}

static void
xjnz(VM *vm, Operand *src, Ctl *label)
{
	Val v;
	v = getvalrand(vm, src);
	if(!zeroval(v))
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
		vmerr(vm, "stack overflow");
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
	// specializer can trigger redundant boxing of parameters
	// when jumping from original to new function
	if(Vkind(v) == Qbox)
		return;
	putvalrand(vm, mkvalbox(v), op);
}

static void
xbox0(VM *vm, Operand *op)
{
	putvalrand(vm, mkvalbox(Xundef), op);
}

static void
xref(VM *vm, Operand *x, Operand *type, Operand *cval, Operand *dst)
{
	Val xv, typev, cvalv, rv;
	Xtypename *t, *b, *pt;
	Dom *d;
	Cval *cv;
	Imm imm;

	typev = getvalrand(vm, type);
	cvalv = getvalrand(vm, cval);
	xv = getvalrand(vm, x);
	if(Vkind(xv) != Qdom)
		vmerr(vm, "attempt to derive location from non-domain");
	d = valdom(xv);
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
	case Tbitfield: /* bitfield bonus: C wouldn't let you do this */
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
	case Tconst:
		vmerr(vm, "attempt to use enumeration constant as location");
		return; /* not reached */
	default:
	case Ttypedef:
		fatal("bug");
		return; /* not reached */
	case Txaccess:
		vmerr(vm, "attempt to construct pointer to value of "
		      "extended access type");
		return; /* not reached */
	}
	putvalrand(vm, rv, dst);
}

static int
dobitfieldgeom(Xtypename *b, BFgeom *bfg)
{
	Cval *bit0, *bs;
	Xtypename *bb;

	bit0 = valcval(b->bit0);
	bs = valcval(b->cnt);
	bfg->bp = bit0->val;
	bfg->bs = bs->val;
	bb = chasetype(b->link);
	bfg->isbe = isbigendian[bb->rep];
	return bitfieldgeom(bfg);
}

static void
xcval(VM *vm, Operand *x, Operand *type, Operand *cval, Operand *dst)
{
	Val xv, typev, cvalv, rv, p, argv[2];
	Imm imm;
	Dom *d;
	Xtypename *t, *b, *pt;
	Cval *cv;
	Str *s, *es;
	BFgeom bfg;

	xv = getvalrand(vm, x);
	typev = getvalrand(vm, type);
	cvalv = getvalrand(vm, cval);

	rv = 0;
	t = valxtn(typev);
	cv = valcval(cvalv);
	b = chasetype(t);

	/* special case: enum constants can be referenced through namespace */
	if(b->tkind == Tconst){
		switch(Vkind(xv)){
		case Qdom:
			d = valdom(xv);
			rv = mkvalcval(d, b->link, cv->val);
			goto out;
		case Qns:
			d = mkdom(valns(xv), litdom->as, 0);
			rv = mkvalcval(d, b->link, cv->val);
			goto out;
		default:
			fatal("bug");
		}
	}

	if(Vkind(xv) != Qdom)
		vmerr(vm, "attempt to access address space through non-domain");
	d = valdom(xv);
	switch(b->tkind){
	case Tbitfield:
		if(b->link->tkind == Tundef){
			es = fmtxtn(b->link->link);
			vmerr(vm, "attempt to read object of undefined type: "
			      "%.*s", (int)es->len, es->s);
		}
		if(0 > dobitfieldgeom(b, &bfg))
			vmerr(vm, "invalid bitfield access");
//		xprintf("get %lld %lld\n", cv->val+bfg.addr, bfg.cnt);
		s = callget(vm, d->as, cv->val+bfg.addr, bfg.cnt);
		imm = bitfieldget(s->s, &bfg);
		rv = mkvalcval(d, b->link, imm);
		break;
	case Tbase:
	case Tptr:
		/* FIXME: check type of cv */
		s = callget(vm, d->as, cv->val, typesize(vm, t));
		imm = str2imm(vm, t, s);
		rv = mkvalcval(d, t, imm);
		break;
	case Tarr:
		/* construct pointer to first element */
		pt = mkptrxtn(t->link, d->ns->base[Vptr]->rep);
		imm = truncimm(cv->val, pt->rep);
		rv = mkvalcval(d, pt, imm);
		break;
	case Txaccess:
		argv[0] = xv;
		p = safedovm(vm, t->get, 1, argv);
		if(Vkind(p) != Qcval)
			vmerr(vm,
			      "get method of extended access type returned "
			      "non-cval");
		rv = mkvalcval2(typecast(vm, t->link, valcval(p)));
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
	case Tconst:
		fatal("bug");
	}
out:
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

static int
isptrtype(Xtypename *t)
{
	t = chasetype(t);
	return t->tkind == Tptr;
}

static Xtypename*
nsvoidstar(Ns *ns)
{
	return mkptrxtn(ns->base[Vvoid], ns->base[Vptr]->rep);
}

static Cval*
str2voidstar(Ns *ns, Str *s)
{
	Dom *d;
	d = mkdom(ns, mksas(s), 0);
	return mkcval(d, nsvoidstar(ns), 0);
}

static void
xxcast(VM *vm, Operand *typeordom, Operand *o, Operand *dst)
{
	Val tv, ov, rv;
	Cval *cv;
	Dom *d;
	Xtypename *t;

	rv = 0;
	ov = getvalrand(vm, o);
	if(Vkind(ov) != Qcval && Vkind(ov) != Qstr)
		vmerr(vm, "operand 2 to extended cast operator must be a"
		      " cvalue or string");
	tv = getvalrand(vm, typeordom);
	if(Vkind(tv) != Qxtn && Vkind(ov) == Qstr)
		vmerr(vm, "illegal conversion");
	if(Vkind(tv) == Qxtn){
		t = valxtn(tv);
		if(!iscvaltype(t))
			vmerr(vm, "illegal type conversion");
		if(Vkind(ov) == Qstr){
			if(!isptrtype(t))
				vmerr(vm, "illegal type conversion");
			cv = str2voidstar(litdom->ns, valstr(ov));
		}else
			cv = valcval(ov);
		rv = mkvalcval2(typecast(vm, t, cv));
	}else if(Vkind(tv) == Qdom){
		d = valdom(tv);
		rv = mkvalcval2(domcast(vm, d, valcval(ov)));
	}else if(Vkind(tv) == Qns){
		cv = valcval(ov);
		d = mkdom(valns(tv), cv->dom->as, 0);
		rv = mkvalcval2(domcast(vm, d, valcval(ov)));
	}else if(Vkind(tv) == Qas){
		cv = valcval(ov);
		d = mkdom(cv->dom->ns, valas(tv), 0);
		rv = mkvalcval2(domcast(vm, d, valcval(ov)));
	}else
		vmerr(vm, "bad type for operand 1 to extended cast operator");
	putvalrand(vm, rv, dst);
}

static void
xlist(VM *vm, Operand *op1, Operand *op2, Operand *dst)
{
	Val v;
	Imm sp, n, m, i;
	List *lst;
	Val rv;

	v = getvalrand(vm, op1);
	sp = vm->fp+valimm(v);
	n = stkimm(vm->stack[sp]);
	v = getvalrand(vm, op2);
	m = valimm(v);

	lst = mklist();
	for(i = m; i < n; i++)
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

	xtn = 0;
	v = getvalrand(vm, op);
	if(Vkind(v) == Qcval){
		cv = valcval(v);
		xtn = cv->type;
	}else if(Vkind(v) == Qxtn)
		xtn = valxtn(v);
	else
		vmerr(vm, "operand 1 to sizeof must be a type or cvalue");
	imm = typesize(vm, xtn);
	rv = mkvalcval(litdom, litdom->ns->base[Vuint], imm);
	putvalrand(vm, rv, dst);
}

static void
nilfn(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(vm);
	USED(argc);
	USED(argv);
	USED(rv);
}

/* dispatch for abstract address spaces */
static void
nasdispatch(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *cmd;

	if(argc < 2)
		vmerr(vm,
		      "wrong number of arguments to address space dispatch");
	checkarg(vm, "nasdispatch", argv, 1, Qstr);
	cmd = valstr(argv[1]);
	if(equalstrc(cmd, "map")){
		if(argc != 2)
			vmerr(vm, "wrong number of arguments to map");
		*rv = mkvalvec(mkvec(0));
		return;
	}
	vmerr(vm, "attempt to access abstract address space");
}

static As*
mknas(void)
{
	As *as;
	Tab *mtab;
	mtab = mktab();
	tabput(mtab,
		mkvalstr(mkstr0("dispatch")),
		mkvalcl(mkcfn("nasdispatch", nasdispatch)));
	as = mkastab(mtab, mkstr0("nullas"));
	return as;
}

static void
sasget(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s, *dat;
	Range *r;
	Cval *beg, *end;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to get");
	checkarg(vm, "sasget", argv, 1, Qrange);
	s = valstr(disp[0]);
	r = valrange(argv[1]);
	beg = r->beg;
	end = xcvalalu(vm, Iadd, beg, r->len);
	if(beg->val > s->len)	/* FIXME: >=? */
		vmerr(vm, "address space access out of bounds");
	if(end->val > s->len)
		vmerr(vm, "address space access out of bounds");
	if(beg->val > end->val)
		vmerr(vm, "address space access out of bounds");
	dat = strslice(s, beg->val, end->val);
	*rv = mkvalstr(dat);
}

static void
sasput(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s, *dat;
	Range *r;
	Cval *beg, *end;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to put");
	checkarg(vm, "sasput", argv, 1, Qrange);
	checkarg(vm, "sasput", argv, 2, Qstr);
	s = valstr(disp[0]);
	r = valrange(argv[1]);
	dat = valstr(argv[2]);
	beg = r->beg;
	if(r->len->val == 0 && beg->val <= s->len)
		/* special case: empty string */
		return;
	end = xcvalalu(vm, Iadd, beg, r->len);
	if(beg->val >= s->len)
		vmerr(vm, "address space access out of bounds");
	if(end->val > s->len)
		vmerr(vm, "address space access out of bounds");
	if(beg->val > end->val)
		vmerr(vm, "address space access out of bounds");
	if(dat->len < r->len->val)
		vmerr(vm, "short put");
	/* FIXME: rationalize with l1_strput */
	memcpy(s->s+beg->val, dat->s, dat->len);
	USED(rv);
}

static void
sasmap(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Val val;
	Vec *v;
	Str *s;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to map");
	USED(argv);
	s = valstr(disp[0]);
	v = mkvec(1);
	val = mkvalrange(cvalnull,
			 mkcval(litdom,
				litdom->ns->base[Vptr], s->len));
	_vecset(v, 0, val);
	*rv = mkvalvec(v);
}

As*
mksas(Str *s)
{
	Tab *mtab;
	mtab = mktab();
	tabput(mtab, mkvalstr(mkstr0("get")),
		mkvalcl(mkccl("sasget", sasget, 1, mkvalstr(s))));
	tabput(mtab, mkvalstr(mkstr0("put")),
		mkvalcl(mkccl("sasput", sasput, 1, mkvalstr(s))));
	tabput(mtab, mkvalstr(mkstr0("map")),
		mkvalcl(mkccl("sasmap", sasmap, 1, mkvalstr(s))));
	return mkastab(mtab, 0);
}

static void
masget(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s, *dat;
	Range *r;
	Cval *beg, *end;
	Imm o;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to get");
	checkarg(vm, "masget", argv, 1, Qrange);
	s = valstr(disp[0]);
	r = valrange(argv[1]);
	beg = r->beg;
	end = xcvalalu(vm, Iadd, beg, r->len);
	o = (Imm)s->s;
	if(beg->val < o)
		vmerr(vm, "address space access out of bounds");
	if(beg->val > o+s->len)	/* FIXME: >=? */
		vmerr(vm, "address space access out of bounds");
	if(end->val > o+s->len)
		vmerr(vm, "address space access out of bounds");
	if(beg->val > end->val)
		vmerr(vm, "address space access out of bounds");
	dat = strslice(s, beg->val-o, end->val-o); /* yee-haw! */
	*rv = mkvalstr(dat);
}

static void
masput(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Str *s, *dat;
	Range *r;
	Cval *beg, *end;
	Imm o;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to put");
	checkarg(vm, "masput", argv, 1, Qrange);
	checkarg(vm, "masput", argv, 2, Qstr);
	s = valstr(disp[0]);
	r = valrange(argv[1]);
	dat = valstr(argv[2]);
	beg = r->beg;
	o = (Imm)s->s;
	if(r->len->val == 0 && beg->val <= o+s->len)
		/* special case: empty string */
		return;
	end = xcvalalu(vm, Iadd, beg, r->len);
	if(beg->val < o)
		vmerr(vm, "address space access out of bounds");
	if(beg->val >= o+s->len)
		vmerr(vm, "address space access out of bounds");
	if(end->val > o+s->len)
		vmerr(vm, "address space access out of bounds");
	if(beg->val > end->val)
		vmerr(vm, "address space access out of bounds");
	if(dat->len < r->len->val)
		vmerr(vm, "short put");
	/* FIXME: rationalize with l1_strput */
	memcpy((char*)beg->val, dat->s, dat->len);
	USED(rv);
}

static void
masmap(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Val val;
	Vec *v;
	Str *s;
	Imm o;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to map");
	USED(argv);
	s = valstr(disp[0]);
	v = mkvec(1);
	o = (Imm)s->s;
	val = mkvalrange(mkcval(litdom, litdom->ns->base[Vptr], o),
			 mkcval(litdom, litdom->ns->base[Vptr], o+s->len));
	_vecset(v, 0, val);
	*rv = mkvalvec(v);
}

As*
mkmas(Str *s)
{
	Tab *mtab;
	mtab = mktab();
	tabput(mtab, mkvalstr(mkstr0("get")),
		mkvalcl(mkccl("masget", masget, 1, mkvalstr(s))));
	tabput(mtab, mkvalstr(mkstr0("put")),
		mkvalcl(mkccl("masput", masput, 1, mkvalstr(s))));
	tabput(mtab, mkvalstr(mkstr0("map")),
		mkvalcl(mkccl("masmap", masmap, 1, mkvalstr(s))));
	return mkastab(mtab, 0);
}

As*
mkzas(Imm len)
{
	return mksas(mkstrn(len));
}

Val
mkattr(Val o)
{
	Tab *tab;

	if(Vkind(o) != Qtab && Vkind(o) != Qcval)
		fatal("bug");
	if(Vkind(o) == Qtab)
		tab = tabcopy(valtab(o));
	else{
		tab = mktab();
		tabput(tab, mkvalstr(mkstr0("offset")), o);
	}
	return mkvaltab(tab);
}

Val
attroff(Val o)
{
	Tab *tab;
	Val vp;

	if(Vkind(o) == Qcval)
		return o;
	if(Vkind(o) != Qtab)
		fatal("bug");
	tab = valtab(o);
	vp = tabget(tab, mkvalstr(mkstr0("offset")));
	if(vp)
		return vp;
	return Xnil;
}

static Val
copyattr(Val attr, Val newoff)
{
	Tab *t;
	Val off;
	Val key;
	if(Vkind(attr) != Qtab)
		fatal("bug");
	t = tabcopy(valtab(attr));
	key = mkvalstr(mkstr0("offset"));
	if(newoff)
		off = newoff;
	else
		off = tabget(t, key);
	tabput(t, key, off);
	return mkvaltab(t);
}

Xtypename*
chasetype(Xtypename *xtn)
{
	if(xtn->tkind == Ttypedef || xtn->tkind == Tenum)
		return chasetype(xtn->link);
	return xtn;
}

/* call looktype operator of NS on typename XTN.
   NB: on failure, we throw away valuable information: which part
   of the type was undefined.  consider adding a new return
   parameter that, if result is 0, points to the undefined type name */
static Xtypename*
_dolooktype(VM *vm, Xtypename *xtn, Ns *ns)
{
	Val argv[2], rv;
	Xtypename *tmp, *new;
	Vec *vec;
	Imm i;
	Rkind rep;

	switch(xtn->tkind){
	case Tvoid:
		return mkvoidxtn();
	case Tbase:
		return ns->base[xtn->basename];
	case Ttypedef:
	case Tstruct:
	case Tunion:
	case Tenum:
		argv[0] = mkvalns(ns);
		argv[1] = mkvalxtn(xtn);
		if(ns->looktype == 0)
			return 0;
		rv = safedovm(vm, ns->looktype, 2, argv);
		if(Vkind(rv) == Qnil)
			return 0;
		return valxtn(rv);
	case Tptr:
		rep = ns->base[Vptr]->rep;
		new = mkxtn();
		new->tkind = Tptr;
		new->link = _dolooktype(vm, xtn->link, ns);
		if(new->link == 0)
			return 0;
		new->rep = rep;
		return new;
	case Tarr:
		new = mkxtn();
		new->tkind = Tarr;
		new->link = _dolooktype(vm, xtn->link, ns);
		if(new->link == 0)
			return 0;
		new->cnt = xtn->cnt;
		return new;
	case Tfun:
		new = mkxtn();
		new->tkind = Tfun;
		new->link = _dolooktype(vm, xtn->link, ns);
		if(new->link == 0)
			return 0;
		new->param = mkvec(xtn->param->len);
		for(i = 0; i < xtn->param->len; i++){
			vec = veccopy(valvec(vecref(xtn->param, i)));
			vecset(new->param, i, mkvalvec(vec));
			tmp = _dolooktype(vm, valxtn(vecref(vec, Typepos)), ns);
			if(tmp == 0)
				return 0;
			vecset(vec, Typepos, mkvalxtn(tmp));
		}
		return new;
	case Tundef:
		/* FIXME: do we want this? */
		return _dolooktype(vm, xtn->link, ns);
	case Tconst:
		vmerr(vm, "looktype is undefined on enumeration constants");
	case Tbitfield:
		new = mkxtn();
		new->tkind = Tbitfield;
		new->cnt = xtn->cnt;
		new->bit0 = xtn->bit0;
		new->link = _dolooktype(vm, xtn->link, ns);
		if(new->link == 0)
			return 0;
		return new;
	case Txaccess:
		vmerr(vm, "looktype is undefined on extend access types");
	}
	fatal("bug");
	return 0; /* not reached */
}

/* NS must have initialized base cache */
static Xtypename*
dolooktype(VM *vm, Xtypename *xtn, Ns *ns)
{
	Xtypename *rv;
	switch(xtn->tkind){
	case Tvoid:
		rv = mkvoidxtn();
		break;
	case Tbase:
		rv = ns->base[xtn->basename];
		break;
	default:
		rv = _dolooktype(vm, xtn, ns);
		break;
	}
	return rv;
}

static void
nscache1base(VM *vm, Ns *ns, Cbase cb)
{
	Val rv, argv[2];
	Xtypename *xtn;
	argv[0] = mkvalns(ns);
	xtn = mkxtn();
	xtn->tkind = Tbase;
	xtn->basename = cb;
	argv[1] = mkvalxtn(xtn);
	rv = safedovm(vm, ns->looktype, 2, argv);
	if(Vkind(rv) == Qnil)
		vmerr(vm, "name space does not define %s", cbasename[cb]);
	ns->base[cb] = valxtn(rv);
}

static void
nscachebase(VM *vm, Ns *ns)
{
	Cbase cb;
	for(cb = Vchar; cb < Vnbase; cb++)
		nscache1base(vm, ns, cb);
	nscache1base(vm, ns, Vptr);
}

/* enumsym for namespaces constructed by @names */
static void
stdenumsym(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to enumsym");
	USED(argv);
	*rv = disp[0];
}

/* enumtype for namespaces constructed by @names */
static void
stdenumtype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to enumtype");
	USED(argv);
	*rv = disp[0];
}

/* looksym for namespaces constructed by @names */
static void
stdlooksym(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Tab *sym;
	Val vp;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looksym");
	checkarg(vm, "looksym", argv, 1, Qstr);
	sym = valtab(disp[0]);
	vp = tabget(sym, argv[1]);
	if(vp)
		*rv = vp;
}

/* looktype for namespaces constructed by @names */
static void
stdlooktype(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Tab *type;
	Val vp;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looktype");
	checkarg(vm, "looktype", argv, 1, Qxtn);
	type = valtab(disp[0]);
	vp = tabget(type, argv[1]);
	if(vp)
		*rv = vp;
}

/* lookaddr for namespaces constructed by @names */
static void
stdlookaddr(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	List *l;
	Cval *cv, *a;
	Imm addr, m, i, b, n;
	Vec *sym;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to lookaddr");
	checkarg(vm, "lookaddr", argv, 1, Qcval);
	l = vallist(disp[0]);
	cv = valcval(argv[1]);
	addr = cv->val;
	listlen(disp[0], &n);
	if(n == 0){
		*rv = Xnil;
		return;
	}
	sym = valvec(listref(vm, l, 0));
	a = valcval(attroff(vecref(sym, Attrpos)));
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
		a = valcval(attroff(vecref(sym, Attrpos)));
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
	Xtypename *xtn, *new, *def;

	/* have we already defined this type in the new namespace? */
	rv = tabget(ctx->type, xtnv);
	if(rv)
		return valxtn(rv);

	/* do we have an unprocessed definition for the type? */
	rv = tabget(ctx->rawtype, xtnv);
	if(rv){
		if(Vkind(rv) != Qxtn)
			vmerr(vm, "invalid raw type table");
		xtn = valxtn(xtnv);
		new = mktypedefxtn(xtn->tid, 0);

		/* bind before recursive resolve call to stop cycles */
		tabput(ctx->type, xtnv, mkvalxtn(new));

		def = valxtn(rv);
		if(def->tkind != Ttypedef)
			vmerr(vm, "invalid typedef in raw type table");
		if(!equalstr(def->tid, xtn->tid))
			vmerr(vm, "invalid typedef in raw type table");
		new->link = resolvetypename(vm, def->link, ctx);
		if(new->link == new)
			vmerr(vm, "circular definition in name space: "
			      "typedef %.*s",
			      (int)def->tid->len, def->tid->s);
		return new;
	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, xtnv);
	if(rv){
		tabput(ctx->type, xtnv, rv);
		return valxtn(rv);
	}

	tabput(ctx->undef, xtnv, xtnv);
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

	/* determine type that contains all constant values */
	a = mkcval(d, d->ns->base[Vint], 0);
	for(m = 0; m < v->len; m++){
		e = valvec(vecref(v, m)); /* FIXME check sanity */
		a = xcvalalu1dom(vm, Iadd, a,
				 domcastbase(vm, d, valcval(vecref(e, 1))));
	}
	t = a->type;

	/* cast all values to new type */
	for(m = 0; m < v->len; m++){
		e = valvec(vecref(v, m)); /* FIXME check sanity */
		cv = typecast(vm, t, domcastbase(vm, d, valcval(vecref(e, 1))));
		val = mkvalcval2(cv);
		vecset(e, 1, val);
	}

	return t;
}

static Xtypename*
resolvetag(VM *vm, Val xtnv, NSctx *ctx)
{
	Val rv, v, vp, attr;
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
		if(Vkind(rv) != Qxtn)
			vmerr(vm, "invalid raw type table");
		xtn = valxtn(rv);
		switch(xtn->tkind){
		case Tstruct:
		case Tunion:
			fld = xtn->field;
			attr = xtn->attr;
			if(fld == 0 || Vkind(attr) == Qnil)
				goto error;
			new = mkxtn();
			new->tkind = xtn->tkind;
			new->tag = xtn->tag;
			new->field = mkvec(fld->len);
			new->attr = attr;

			/* bind before recursive resolve call to stop cycles */
			tabput(ctx->type, xtnv, mkvalxtn(new));

			for(i = 0; i < fld->len; i++){
				vec = valvec(vecref(fld, i));
				vp = vecref(vec, Typepos);
				tmp = valxtn(vp);
				tmp = resolvetypename(vm, tmp, ctx);
				v = mkvalxtn(tmp);
				fv = mkvec(3);
				_vecset(fv, Typepos, v);
				_vecset(fv, Idpos, vecref(vec, Idpos));
				_vecset(fv, Attrpos, vecref(vec, Attrpos));
				v = mkvalvec(fv);
				_vecset(new->field, i, v);
			}
			return new;
		case Tenum:
			if(xtn->konst == 0)
				goto error;
			tmp = doenconsts(vm, valvec(xtn->konst), ctx->ons);
			new = mkxtn();
			new->tkind = Tenum;
			new->tag = xtn->tag;
			new->konst = xtn->konst;

			tabput(ctx->type, xtnv, mkvalxtn(new));
			new->link = resolvetypename(vm, tmp, ctx);
			return new;
		default:
		error:
			es = fmtxtn(xtn);
			vmerr(vm, "bad definition for tagged type: %.*s",
			      (int)es->len, es->s);
		}

	}

	/* does the ns from which we inherit have a definition? */
	rv = tabget(ctx->otype, xtnv);
	if(rv){
		tabput(ctx->type, xtnv, rv);
		return valxtn(rv);
	}

	tabput(ctx->undef, xtnv, xtnv);
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
		tabput(ctx->type, xtnv, rv);
		return valxtn(rv);
	}

	xtn = valxtn(xtnv);
	vmerr(vm, "undefined base type %s", cbasename[xtn->basename]);
	return 0; /* not reached */
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
		if(xtn->rep != Rundef){
			new = mkxtn();
			new->tkind = Tbase;
			new->basename = xtn->basename;
			xtn = new;
		}
		return resolvebase(vm, mkvalxtn(xtn), ctx);
	case Tptr:
		new = mkxtn();
		new->tkind = xtn->tkind;
		new->rep = ctx->ptrrep;
		new->link = resolvetypename(vm, xtn->link, ctx);
		return new;
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
		new->cnt = xtn->cnt;
		new->link = resolvetypename(vm, xtn->link, ctx);
		return new;
	case Tconst:
		new = mkxtn();
		new->tkind = xtn->tkind;
		new->link = resolvetypename(vm, xtn->link, ctx);
		return new;
	case Txaccess:
		new = mkxtn();
		new->tkind = xtn->tkind;
		new->link = resolvetypename(vm, xtn->link, ctx);
		new->get = xtn->get;
		new->put = xtn->put;
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
		new = mkxtn();
		new->tkind = Tundef;
		new->link = xtn->link;
		return new;
	}
	fatal("bug");
	return 0; /* not reached */
}

static void
calldispatch(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv)
{
	Closure *dcl;
	Val *xargv;

	dcl = valcl(disp[0]);
	xargv = emalloc((1+argc)*sizeof(Val));
	xargv[0] = argv[0];	/* this */
	xargv[1] = disp[1];	/* name of method to call */
	memcpy(xargv+2, argv+1, (argc-1)*sizeof(Val));
	if(waserror(vm)){
		efree(xargv);
		nexterror(vm);
	}
	*rv = safedovm(vm, dcl, argc+1, xargv);
	efree(xargv);
	poperror(vm);
}

static void
ascache1method(As *as, char *id, Closure **p)
{
	Val v;
	Str *ids;

	ids = mkstr0(id);

	/* is method in method table? */
	v = tabget(as->mtab, mkvalstr(ids));
	if(v){
		*p = valcl(v);
		return;
	}

	/* no dispatch, no dice */
	if(as->dispatch == 0){
		*p = 0;
		return;
	}

	/* bind a new caller to dispatch and hope for the best */
	*p = mkccl(id, calldispatch, 2, mkvalcl(as->dispatch), mkvalstr(ids));
}

static void
ascachemethod(As *as)
{
	/* dispatch must be resolved first */
	ascache1method(as, "dispatch", &as->dispatch);

	ascache1method(as, "get", &as->get);
	ascache1method(as, "put", &as->put);
	ascache1method(as, "map", &as->map);
}

As*
mkastab(Tab *mtab, Str *name)
{
	As *as;
	as = mkas();
	as->mtab = mtab;
	as->name = name;
	ascachemethod(as);
	return as;
}

static void
nscache1method(Ns *ns, char *id, Closure **p)
{
	Val v;
	Str *ids;

	ids = mkstr0(id);

	/* is method in method table? */
	v = tabget(ns->mtab, mkvalstr(ids));
	if(v){
		*p = valcl(v);
		return;
	}

	/* no dispatch, no dice */
	if(ns->dispatch == 0){
		*p = 0;
		return;
	}

	/* bind a new caller to dispatch and hope for the best */
	*p = mkccl(id, calldispatch, 2, mkvalcl(ns->dispatch), mkvalstr(ids));
}

static void
nscachemethod(Ns *ns)
{
	/* dispatch must be resolved first */
	nscache1method(ns, "dispatch", &ns->dispatch);

	nscache1method(ns, "looktype", &ns->looktype);
	nscache1method(ns, "enumtype", &ns->enumtype);
	nscache1method(ns, "looksym", &ns->looksym);
	nscache1method(ns, "enumsym", &ns->enumsym);
	nscache1method(ns, "lookaddr", &ns->lookaddr);
}

Ns*
mknstab(Tab *mtab, Str *name)
{
	Ns *ns;
	ns = mkns();
	ns->mtab = mtab;
	ns->name = name;
	nscachemethod(ns);
	return ns;
}

static Ns*
mknsfn(Closure *looktype, Closure *enumtype,
       Closure *looksym, Closure *enumsym,
       Closure *lookaddr, Str *name)
{
	Tab *mtab;
	mtab = mktab();
	tabput(mtab, mkvalstr(mkstr0("looktype")), mkvalcl(looktype));
	tabput(mtab, mkvalstr(mkstr0("enumtype")), mkvalcl(enumtype));
	tabput(mtab, mkvalstr(mkstr0("looksym")), mkvalcl(looksym));
	tabput(mtab, mkvalstr(mkstr0("enumsym")), mkvalcl(enumsym));
	tabput(mtab, mkvalstr(mkstr0("lookaddr")), mkvalcl(lookaddr));
	return mknstab(mtab, name);
}

static Ns*
mknstype(Tab *type, Str *name)
{
	return mknsfn(mkccl("looktype", stdlooktype, 1, mkvaltab(type)),
		      mkccl("enumtype", stdenumtype, 1, mkvaltab(type)),
		      mkcfn("looksym", nilfn),
		      mkccl("enumsym", stdenumsym, 1, mkvaltab(mktab())),
		      mkcfn("lookaddr", nilfn),
		      name);
}

static void
symcmp(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *sa, *sb;
	Cval *a, *b;
	USED(vm);
	USED(argc);
	sa = valvec(argv[0]);
	sb = valvec(argv[1]);
	a = valcval(attroff(vecref(sa, Attrpos)));
	b = valcval(attroff(vecref(sb, Attrpos)));
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

	/* create sorted list of symbols with offsets for lookaddr*/
	vec = tabenum(sym);
	len = vec->len/2;
	ls = mklist();
	for(m = 0; m < len; m++){
		vp = vecref(vec, len+m);	/* symbol #m */
		s = valvec(vp);
		op = vecref(s, Attrpos);
		if(Vkind(op) == Qnil)
			continue;
		_listappend(ls, vp);
	}
	argv[0] = mkvallist(ls);
	argv[1] = mkvalcl(mkcfn("symcmp", symcmp));
	l1_sort(vm, 2, argv, &rv);
	return mknsfn(mkccl("looktype", stdlooktype, 1, mkvaltab(type)),
		      mkccl("enumtype", stdenumtype, 1, mkvaltab(type)),
		      mkccl("looksym", stdlooksym, 1, mkvaltab(sym)),
		      mkccl("enumsym", stdenumsym, 1, mkvaltab(sym)),
		      mkccl("lookaddr", stdlookaddr, 1, mkvallist(ls)),
		      name);
}

static Ns*
mknsraw(VM *vm, Ns *ons, Tab *rawtype, Tab *rawsym, Str *name)
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
	Val xargv[1];

	ctx.rawtype = rawtype;
	ctx.rawsym = rawsym;
	ctx.type = mktab();
	ctx.sym = mktab();
	ctx.undef = mktab();
	ctx.ons = ons;
	xargv[0] = mkvalns(ons);
	if(ons->enumtype == 0)
		vmerr(vm, "parent name space does not define enumtype");
	if(ons->enumsym == 0)
		vmerr(vm, "parent name space does not define enumsym");
	ctx.otype = valtab(safedovm(vm, ons->enumtype, 1, xargv));
	ctx.osym = valtab(safedovm(vm, ons->enumsym, 1, xargv));

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

	x = ctx.rawtype->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			v = x->key[tk->idx];
			if(Vkind(v) != Qxtn)
				vmerr(vm, "invalid raw type table");
			xtn = valxtn(v);
			resolvetypename(vm, xtn, &ctx);
			tk = tk->link;
		}
	}

	x = ctx.rawsym->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			/* id -> [ xtn, id, attr ] */
			idv = x->key[tk->idx];
			vecv = x->val[tk->idx];
			if(Vkind(idv) != Qstr)
				vmerr(vm, "invalid raw symbol table");
			if(Vkind(vecv) != Qvec)
				vmerr(vm, "invalid raw symbol table");
			vec = valvec(vecv);
			if(!issym(vec))
				vmerr(vm, "invalid raw symbol table");
			xtn = valxtn(vecref(vec, Typepos));
			xtn = resolvetypename(vm, xtn, &ctx);
			if(xtn != 0){
				v = mkvalxtn(xtn);
				vecset(vec, Typepos, v); /* reuse vec */
				tabput(ctx.sym, idv, vecv);
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
				tabput(ctx.type, vp, x->val[tk->idx]);
			tk = tk->link;
		}
	}
	x = ctx.osym->x;
	for(i = 0; i < x->sz; i++){
		tk = x->idx[i];
		while(tk){
			vp = x->key[tk->idx];
			if(!tabget(ctx.sym, vp))
				tabput(ctx.sym, vp, x->val[tk->idx]);
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
				_vecset(nvec, Typepos, mkvalxtn(tmp));
				_vecset(nvec, Idpos, vecref(kvec, 0));
				_vecset(nvec, Attrpos, mkattr(vecref(kvec, 1)));
				tabput(ctx.sym,
				       vecref(kvec, 0), mkvalvec(nvec));
			}
		next:
			tk = tk->link;
		}
	}

	vec = tabenum(ctx.undef);
	m = vec->len/2;
	if(m > 0 && cqctflags['w']){
		xprintf("warning: name space references undefined "
			"type%s:\n", m > 1 ? "s" : "");
		while(m != 0){
			m--;
			vp = vecref(vec, m);
			xtn = valxtn(vp);
			as = fmtxtn(xtn);
			xprintf("\t%.*s\n", (int)as->len, as->s);
		}
	}

	ns = mknstypesym(vm, ctx.type, ctx.sym, name);
	nscachebase(vm, ns);

	return ns;
}

static char*
myroot(void)
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

static void* gotab[Iopmax+1];

static void
setgo(Insn *i, Imm lim)
{
	Imm k;
	for(k = 0; k < lim; k++){
		i->go = gotab[i->kind];
		i++;
	}
}

static void
vmsetcl(VM *vm, Val val)
{
	Closure *cl;

	if(Vkind(val) != Qcl)
		vmerr(vm, "attempt to apply non-procedure");
	cl = valcl(val);
	vm->clx = cl;
	vm->ibuf = vm->clx->code->insn;
	if(vm->ibuf->go == 0)
		setgo(vm->ibuf, vm->clx->code->ninsn);
}

jmp_buf*
_pusherror(VM *vm)
{
	Err *ep;
	if(vm->edepth >= vm->emax){
		vm->err = erealloc(vm->err, vm->emax*sizeof(Err),
				   2*vm->emax*sizeof(Err));
		vm->emax *= 2;
	}
	ep = &vm->err[vm->edepth++];
	ep->fp = vm->fp;
	ep->sp = vm->sp;
	ep->pc = vm->pc;
	ep->cl = vm->cl;
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
	vm->fp = ep->fp;
	vm->sp = ep->sp;
	vm->pc = ep->pc;
	vm->cl = ep->cl;
	vmsetcl(vm, vm->cl);
	longjmp(ep->esc, 1);
}

void
poperror(VM *vm)
{
	if(vm->edepth == 0)
		fatal("bad error stack discipline");
	vm->edepth--;
}

Val
cqctgcprotect(VM *vm, Val v)
{
	return gcprotect(v);
}

Val
cqctgcunprotect(VM *vm, Val v)
{
	return gcunprotect(v);
}

void
builtinnil(Env *env, char *name)
{
	envbind(env, name, Xnil);
}

void
builtinfd(Env *env, char *name, Fd *fd)
{
	Val val;
	val = mkvalfd(fd);
	envbind(env, name, val);
}

void
builtinfn(Env *env, char *name, Closure *cl)
{
	Val val;
	val = mkvalcl(cl);
	envbind(env, name, val);
	if(name[0] == '%')
		envbind(env, name+1, val);
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

#if 0
static void
builtintab(Env *env, char *name, Tab *tab)
{
	Val val;
	val = mkvaltab(tab);
	envbind(env, name, val);
}
#endif

static void
builtincval(Env *env, char *name, Cval *cv)
{
	Val val;
	val = mkvalcval2(cv);
	envbind(env, name, val);
}

static void
builtinval(Env *env, char *name, Val val)
{
	envbind(env, name, val);
}

static void
vmresetctl(VM *vm)
{
	vm->edepth = 0;
	vm->fp = 0;
	vm->sp = Maxstk;
	vm->ac = Xnil;
	vm->cl = mkvalcl(panicthunk());
}

Fd*
vmstdout(VM *vm)
{
	Val v;
	if(!envlookup(vm->top->env, "stdout", &v))
		vmerr(vm, "stdout is undefined");
	if(Vkind(v) != Qfd)
		vmerr(vm, "stdout not bound to a file descriptor");
	return valfd(v);
}

Val
safedovm(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	Val rv;
	gcdisable();
	if(waserror(vm)){
		gcenable();
		nexterror(vm);
	}
	rv = dovm(vm, cl, argc, argv);
	poperror(vm);
	gcenable();
	return rv;
}

Val
dovm(VM *vm, Closure *cl, Imm argc, Val *argv)
{
	Insn *i;
	Cval *cv;
	Val val;
	Imm m, narg, onarg;

#ifdef THREADED
	static int once;
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
		gotab[Ilive]	= &&Ilive;
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
		gotab[Isubsp] 	= &&Isubsp;
		gotab[Ivargc]	= &&Ivargc;
		gotab[Ixcast] 	= &&Ixcast;
		gotab[Ixor] 	= &&Ixor;
	}
#endif

	/* for recursive entry, store current context */
	vmpushi(vm, vm->fp);	/* fp */
	vmpush(vm, vm->cl);	/* cl */
	vmpushi(vm, vm->pc);	/* pc */
	vmpushi(vm, 0);		/* narg */
	vm->fp = vm->sp;

	/* push frame for halt thunk */
	vmpushi(vm, vm->fp);		/* fp */
	vmpush(vm, mkvalcl(halt));	/* cl */
	vmpushi(vm, halt->entry);	/* pc */
	for(m = argc; m > 0; m--)
		vmpush(vm, argv[m-1]);
	vmpushi(vm, argc);		/* narg */
	vm->fp = vm->sp;

	/* switch to cl */
	vm->cl = mkvalcl(cl);
	vmsetcl(vm, vm->cl);
	vm->pc = vm->clx->entry;

	while(1){
		i = &vm->ibuf[vm->pc++];
		tick++;
		NEXTLABEL(i){
		LABEL Inop:
			continue;
		LABEL Icallc:
			xcallc(vm);
			continue;
		LABEL Iinv:
		LABEL Ineg:
		LABEL Inot:
			xunop(vm, i->kind, &i->op1, &i->dst);
			continue;
		LABEL Iadd:
		LABEL Iand:
		LABEL Idiv:
		LABEL Imod:
		LABEL Imul:
		LABEL Ior:
		LABEL Ishl:
		LABEL Ishr:
		LABEL Isub:
		LABEL Ixor:
		LABEL Icmplt:
		LABEL Icmple:
		LABEL Icmpgt:
		LABEL Icmpge:
		LABEL Icmpeq:
		LABEL Icmpneq:
			xbinop(vm, i->kind, &i->op1, &i->op2, &i->dst);
			continue;
		LABEL Isubsp:
			val = getvalrand(vm, &i->op1);
			vm->sp -= valimm(val);
			continue;
		LABEL Imov:
			xmov(vm, &i->op1, &i->dst);
			continue;
		LABEL Ipush:
			xpush(vm, &i->op1);
			continue;
		LABEL Ipushi:
			xpushi(vm, &i->op1);
			continue;
		LABEL Iargc:
			val = getvalrand(vm, &i->op1);
			cv = valcval(val);
			if(stkimm(vm->stack[vm->fp]) != cv->val)
				vmerr(vm, "wrong number of arguments to %s",
				      vm->clx->id);
			continue;
		LABEL Ivargc:
			val = getvalrand(vm, &i->op1);
			cv = valcval(val);
			if(stkimm(vm->stack[vm->fp]) < cv->val)
				vmerr(vm, "insufficient arguments to %s",
				      vm->clx->id);
			continue;
		LABEL Icall:
			vm->cl = getvalrand(vm, &i->op1);
			vmsetcl(vm, vm->cl);
			vm->pc = vm->clx->entry;
			vm->fp = vm->sp;
			continue;
		LABEL Icallt:
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
		LABEL Iframe:
			vmpushi(vm, vm->fp);
			vmpush(vm, vm->cl);
			vmpushi(vm, i->dstlabel->insn);
			continue;
		LABEL Ipanic:
			fatal("vm panic");
		LABEL Ihalt:
			/* Ihalt is exactly like Iret... */
			vm->sp = vm->fp+stkimm(vm->stack[vm->fp])+1;/* narg+1 */
			vm->fp = stkimm(vm->stack[vm->sp+2]);
			vm->cl = vm->stack[vm->sp+1];
			vmsetcl(vm, vm->cl);
			vm->pc = stkimm(vm->stack[vm->sp]);
			vmpop(vm, 3);

			/* ...except that it returns from dovm */
			return vm->ac;
		LABEL Iret:
			vm->sp = vm->fp+stkimm(vm->stack[vm->fp])+1;/* narg+1 */
			vm->fp = stkimm(vm->stack[vm->sp+2]);
			vm->cl = vm->stack[vm->sp+1];
			vmsetcl(vm, vm->cl);
			vm->pc = stkimm(vm->stack[vm->sp]);
			vmpop(vm, 3);
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			gcpoll();
			continue;
		LABEL Ijmp:
			vm->pc = i->dstlabel->insn;
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			continue;
		LABEL Ijnz:
			xjnz(vm, &i->op1, i->dstlabel);
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			continue;
		LABEL Ijz:
			xjz(vm, &i->op1, i->dstlabel);
			if(vm->flags&VMirq)
				vmerr(vm, "interrupted");
			continue;
		LABEL Iclo:
			xclo(vm, &i->op1, i->dstlabel, &i->dst);
			/* vm->sp has been updated */
			continue;
		LABEL Ikg:
			xkg(vm, &i->dst);
			continue;
		LABEL Ikp:
			xkp(vm);
			/* vm->sp, vm->fp have been updated */
			continue;
		LABEL Ibox:
			xbox(vm, &i->op1);
			continue;
		LABEL Ibox0:
			xbox0(vm, &i->op1);
			continue;
		LABEL Icval:
			xcval(vm, &i->op1, &i->op2, &i->op3, &i->dst);
			continue;
		LABEL Iref:
			xref(vm, &i->op1, &i->op2, &i->op3, &i->dst);
			continue;
		LABEL Ixcast:
			xxcast(vm, &i->op1, &i->op2, &i->dst);
			continue;
		LABEL Ilist:
			xlist(vm, &i->op1, &i->op2, &i->dst);
			continue;
		LABEL Isizeof:
			xsizeof(vm, &i->op1, &i->dst);
			continue;
		LABEL Ilive:
			fatal("attempt to execute live mask");
		}
	}
}

void
checkarg(VM *vm, char *fn, Val *argv, unsigned arg, Qkind qkind)
{
	if(Vkind(argv[arg]) != qkind)
		vmerr(vm, "operand %d to %s must be a %s",
		      arg+1, fn, qname[qkind]);
}

int
isbasecval(Cval *cv)
{
	Xtypename *t;
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		return 0;
	return 1;
}

int
isnegcval(Cval *cv)
{
	Xtypename *t;
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		return 0;
	if(isunsigned[t->basename])
		return 0;
	return (s64)cv->val < 0;
}

int
isnatcval(Cval *cv)
{
	Xtypename *t;
	t = chasetype(cv->type);
	if(t->tkind != Tbase)
		return 0;
	if(isunsigned[t->basename])
		return 1;
	return (s64)cv->val >= 0;
}

int
iszerocval(Cval *cv)
{
	return cv->val == 0;
}

static int
docmp(VM *vm, Val a, Val b, Closure *cmp)
{
	Val argv[2], rv;
	Cval *cv;

	argv[0] = a;
	argv[1] = b;
	rv = safedovm(vm, cmp, 2, argv);
	if(Vkind(rv) != Qcval)
		vmerr(vm, "comparison function must return an integer cvalue");
	cv = valcval(rv);
	if(!isbasecval(cv))
		vmerr(vm, "comparison function must return an integer cvalue");
	if(isnegcval(cv))
		return -1;
	if(iszerocval(cv))
		return 0;
	return 1;
}

static void
doswap(Val *vs, Imm i, Imm j)
{
	Val t;
	gcwb(vs[i]);
	gcwb(vs[j]);
	t = vs[i];
	vs[i] = vs[j];
	vs[j] = t;
}

static void
dosort(VM *vm, Val *vs, Imm n, Closure *cmp)
{
	Val pv;
	Imm lo, hi, p;
	if(n < 2)
		return;

	lo = 0;
	hi = n;
	p = n>>1;		/* weak pivot */
	doswap(vs, p, lo);
	pv = vs[0];
	while(1){
		do
			lo++;
		while(lo < n && docmp(vm, vs[lo], pv, cmp) < 0);
		do
			hi--;
		while(hi && docmp(vm, vs[hi], pv, cmp) > 0);
		if(hi < lo)
			break;
		doswap(vs, lo, hi);
	}
	doswap(vs, 0, hi);
	dosort(vm, vs, hi, cmp);
	dosort(vm, vs+hi+1, n-hi-1, cmp);
}

static void
l1_sort(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Listx *x;
	Vec *v;
	Val *vs, o;
	Closure *cmp;
	Imm n;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to sort");
	if(Vkind(argv[0]) != Qlist && Vkind(argv[0]) != Qvec)
		vmerr(vm, "operand 1 to sort must a list or vector");
	checkarg(vm, "sort", argv, 1, Qcl);
	cmp = valcl(argv[1]);
	o = argv[0];
	switch(Vkind(o)){
	case Qlist:
		l = vallist(o);
		x = l->x;
		n = x->tl-x->hd;
		vs = &x->val[x->hd];
		break;
	case Qvec:
		v = valvec(o);
		n = v->len;
		vs = v->vec;
		break;
	default:
		return;
	}
	if(n < 2){
		*rv = o;
		return;
	}
	dosort(vm, vs, n, cmp);
	*rv = o;
}

static Val
dobsearch(VM *vm, Val key, Val *vs, Imm n, Closure *cmp)
{
	Imm m, i, b;
	int rv;

	b = 0;
	while(n > 1){
		i = n/2;
		m = b+i;
		rv = docmp(vm, key, vs[m], cmp);
		if(rv == 0)
			return vs[m];
		if(rv < 0)
			n = i;
		else{
			b = m;
			n = n-i;
		}
	}
	if(n && docmp(vm, key, vs[b], cmp) == 0)
		return vs[b];
	return Xnil;
}

static void
l1_bsearch(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Listx *x;
	Vec *v;
	Val *vs;
	Closure *cmp;
	Imm n;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to bsearch");
	if(Vkind(argv[1]) != Qlist && Vkind(argv[1]) != Qvec)
		vmerr(vm, "operand 2 to bsearch must a list or vector");
	checkarg(vm, "bsearch", argv, 2, Qcl);
	cmp = valcl(argv[2]);
	switch(Vkind(argv[1])){
	case Qlist:
		l = vallist(argv[1]);
		x = l->x;
		n = x->tl-x->hd;
		vs = &x->val[x->hd];
		break;
	case Qvec:
		v = valvec(argv[1]);
		n = v->len;
		vs = v->vec;
		break;
	default:
		return;
	}
	*rv = dobsearch(vm, argv[0], vs, n, cmp);
}

static void
l1_looktype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	Xtypename *xtn;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looktype");

	ns = 0;
	if(Vkind(argv[0]) == Qns)
		ns = valns(argv[0]);
	else if(Vkind(argv[0]) == Qdom){
		dom = valdom(argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to looktype must be a namespace or domain");

	if(Vkind(argv[1]) != Qxtn)
		vmerr(vm, "operand 2 to looktype must be a typename");
	xtn = valxtn(argv[1]);

	xtn = dolooktype(vm, xtn, ns);
	if(xtn)
		*rv = mkvalxtn(xtn);
}

static void
l1_looksym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to looksym");

	ns = 0;
	if(Vkind(argv[0]) == Qns)
		ns = valns(argv[0]);
	else if(Vkind(argv[0]) == Qdom){
		dom = valdom(argv[0]);
		ns = dom->ns;
	}else
		vmerr(vm,
		      "operand 1 to looksym must be a namespace or domain");

	if(Vkind(argv[1]) != Qstr)
		vmerr(vm, "operand 2 to looksym must be a string");
	*rv = safedovm(vm, ns->looksym, argc, argv);
}

static void
l1_domof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to domof");
	arg0 = argv[0];
	if(Vkind(arg0) != Qcval)
		vmerr(vm,
		      "operand 1 to domof must be a cvalue");
	cv = valcval(arg0);
	*rv = mkvaldom(cv->dom);
}

int
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

static Str*
valstrorcval(VM *vm, char *fn, Val *argv, unsigned arg)
{
	Cval *cv;
	if(Vkind(argv[arg]) == Qstr)
		return valstr(argv[arg]);
	else if(Vkind(argv[arg]) == Qcval){
		cv = valcval(argv[arg]);
		if(isstrcval(cv))
			return stringof(vm, cv);
	}
	vmerr(vm, "operand %d to %s must be a string, char* cvalue, "
	      "or uchar* cvalue",
	      arg+1, fn);
	return 0; /* not reached */
}

static Str*
valstrorcvalornil(VM *vm, char *fn, Val *argv, unsigned arg)
{
	Cval *cv;
	if(Vkind(argv[arg]) == Qstr)
		return valstr(argv[arg]);
	else if(Vkind(argv[arg]) == Qcval){
		cv = valcval(argv[arg]);
		if(isstrcval(cv))
			return stringof(vm, cv);
	}else if(Vkind(argv[arg]) == Qnil)
		return 0;
	vmerr(vm, "operand %d to %s must be a string, char* cvalue, "
	      "uchar* cvalue, or nil",
	      arg+1, fn);
	return 0; /* not reached */
}

void
callput(VM *vm, As *as, Imm off, Imm len, Str *s)
{
	Val argv[3];

	argv[0] = mkvalas(as);
	argv[1] = mkvalrange(mkcval(litdom, litdom->ns->base[Vptr], off),
			     mkcval(litdom, litdom->ns->base[Vptr], len));
	argv[2] = mkvalstr(s);
	if(s->len < len)
		vmerr(vm, "attempt to put short string into longer range");
	safedovm(vm, as->put, 3, argv);
}

Str*
callget(VM *vm, As *as, Imm off, Imm len)
{
	Val rv, argv[2];
	Str *s;

	argv[0] = mkvalas(as);
	argv[1] = mkvalrange(mkcval(litdom, litdom->ns->base[Vptr], off),
			     mkcval(litdom, litdom->ns->base[Vptr], len));
	rv = safedovm(vm, as->get, 2, argv);
	if(Vkind(rv) != Qstr)
		vmerr(vm, "address space get method returned non-string");
	s = valstr(rv);
	if(s->len != len)
		vmerr(vm, "address space get method returned wrong number of "
		      "bytes");
	return s;
}

Str*
getbytes(VM *vm, Cval *addr, Imm n)
{
	return callget(vm, addr->dom->as, addr->val, n);
}

Vec*
callmap(VM *vm, As *as)
{
	Val argv[1], rv;
	argv[0] = mkvalas(as);
	rv = safedovm(vm, as->map, 1, argv);
	if(Vkind(rv) != Qvec)
		vmerr(vm, "address space map returned invalid value");
	return valvec(rv);
}

Range*
mapstab(VM *vm, Vec *map, Imm addr, Imm len)
{
	Imm m;
	Val rp;
	Range *r;

	for(m = 0; m < map->len; m++){
		rp = vecref(map, m);
		if(Vkind(rp) != Qrange)
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
Str*
stringof(VM *vm, Cval *cv)
{
	Str *s;
	char *buf, *q;
	Vec *v;
	Range *r;
	Imm l, m, n, o;
	static unsigned unit = 128;

	/* effectively a call to unit */
	v = callmap(vm, cv->dom->as);
	r = mapstab(vm, v, cv->val, 0);	/* FIXME: type sanity */
	if(r == 0)
		vmerr(vm, "address space access out of bounds");

	l = 0;
	m = r->beg->val+r->len->val-cv->val;
	o = cv->val;
	buf = 0;
	while(m > 0){
		n = MIN(m, unit);
		if(buf == 0)
			buf = emalloc(unit);
		else
			buf = erealloc(buf, l, l+n);
		s = callget(vm, cv->dom->as, o, n);
		memcpy(buf+l, s->s, s->len);
		q = strnchr(buf+l, '\0', s->len);
		if(q){
			l += q-(buf+l);
			break;
		}
		l += s->len;
		o += s->len;
		m -= s->len;
	}
	s = mkstr(buf, l);	/* FIXME: mkstr copies buf; should steal */
	efree(buf);
	return s;
}

int
ismapped(VM *vm, As *as, Imm addr, Imm len)
{
	Vec *v;
	Range *r;

	if(len == 0)
		return 0;
	if(addr+len < len)
		/* bogus length */
		return 0;
	v = callmap(vm, as);
	r = mapstab(vm, v, addr, len);	/* FIXME: type sanity */
	return r != 0;
}

static void
l1_backtrace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to backtrace");
	USED(argv);
	vmbacktrace(vm);
	USED(rv);
}

static void
l1_tabkeys(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg0;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tabkeys");
	arg0 = argv[0];
	if(Vkind(arg0) != Qtab)
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
	if(Vkind(arg0) != Qtab)
		vmerr(vm, "operand 1 to tabvals must be a table");
	*rv = mkvalvec(tabenumvals(valtab(arg0)));
}

static void
l1_myrootns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *r;
	USED(argc);
	USED(argv);
	r = myroot();
	if(!envlookup(vm->top->env, r, rv))
		vmerr(vm, "my root name space is undefined: %s", r);
}

static void
dotypepredicate(VM *vm, Imm argc, Val *argv, Val *rv, char *id, unsigned kind)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", id);
	if(Vkind(argv[0]) != Qxtn)
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
l1_isxaccess(VM *vm, Imm argc, Val *argv, Val *rv)
{
	dotypepredicate(vm, argc, argv, rv, "isxaccess", Txaccess);
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
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tbase)
		vmerr(vm, "operand 1 to baseid must be a base ctype");
	*rv = mkvalstr(mkstr0(cbasename[xtn->basename]));
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
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, err);

	xtn = valxtn(argv[0]);
	if(xtn->tkind == Ttypedef)
		xtn = chasetype(xtn);
	if(xtn->tkind != Tptr && xtn->tkind != Tarr
	   && xtn->tkind != Tenum && xtn->tkind != Tconst
	   && xtn->tkind != Txaccess && xtn->tkind != Tundef)
		vmerr(vm, err);

	if(xtn->link)
		*rv = mkvalxtn(xtn->link);
}

static void
l1_rettype(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to retttype");
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm,
		      "operand 1 to susize must be a struct or union ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to susize must be a struct or union ctype");
	*rv = attroff(xtn->attr);
}

static void
l1_suattr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to suattr");
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm,
		      "operand 1 to suattr must be a struct or union ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to suattr must be a struct or union ctype");
	*rv = xtn->attr;
}

static void
l1_bitfieldwidth(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldwidth");
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, "operand 1 to bitfieldwidth "
		      "must be a bitfield ctype");

	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldwidth "
		      "must be a bitfield ctype");
	*rv = xtn->cnt;
}

static void
l1_bitfieldcontainer(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to bitfieldcontainer");
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, "operand 1 to bitfieldpos must be a bitfield ctype");

	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tbitfield)
		vmerr(vm, "operand 1 to bitfieldpos must be a bitfield ctype");
	*rv = xtn->bit0;
}

static void
l1_xaccessget(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to xaccessget");
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, "operand 1 to xaccessget must be an "
		      "extended access bitfield ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Txaccess)
		vmerr(vm, "operand 1 to xaccessget must be an "
		      "extended access bitfield ctype");
	*rv = mkvalcl(xtn->get);
}

static void
l1_xaccessput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to xaccessput");
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, "operand 1 to xaccessput must be an "
		      "extended access bitfield ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Txaccess)
		vmerr(vm, "operand 1 to xaccessput must be an "
		      "extended access bitfield ctype");
	*rv = mkvalcl(xtn->put);
}

static void
l1_arraynelm(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to arraynelm");
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Ttypedef)
		vmerr(vm,
		      "operand 1 to typedeftype must be a typedef ctype");
	if(xtn->link)
		*rv = mkvalxtn(xtn->link);
}

static void
l1_params(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to params");
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 2)
		vmerr(vm, err);
	vp = v->vec[Typepos];
	if(Vkind(vp) != Qxtn)
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
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 2)
		vmerr(vm, err);
	vp = v->vec[Idpos];
	if(Vkind(vp) != Qstr && Vkind(vp) != Qnil)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_fields(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fields");
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	xtn = valxtn(argv[0]);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm,
		      "operand 1 to fields must be a struct or union ctype");
	if(xtn->field == 0)
		return;		/* nil */
	*rv = mkvalvec(xtn->field);
}

static Val
rlookfield(VM *vm, Xtypename *su, Val tag)
{
	Xtypename *t;
	Val vp, rp, id, o;
	Vec *f, *r;
	Imm i;

	for(i = 0; i < su->field->len; i++){
		vp = vecref(su->field, i);
		f = valvec(vp);
		id = vecref(f, Idpos);
		if(Vkind(id) == Qstr){
			if(equalstrv(tag, id))
				return vp;
			else
				continue;
		}
		/* assume id is nil */
		t = chasetype(valxtn(vecref(f, Typepos)));
		if(t->tkind != Tstruct && t->tkind != Tunion)
			continue;
		/* recursively search embedded anonymous field */
		rp = rlookfield(vm, t, tag);
		if(rp == 0)
			continue;
		r = valvec(rp);
		o = mkvalcval2(xcvalalu(vm, Iadd,
					valcval(attroff(vecref(f, Attrpos))),
					valcval(attroff(vecref(r, Attrpos)))));
		r = veccopy(r);
		_vecset(r, Attrpos, copyattr(vecref(r, Attrpos), o));
		return mkvalvec(r);
	}
	return 0;
}

static void
l1_lookfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	Val vp;
	static char *err1
		= "operand 1 to lookfield must be a struct or union ctype";
	static char *err2
		= "operand 2 to lookfield must be a string";

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to lookfield");
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, err1);
	xtn = valxtn(argv[0]);
	xtn = chasetype(xtn);
	if(xtn->tkind != Tstruct && xtn->tkind != Tunion)
		vmerr(vm, err1);
	if(Vkind(argv[1]) != Qstr)
		vmerr(vm, err2);
	vp = rlookfield(vm, xtn, argv[1]);
	if(vp)
		*rv = vp;
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
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Typepos];
	if(Vkind(vp) != Qxtn)
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
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Idpos];
	if(Vkind(vp) != Qstr && Vkind(vp) != Qnil)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_fieldattr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	static char *err
		= "operand 1 to fieldattr must be a vector returned by fields";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fieldattr");
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	*rv = vecref(v, Attrpos);
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
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = vecref(v, Attrpos);
	if(Vkind(vp) == Qnil)
		return;		/* nil */
	vp = attroff(vp);
	*rv  = vp;
}

static void
l1_enumconsts(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	static char *err
		= "operand 1 to enumconsts must be a defined enum ctype";
	if(argc != 1)
		vmerr(vm, err);
	if(Vkind(argv[0]) != Qxtn)
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
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Typepos];
	if(Vkind(vp) != Qxtn)
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
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = v->vec[Idpos];
	if(Vkind(vp) != Qstr && Vkind(vp) != Qnil)
		vmerr(vm, err);
	*rv = vp;
}

static void
l1_symattr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	static char *err
		= "operand 1 to symattr must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symattr");
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	*rv = vecref(v, Attrpos);
}

static void
l1_symoff(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Val vp;
	static char *err
		= "operand 1 to symoff must be a vector returned by looksym";

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to symoff");
	if(Vkind(argv[0]) != Qvec)
		vmerr(vm, err);
	v = valvec(argv[0]);
	if(v->len < 3)
		vmerr(vm, err);
	vp = vecref(v, Attrpos);
	if(Vkind(vp) == Qnil)
		return;		/* nil */
	*rv = attroff(vp);
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
	t = 0;
	if(Vkind(argv[0]) == Qcval){
		cv = valcval(argv[0]);
		t = cv->type;
	}else if(Vkind(argv[0]) == Qxtn){
		t = valxtn(argv[0]);
		if(t->tkind == Tbitfield || t->tkind == Tconst
		   || t->tkind == Txaccess)
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
	USED(argv);
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
	USED(argv);
	domkctype_base(Vchar, rv);
}

static void
l1_mkctype_short(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_short");
	USED(argv);
	domkctype_base(Vshort, rv);
}

static void
l1_mkctype_int(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_int");
	USED(argv);
	domkctype_base(Vint, rv);
}

static void
l1_mkctype_long(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_long");
	USED(argv);
	domkctype_base(Vlong, rv);
}

static void
l1_mkctype_vlong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_vlong");
	USED(argv);
	domkctype_base(Vvlong, rv);
}

static void
l1_mkctype_uchar(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uchar");
	USED(argv);
	domkctype_base(Vuchar, rv);
}

static void
l1_mkctype_ushort(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ushort");
	USED(argv);
	domkctype_base(Vushort, rv);
}

static void
l1_mkctype_uint(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uint");
	USED(argv);
	domkctype_base(Vuint, rv);
}

static void
l1_mkctype_ulong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ulong");
	USED(argv);
	domkctype_base(Vulong, rv);
}

static void
l1_mkctype_uvlong(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_uvlong");
	USED(argv);
	domkctype_base(Vuvlong, rv);
}

static void
l1_mkctype_float(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_float");
	USED(argv);
	domkctype_base(Vfloat, rv);
}

static void
l1_mkctype_double(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_double");
	USED(argv);
	domkctype_base(Vdouble, rv);
}

static void
l1_mkctype_ldouble(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mkctype_ldouble");
	USED(argv);
	domkctype_base(Vlongdouble, rv);
}

static void
l1_mkctype_ptr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *pxtn;
	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkctype_ptr");
	if(Vkind(argv[0]) != Qxtn)
		vmerr(vm, "operand 1 to mkctype_ptr must be a pointer ctype");
	xtn = valxtn(argv[0]);
	if(argc == 1)
		xtn = mkptrxtn(xtn, Rundef);
	else{
		if(Vkind(argv[1]) != Qxtn)
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

	xtn = 0;
	switch((unsigned)argc){
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

static int
issym(Vec *sym)
{
	Val x;

	if(sym->len != 2 && sym->len != 3)
		return 0;
	x = vecref(sym, Typepos);
	if(Vkind(x) != Qxtn)
		return 0;
	x = vecref(sym, Idpos);
	if(Vkind(x) != Qstr && Vkind(x) != Qnil)
		return 0;
	if(sym->len < 3)
		return 1;
	x = vecref(sym, Attrpos);
	if(Vkind(x) != Qtab && Vkind(x) != Qnil)
		return 0;
	return 1;
}

static int
issymvec(Vec *v)
{
	Imm m;
	Val e;
	Vec *sym;
	for(m = 0; m < v->len; m++){
		e = vecref(v, m);
		if(Vkind(e) != Qvec)
			return 0;
		sym = valvec(e);
		if(!issym(sym))
			return 0;
	}
	return 1;
}

static void
domkctype_su(VM *vm, char *fn, Tkind tkind, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;
	Str *s;
	Vec *f;

	xtn = 0;
	switch((unsigned)argc){
	case 1:
		/* TAG */
		checkarg(vm, fn, argv, 0, Qstr);
		s = valstr(argv[0]);
		xtn = mkxtn();
		xtn->tkind = tkind;
		xtn->tag = s;
		xtn->attr = Xnil;
		break;
	case 3:
		/* TAG FIELDS SIZE */
		checkarg(vm, fn, argv, 0, Qstr);
		checkarg(vm, fn, argv, 1, Qvec);
		if(Vkind(argv[2]) != Qcval && Vkind(argv[2]) != Qtab)
			vmerr(vm, "operand 3 to %s must be a cvalue or table",
			      fn);
		s = valstr(argv[0]);
		f = valvec(argv[1]);
		if(!issymvec(f))
			vmerr(vm, "bad field vector", fn);
		xtn = mkxtn();
		xtn->tkind = tkind;
		xtn->tag = s;
		xtn->field = f;
		xtn->attr = argv[2];
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

	xtn = 0;
	switch((unsigned)argc){
	case 1:
		/* TYPE */
		checkarg(vm, "mkctype_array", argv, 0, Qxtn);
		sub = valxtn(argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tarr;
		xtn->link = sub;
		xtn->cnt = Xnil;
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
	if(!issymvec(p))
		vmerr(vm, "bad parameter vector");
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
	xtn->cnt = argv[1];
	xtn->bit0 = argv[2];
	*rv = mkvalxtn(xtn);
}

static void
l1_mkctype_enum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn, *t;
	Str *s;
	Vec *c;

	xtn = 0;
	switch((unsigned)argc){
	case 1:
		/* TAG */
		checkarg(vm, "mkctype_enum", argv, 0, Qstr);
		s = valstr(argv[0]);
		xtn = mkxtn();
		xtn->tkind = Tenum;
		xtn->tag = s;
		xtn->konst = 0;
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
l1_mkctype_xaccess(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Xtypename *xtn;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to mkctype_xaccess");
	checkarg(vm, "mkctype_xaccess", argv, 0, Qxtn);
	checkarg(vm, "mkctype_xaccess", argv, 1, Qcl);
	checkarg(vm, "mkctype_xaccess", argv, 2, Qcl);
	xtn = mkxtn();
	xtn->tkind = Txaccess;
	xtn->link = valxtn(argv[0]);
	xtn->get = valcl(argv[1]);
	xtn->put = valcl(argv[2]);
	*rv = mkvalxtn(xtn);
}

static void
mksymorfieldorparam(char *what, VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *vec;
	Val attr;

	checkarg(vm, what, argv, 0, Qxtn);
	if(argc > 1)
		if(Vkind(argv[1]) != Qstr && Vkind(argv[1]) != Qnil)
			vmerr(vm, "operand 2 to %s must be a string or nil",
			      what);
	if(argc == 3)
		if(Vkind(argv[2]) != Qcval
		   && Vkind(argv[2]) != Qtab
		   && Vkind(argv[2]) != Qnil)
			vmerr(vm,
			      "operand 3 to %s must be a table, cvalue, or nil",
			      what);
	vec = mkvec(3);
	_vecset(vec, 0, argv[0]);
	if(argc > 1)
		_vecset(vec, 1, argv[1]);
	else
		_vecset(vec, 1, Xnil);
	if(argc > 2 && Vkind(argv[2]) == Qcval)
		attr = mkattr(argv[2]);
	else if(argc > 2)
		attr = argv[2];
	else
		attr = Xnil;
	_vecset(vec, 2, attr);
	*rv = mkvalvec(vec);
}

static void
l1_mksym(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to mksym");
	mksymorfieldorparam("mksym", vm, argc, argv, rv);
}

static void
l1_mkfield(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to mkfield");
	mksymorfieldorparam("mkfield", vm, argc, argv, rv);
}

static void
l1_mkparam(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc < 1 || argc > 2)
		vmerr(vm, "wrong number of arguments to mkparam");
	mksymorfieldorparam("mkparam", vm, argc, argv, rv);
}

static void
l1_isnil(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isnil");
	if(Vkind(argv[0]) == Qnil)
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
	USED(argc);
	USED(argv);
	vmerr(vm, "memory access fault");
	USED(rv);
}

static void
l1_strput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s, *t;
	Cval *off, *cv;
	Imm o;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to strput");
	if(Vkind(argv[0]) != Qstr)
		vmerr(vm, "operand 1 to strput must be a string");
	if(Vkind(argv[1]) != Qcval)
		vmerr(vm, "operand 2 to strput must be an offset");
	if(Vkind(argv[2]) != Qstr && Vkind(argv[2]) != Qcval)
		vmerr(vm, "operand 3 to strput must be a string or character");
	s = valstr(argv[0]);
	off = valcval(argv[1]);
	o = off->val;		/* FIXME: use type */
	if(o >= s->len)
		vmerr(vm, "strput out of bounds");
	if(Vkind(argv[2]) == Qstr){
		t = valstr(argv[2]);
		if(o+t->len > s->len)
			vmerr(vm, "strput out of bounds");
		memcpy(s->s+o, t->s, t->len);
	}else{
		cv = valcval(argv[2]);
		s->s[o] = (char)cv->val;
	}
	USED(rv);
}

static void
l1_put(VM *vm, Imm argc, Val *iargv, Val *rv)
{
	Dom *d;
	Xtypename *t, *b;
	Cval *addr, *cv;
	Val argv[3];
	Str *bytes, *es;
	BFgeom bfg;
	Imm imm;

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
		bytes = imm2str(t, cv->val);
		callput(vm, d->as, addr->val, typesize(vm, t), bytes);
		*rv = mkvalcval2(cv);
		break;
	case Tbitfield:
		if(b->link->tkind == Tundef){
			es = fmtxtn(b->link->link);
			vmerr(vm, "attempt to write object of undefined type: "
			      "%.*s", (int)es->len, es->s);
		}
		if(0 > dobitfieldgeom(b, &bfg))
			vmerr(vm, "invalid bitfield access");

		/* get contents of bitfield container */
		bytes = callget(vm, d->as, addr->val+bfg.addr, bfg.cnt);

		/* update bitfield container */
		imm = bitfieldput(bytes->s, &bfg, cv->val);

		/* put updated bitfield container */
		callput(vm, d->as, addr->val+bfg.addr, bfg.cnt, bytes);

		/* return value of bitfield (not container) */
		*rv = mkvalcval(d, b->link, imm);
		break;
	case Txaccess:
		if(b->link->tkind == Tundef){
			es = fmtxtn(b->link->link);
			vmerr(vm, "attempt to write object of undefined type: "
			      "%.*s", (int)es->len, es->s);
		}
		cv = typecast(vm, b->link, cv);
		argv[0] = mkvaldom(d);
		argv[1] = mkvalcval2(cv);
		safedovm(vm, b->put, 2, argv);
		*rv = mkvalcval2(cv);
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
	Imm m, i, len, len2;
	Val* argv;

	if(argc < 2)
		vmerr(vm, "wrong number of arguments to foreach");
	checkarg(vm, "foreach", iargv, 0, Qcl);

	// tables: only one container operand
	if(Vkind(iargv[1]) == Qtab){
		if(argc > 2)
			vmerr(vm, "bad combination of containers");
		cl = valcl(iargv[0]);
		t = valtab(iargv[1]);
		k = tabenumkeys(t);
		v = tabenumvals(t);
		argv = emalloc(2*sizeof(Val));
		for(m = 0; m < v->len; m++){
			argv[0] = vecref(k, m);
			argv[1] = vecref(v, m);
			safedovm(vm, cl, 2, argv);
		}
		efree(argv);
		return;
	}

	// list and vector: any number, but must have equal length
	len = 0;
	for(m = 1; m < argc; m++){
		switch(Vkind(iargv[m])){
		case Qlist:
			l = vallist(iargv[m]);
			len2 = listxlen(l->x);
			break;
		case Qvec:
			v = valvec(iargv[m]);
			len2 = v->len;
			break;
		default:
			vmerr(vm,
			      "operand %u to foreach must be a list or vector",
			      (unsigned)m);
		}
		if(m == 1)
			len = len2;
		else if(len != len2)
			vmerr(vm,
			      "container operands must be of the same length");
	}
	cl = valcl(iargv[0]);
	argv = emalloc((argc-1)*sizeof(Val));
	for(i = 0; i < len; i++){
		for(m = 1; m < argc; m++){
			switch(Vkind(iargv[m])){
			case Qlist:
				l = vallist(iargv[m]);
				argv[m-1] = listref(vm, l, i);
				break;
			case Qvec:
				v = valvec(iargv[m]);
				argv[m-1] = vecref(v, i);
				break;
			default:
				fatal("bug");
			}
		}
		safedovm(vm, cl, argc-1, argv);
	}
	efree(argv);
	USED(rv);
}

static void
l1_map(VM *vm, Imm argc, Val *iargv, Val *rv)
{
	List *l, *r;
	Vec *k, *v;
	Tab *t;
	Closure *cl;
	Imm m, i, len, len2;
	Val x, *argv;

	if(argc < 2)
		vmerr(vm, "wrong number of arguments to map");
	checkarg(vm, "map", iargv, 0, Qcl);

	// tables: only one container operand
	if(Vkind(iargv[1]) == Qtab){
		if(argc > 2)
			vmerr(vm, "bad combination of containers");
		r = mklist();
		cl = valcl(iargv[0]);
		t = valtab(iargv[1]);
		k = tabenumkeys(t);
		v = tabenumvals(t);
		argv = emalloc(2*sizeof(Val));
		for(m = 0; m < v->len; m++) {
			argv[0] = vecref(k, m);
			argv[1] = vecref(v, m);
			x = safedovm(vm, cl, 2, argv);
			listins(vm, r, m, x);
		}
		efree(argv);
		*rv = mkvallist(r);
		return;
	}

	// list and vector: any number, but must have equal length
	len = 0;
	for(m = 1; m < argc; m++){
		switch(Vkind(iargv[m])){
		case Qlist:
			l = vallist(iargv[m]);
			len2 = listxlen(l->x);
			break;
		case Qvec:
			v = valvec(iargv[m]);
			len2 = v->len;
			break;
		default:
			vmerr(vm,
			      "operand %u to map must be a list or vector",
			      (unsigned)m);
		}
		if(m == 1)
			len = len2;
		else if(len != len2)
			vmerr(vm,
			      "container operands must be of the same length");
	}

	cl = valcl(iargv[0]);
	r = mklist();
	argv = emalloc((argc-1)*sizeof(Val));
	for(i = 0; i < len; i++){
		for(m = 1; m < argc; m++){
			switch(Vkind(iargv[m])){
			case Qlist:
				l = vallist(iargv[m]);
				argv[m-1] = listref(vm, l, i);
				break;
			case Qvec:
				v = valvec(iargv[m]);
				argv[m-1] = vecref(v, i);
				break;
			default:
				fatal("bug");
			}
		}
		x = safedovm(vm, cl, m-1, argv);
		listins(vm, r, i, x);
	}
	efree(argv);
	*rv = mkvallist(r);
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
	if(fd->flags&Ffn){
		if(fd->u.fn.close)
			fd->u.fn.close(&fd->u.fn);
	}else
		if(fd->u.cl.close)
			safedovm(vm, fd->u.cl.close, 0, 0);
	USED(rv);
}

static void
l1_fdname(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to fdname");
	checkarg(vm, "fdname", argv, 0, Qfd);
	fd = valfd(argv[0]);
	*rv = mkvalstr(fd->name);
}

static void
l1_mkfd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Fd *fd;
	Closure *r, *w, *c;
	Str *n;

	if(argc != 3 && argc != 4)
		vmerr(vm, "wrong number of arguments to mkfd");
	if(Vkind(argv[0]) != Qcl && Vkind(argv[0]) != Qnil)
		vmerr(vm, "argument 1 to mkfd must be a function or nil");
	if(Vkind(argv[1]) != Qcl && Vkind(argv[1]) != Qnil)
		vmerr(vm, "argument 2 to mkfd must be a function or nil");
	if(Vkind(argv[2]) != Qcl && Vkind(argv[2]) != Qnil)
		vmerr(vm, "argument 3 to mkfd must be a function or nil");

	r = w = c = 0;
	if(Vkind(argv[0]) == Qcl)
		r = valcl(argv[0]);
	if(Vkind(argv[1]) == Qcl)
		w = valcl(argv[1]);
	if(Vkind(argv[2]) == Qcl)
		c = valcl(argv[2]);
	if(argc == 4){
		checkarg(vm, "mkfd", argv, 3, Qstr);
		n = valstr(argv[3]);
	}else
		n = mkstr0("");
	fd = mkfdcl(n, Fread|Fwrite, r, w, c);
	*rv = mkvalfd(fd);
}

static void
l1_mknas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mknas");
	USED(argv);
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
	as = mkzas(cv->val);
	*rv = mkvalas(as);
}

static void
l1_mkas(VM *vm, Imm argc, Val *argv, Val *rv)
{
	As *as;
	Str *name;
	Tab *mtab;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkas");
	checkarg(vm, "mkas", argv, 0, Qtab);
	name = 0;
	if(argc == 2){
		checkarg(vm, "mkas", argv, 1, Qstr);
		name = valstr(argv[1]);
	}
	mtab = valtab(argv[0]);
	as = mkastab(mtab, name);
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
	ns = mknsraw(vm, ons, rawtype, rawsym, name);
	*rv = mkvalns(ns);
}

static void
l1_mkattr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to mkattr");
	if(Vkind(argv[0]) != Qcval && Vkind(argv[0]) != Qtab)
		vmerr(vm, "argument 1 to mkattr must be a table or cvalue");
	*rv = mkattr(argv[0]);
}

static void
l1_malloc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *len;
	Str *s;
	As *as;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to malloc");
	checkarg(vm, "malloc", argv, 0, Qcval);
	len = valcval(argv[0]);
	if(!isnatcval(len))
		vmerr(vm, "malloc expects a non-negative length");
	s = mkstrn(len->val);
	as = mkmas(s);
	*rv = mkvalcval(mkdom(litdom->ns, as, mkstr0("malloc")),
			mkptrxtn(litdom->ns->base[Vchar],
				 litdom->ns->base[Vptr]->rep),
			(Imm)s->s);
}

static void
l1_attroff(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to attroff");
	checkarg(vm, "attroff", argv, 0, Qtab);
	*rv = attroff(argv[0]);
}

static void
l1_mkns(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Ns *ns;
	Str *name;
	Tab *mtab;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkns");
	checkarg(vm, "mkns", argv, 0, Qtab);
	name = 0;
	if(argc == 2){
		checkarg(vm, "mkns", argv, 1, Qstr);
		name = valstr(argv[1]);
	}
	mtab = valtab(argv[0]);
	ns = mknstab(mtab, name);
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
l1_nameof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Ns *ns;
	As *as;
	Str *name;

	name = 0;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nameof");
	if(Vkind(argv[0]) == Qdom){
		dom = valdom(argv[0]);
		name = dom->name;
	}else if(Vkind(argv[0]) == Qns){
		ns = valns(argv[0]);
		name = ns->name;
	}else if(Vkind(argv[0]) == Qas){
		as = valas(argv[0]);
		name = as->name;
	}else
		vmerr(vm, "operand 1 to nameof must be a domain, name space"
		      " or address space");
	if(name)
		*rv = mkvalstr(name);
}

static void
l1_asof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Dom *dom;
	Cval *cv;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to asof");
	dom = 0;
	if(Vkind(argv[0]) == Qdom)
		dom = valdom(argv[0]);
	else if(Vkind(argv[0]) == Qcval){
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

	dom = 0;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to nsof");
	if(Vkind(argv[0]) == Qdom)
		dom = valdom(argv[0]);
	else if(Vkind(argv[0]) == Qcval){
		cv = valcval(argv[0]);
		dom = cv->dom;
	}else
		vmerr(vm, "operand 1 to nsof must be a domain or cvalue");
	*rv = mkvalns(dom->ns);
}

static void
l1_callmethod(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val this, id, args, v, *xargv;
	Dom *dom;
	As *as;
	Ns *ns;
	Str *s;
	Closure *cl, *dcl;
	Imm ll, xargc;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to callmethod");
	checkarg(vm, "callmethod", argv, 1, Qstr);
	checkarg(vm, "callmethod", argv, 2, Qlist);
	this = argv[0];
	id = argv[1];
	args = argv[2];
	dcl = 0;
	v = 0;

	if(Vkind(this) == Qas){
		as = valas(this);
		v = tabget(as->mtab, id);
		if(v == 0)
			dcl = as->dispatch;
	}else if(Vkind(this) == Qns){
		ns = valns(this);
		v = tabget(ns->mtab, id);
		if(v == 0)
			dcl = ns->dispatch;
	}else if(Vkind(this) == Qdom){
		dom = valdom(this);
		/* search as, then ns */
		v = tabget(dom->as->mtab, id);
		if(v == 0)
			v = tabget(dom->ns->mtab, id);
		if(v == 0)
			dcl = dom->as->dispatch;
		if(dcl == 0)
			dcl = dom->ns->dispatch;
	}else
		vmerr(vm,
		      "operand 2 to callmethod must be an address space, "
		      "name space, or domain");

	if(v == 0 && dcl == 0){
		s = valstr(id);
		vmerr(vm, "callmethod target must define %.*s or dispatch",
		      (int)s->len, s->s);
	}

	listlen(args, &ll);
	if(v){
		cl = valcl(v);
		xargc = ll+1;
		xargv = emalloc(xargc*sizeof(Val));
		xargv[0] = this;
		listcopyv(vallist(args), 0, ll, xargv+1);
	}else{
		cl = dcl;
		xargc = ll+2;
		xargv = emalloc(xargc*sizeof(Val));
		xargv[0] = this;
		xargv[1] = id;
		listcopyv(vallist(args), 0, ll, xargv+2);
	}
	if(waserror(vm)){
		efree(xargv);
		nexterror(vm);
	}
	*rv = safedovm(vm, cl, xargc, xargv);
	poperror(vm);
	efree(xargv);
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
	if(Vkind(arg0) != Qns && Vkind(arg0) != Qdom)
		vmerr(vm,
		      "operand 1 to nslookaddr must be a namespace or domain");
	if(Vkind(arg0) == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	if(ns->lookaddr)
		*rv = mkvalcl(ns->lookaddr);
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
	if(Vkind(arg0) != Qns && Vkind(arg0) != Qdom)
		vmerr(vm,
		      "operand 1 to nsenumsym must be a namespace or domain");
	if(Vkind(arg0) == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	if(ns->enumsym)
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
	if(Vkind(arg0) != Qns && Vkind(arg0) != Qdom)
		vmerr(vm,
		      "operand 1 to nsenumtype must be a namespace or domain");
	if(Vkind(arg0) == Qns)
		ns = valns(arg0);
	else{
		dom = valdom(arg0);
		ns = dom->ns;
	}
	if(ns->enumtype)
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
	if(Vkind(arg0) != Qns && Vkind(arg0) != Qdom)
		vmerr(vm,
		      "operand 1 to nsptr must be a namespace or domain");
	if(Vkind(arg0) == Qns)
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
	if(!isnatcval(cv))
		vmerr(vm, "operand 1 to mkstr must be a non-negative integer");
	*rv = mkvalstr(mkstrn(cv->val));
}

static void
l1_strlen(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to strlen");
	s = valstrorcval(vm, "strlen", argv, 0);
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], s->len);
}

static void
l1_substr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *b, *e;
	Str *s;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to substr");
	s = valstrorcval(vm, "substr", argv, 0);
	checkarg(vm, "substr", argv, 1, Qcval);
	checkarg(vm, "substr", argv, 2, Qcval);
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
l1_strref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *str;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to strref");
	checkarg(vm, "strref", argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to strref must be "
		      "a non-negative integer");
	str = valstrorcval(vm, "strref", argv, 0);
	if(cv->val >= str->len)
		vmerr(vm, "strref out of bounds");
	*rv = mkvallitcval(Vuchar, (u8)str->s[cv->val]);
}

static char*
xmemmem(char *s1, Imm l1, char *s2, Imm l2)
{
	char *p, *e;
	if(l2 > l1)
		return 0;
	p = s1;
	e = s1+(l1-l2+1);
	while(p < e){
		if(!memcmp(p, s2, l2))
			return p;
		p++;
	}
	return 0;
}

// FIXME: the interface and implementation are broken
static void
l1_strstr(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char *s1, *s2, *p;
	Str *str1, *str2;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to strstr");
	str1 = valstrorcval(vm, "strstr", argv, 0);
	s1 = str2cstr(str1);
	str2 = valstrorcval(vm, "strstr", argv, 1);
	s2 = str2cstr(str2);
	if(Vkind(argv[0]) == Qcval && Vkind(argv[1]) == Qcval)
		p = strstr(s1, s2);
	else
		p = xmemmem(s1, str1->len, s2, str2->len);
	if(p)
		*rv = mkvallitcval(Vuvlong, p-s1);
	efree(s1);
	efree(s2);
}

static void
l1_memset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	unsigned char b;
	Cval *bcv, *lcv;
	Imm lim;

	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to memset");
	s = valstrorcval(vm, "memset", argv, 0);
	checkarg(vm, "memset", argv, 1, Qcval);
	bcv = valcval(argv[1]);
	b = bcv->val&0xff;
	if(argc == 3){
		checkarg(vm, "memset", argv, 2, Qcval);
		lcv = valcval(argv[2]);
		lim = lcv->val;
	}else
		lim = s->len;
	memset(s->s, b, lim);
	USED(rv);
}

static void
l1_memcpy(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *ncv, *scv, *dcv;
	Str *buf;

	if(argc != 3)
		vmerr(vm, "wrong number of argument to memcpy");

	checkarg(vm, "memcpy", argv, 0, Qcval);
	checkarg(vm, "memcpy", argv, 1, Qcval);
	checkarg(vm, "memcpy", argv, 2, Qcval);
	dcv = valcval(argv[0]);
	scv = valcval(argv[1]);
	ncv = valcval(argv[2]);
	if(!isstrcval(dcv))
		vmerr(vm, "operand 1 to memcpy must be a char* or "
		      "unsigned char*");
	if(!isstrcval(scv))
		vmerr(vm, "operand 2 to memcpy must be a char* or "
		      "unsigned char*");
	buf = callget(vm, scv->dom->as, scv->val, ncv->val);
	callput(vm, dcv->dom->as, dcv->val, ncv->val, buf);
	USED(rv);
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
	Cval *cv;
	char *err;
	unsigned radix;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to strton");
	checkarg(vm, "strton", argv, 0, Qstr);
	radix = 0;
	if(argc == 2){
		checkarg(vm, "strton", argv, 1, Qcval);
		cv = valcval(argv[1]);
		if(!isnatcval(cv))
			vmerr(vm, "operand 2 to strton must be "
			      "non-negative");
		radix = cv->val;
	}

	s = valstrorcval(vm, "strton", argv, 0);
	if(0 != parseliti(s->s, s->len, &liti, radix, &err))
		vmerr(vm, err);
	*rv = mkvalcval(litdom, litdom->ns->base[liti.base], liti.val);
}

static void
l1_split(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	char *p, *q, *e, *set;
	Cval *lim;
	Imm n;
	List *r;
	int intok, mflag;

	r = mklist();

	/* subject string */
	if(argc != 1 && argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to split");
	s = valstrorcval(vm, "split", argv, 0);
	p = s->s;
	e = s->s+s->len;

	/* split limit */
	lim = 0;
	if(argc == 3){
		checkarg(vm, "split", argv, 2, Qcval);
		lim = valcval(argv[2]);
		if(!isnatcval(lim))
			vmerr(vm, "split expects a non-negative limit");
		if(lim->val == 0){
			*rv = mkvallist(r);
			return;
		}
	}

	/* delimiter set */
	if(argc > 1 && (s = valstrorcvalornil(vm, "split", argv, 1))){
		set = str2cstr(s);
		mflag = 0;
	}else{
		set = xstrdup(" \t\n");
		mflag = 1;
	}

	if(!mflag){
		n = 0;
		q = p;
		while(q < e && (!lim || n < lim->val)){
			if(strchr(set, *q)){
				listins(vm, r, n++, mkvalstr(mkstr(p, q-p)));
				p = q+1;
				q = p;
			}else
				q++;
		}
		if(p < e)
			listins(vm, r, n, mkvalstr(mkstr(p, e-p)));
	}else{
		n = 0;
		q = p;
		intok = 0;
		while(q < e && (!lim || n < lim->val)){
			if(strchr(set, *q)){
				if(intok)
					listins(vm, r, n++,
						mkvalstr(mkstr(p, q-p)));
				intok = 0;
				p = q+1;
				while(p < e && strchr(set, *p))
					p++;
				if(p >= e)
					break;
				q = p;
			}else{
				q++;
				intok = 1;
			}
		}
		if(intok)
			listins(vm, r, n, mkvalstr(mkstr(p, e-p)));
	}
	efree(set);
	*rv = mkvallist(r);
}

static void
l1_mkvec(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val v;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mkvec");
	checkarg(vm, "mkvec", argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(argc == 2)
		v = argv[1];
	else
		v = Xnil;
	if(!isnatcval(cv))
		vmerr(vm, "operand 1 to mkvec must be a non-negative integer");
	*rv = mkvalvec(mkvecinit(cv->val, v));
}

static void
l1_vector(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Vec *v;
	Imm i;

	USED(vm);
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
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], v->len);
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
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to vecref must be "
		      "a non-negative integer");
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
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to vecset must be "
		      "a non-negative integer");
	if(cv->val >= v->len)
		vmerr(vm, "vecset out of bounds");
	vecset(v, cv->val, argv[2]);
	USED(rv);
}

static void
l1_mktab(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to mktab");
	USED(argv);
	*rv = mkvaltab(mktab());
}

static void
l1_tabdelete(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Tab *t;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to tabdelete");
	checkarg(vm, "tabdelete", argv, 0, Qtab);
	if(Vkind(argv[1]) == Qundef)
		vmerr(vm, "attempt to access table with undefined key value");
	t = valtab(argv[0]);
	tabdel(t, argv[1]);
	USED(rv);
}

static void
l1_tablook(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Tab *t;
	Val vp;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to tablook");
	checkarg(vm, "tablook", argv, 0, Qtab);
	if(Vkind(argv[1]) == Qundef)
		vmerr(vm, "attempt to access table with undefined key value");
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
	if(Vkind(argv[1]) == Qundef)
		vmerr(vm, "attempt to access table with undefined key value");
	t = valtab(argv[0]);
	tabput(t, argv[1], argv[2]);
	USED(rv);
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
	Imm sz;
	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to ismapped");
	checkarg(vm, "ismapped", argv, 0, Qcval);
	addr = valcval(argv[0]);
	sz = 0;
	if(argc == 2){
		checkarg(vm, "ismapped", argv, 1, Qcval);
		len = valcval(argv[1]);
		if(!isnatcval(len))
			vmerr(vm, "ismapped expects a non-negative length");
		sz = len->val;
	}
	if(sz == 0)
		sz = typesize(vm, addr->type);
	if(ismapped(vm, addr->dom->as, addr->val, sz))
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
	*rv = mkvalstr(getbytes(vm, addr, n));
}

static void
l1_putbytes(VM *vm, Imm iargc, Val *iargv, Val *rv)
{
	Cval *addr;
	Str *str;
	Xtypename *t;

	if(iargc != 2)
		vmerr(vm, "wrong number of arguments to putbytes");
	checkarg(vm, "putbytes", iargv, 0, Qcval);
	checkarg(vm, "putbytes", iargv, 1, Qstr);
	addr = valcval(iargv[0]);
	str = valstr(iargv[1]);
	t = chasetype(addr->type);
	if(t->tkind != Tptr)
		vmerr(vm, "operand 1 to putbytes must be a pointer");
	callput(vm, addr->dom->as, addr->val, str->len, str);
	USED(rv);
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
	argv = emalloc(argc*sizeof(Val));
	ap = argv;
	ip = &iargv[1];
	for(m = 0; m < iargc-2; m++)
		*ap++ = *ip++;
	lst = vallist(*ip);
	for(m = 0; m < ll; m++){
		vp = listref(vm, lst, m);
		*ap++ = vp;
	}
	if(waserror(vm)){
		efree(argv);
		nexterror(vm);
	}
	vp = safedovm(vm, cl, argc, argv);
	*rv = vp;
	efree(argv);
	poperror(vm);
}

static void
l1_isempty(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to isempty");
	if(Vkind(argv[0]) == Qlist){
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
	Tab *tab;
	Imm len;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to length");
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to length must be a container");
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
	case Qtab:
		tab = valtab(argv[0]);
		len = tab->cnt;
		break;
	}
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], len);
}

static void
l1_count(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Vec *vec;
	Str *str;
	Imm len, i, m;
	Listx *x;
	char c;
	Cval *cv;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to count");
	v = argv[1];
	m = 0;
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to count must be a list, string, "
		      "or vector");
	case Qlist:
		lst = vallist(argv[0]);
		x = lst->x;
		len = listxlen(x);
		for(i = 0; i < len; i++)
			if(eqval(v, x->val[x->hd+i]))
				m++;
		break;
	case Qstr:
		if(Vkind(v) != Qcval)
			vmerr(vm, "operand 2 to count must a character when"
			      " operand 1 is a string");
		cv = valcval(v);
		c = (char)cv->val;
		str = valstr(argv[0]);
		len = str->len;
		for(i = 0; i < len; i++)
			if(c == str->s[i])
				m++;
		break;
	case Qvec:
		vec = valvec(argv[0]);
		len = vec->len;
		for(i = 0; i < len; i++)
			if(eqval(v, vec->vec[i]))
				m++;
		break;
	}
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], m);
}

static void
l1_index(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Vec *vec;
	Str *str;
	Imm len, i;
	Listx *x;
	char c;
	Cval *cv;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to index");
	v = argv[1];
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to index must be a list, string, "
		      "or vector");
	case Qlist:
		lst = vallist(argv[0]);
		x = lst->x;
		len = listxlen(x);
		for(i = 0; i < len; i++)
			if(eqval(v, x->val[x->hd+i]))
				goto gotit;
		break;
	case Qstr:
		if(Vkind(v) != Qcval)
			vmerr(vm, "operand 2 to index must a character when"
			      " operand 1 is a string");
		cv = valcval(v);
		c = (char)cv->val;
		str = valstr(argv[0]);
		len = str->len;
		for(i = 0; i < len; i++)
			if(c == str->s[i])
				goto gotit;
		break;
	case Qvec:
		vec = valvec(argv[0]);
		len = vec->len;
		for(i = 0; i < len; i++)
			if(eqval(v, vec->vec[i]))
				goto gotit;
		break;
	}
	return;   /* nil */
gotit:
	*rv = mkvalcval(litdom, litdom->ns->base[Vuvlong], i);
}

static void
l1_ismember(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Vec *vec;
	Str *str;
	Imm len, i;
	Listx *x;
	char c;
	Cval *cv;
	Tab *tab;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to ismember");
	*rv = mkvalcval2(cval0);
	v = argv[1];
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to ismember must be a list, string, "
		      "table, or vector");
	case Qlist:
		lst = vallist(argv[0]);
		x = lst->x;
		len = listxlen(x);
		for(i = 0; i < len; i++)
			if(eqval(v, x->val[x->hd+i]))
				goto gotit;
		break;
	case Qstr:
		if(Vkind(v) != Qcval)
			vmerr(vm, "operand 2 to ismember must a character when"
			      " operand 1 is a string");
		cv = valcval(v);
		c = (char)cv->val;
		str = valstr(argv[0]);
		len = str->len;
		for(i = 0; i < len; i++)
			if(c == str->s[i])
				goto gotit;
		break;
	case Qvec:
		vec = valvec(argv[0]);
		len = vec->len;
		for(i = 0; i < len; i++)
			if(eqval(v, vec->vec[i]))
				goto gotit;
		break;
	case Qtab:
		tab = valtab(argv[0]);
		if(tabget(tab, argv[1]))
			goto gotit;
		break;
	}
	return;
gotit:
	*rv = mkvalcval2(cval1);;
}

static void
l1_delete(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val v;
	List *lst;
	Imm len, i;
	Listx *x;
	Tab *tab;

	if(argc != 2)
		vmerr(vm, "wrong number of arguments to delete");
	v = argv[1];
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to delete must be a list or table");
	case Qlist:
		lst = vallist(argv[0]);
		x = lst->x;
		len = listxlen(x);
		for(i = 0; i < len; i++)
			if(eqval(v, x->val[x->hd+i]))
				listdel(vm, lst, i);
		break;
	case Qtab:
		tab = valtab(argv[0]);
		tabdel(tab, v);
		break;
	}
	*rv = argv[0];
}

static void
l1_null(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to null");
	USED(argv);
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
l1_pop(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val arg;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to pop");
	arg = argv[0];
	if(Vkind(arg) == Qlist)
		listpop(vallist(argv[0]), rv);
	else if(Vkind(arg) == Qtab)
		tabpop(valtab(argv[0]), rv);
	else
		vmerr(vm, "operand 1 to pop must be a list or table");
}

static void
l1_copy(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to append");
	if(Vkind(argv[0]) == Qlist)
		*rv = mkvallist(listcopy(vallist(argv[0])));
	else if(Vkind(argv[0]) == Qvec)
		*rv = mkvalvec(veccopy(valvec(argv[0])));
	else if(Vkind(argv[0]) == Qtab)
		*rv = mkvaltab(tabcopy(valtab(argv[0])));
	else if(Vkind(argv[0]) == Qstr)
		*rv = mkvalstr(strcopy(valstr(argv[0])));
	else
		vmerr(vm, "operand 1 to copy must be a container");
}

static void
l1_concat(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Str *str;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to concat");
	if(Vkind(argv[0]) == Qlist && Vkind(argv[1]) == Qlist){
		lst = listconcat(vm, vallist(argv[0]), vallist(argv[1]));
		*rv = mkvallist(lst);
	}else if(Vkind(argv[0]) == Qstr && Vkind(argv[1]) == Qstr){
		str = strconcat(valstr(argv[0]), valstr(argv[1]));
		*rv = mkvalstr(str);
	}else
		vmerr(vm, "operands to concat must be lists or strings");
}

static void
l1_rdof(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rec *r;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdof");
	checkarg(vm, "rdof", argv, 0, Qrec);
	r = valrec(argv[0]);
	*rv = mkvalrd(r->rd);
}

static void
l1_mkrd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Closure *fmt;
	List *lst;
	Imm n, nf;
	Val v;

	if(argc != 2 && argc != 3)
		vmerr(vm, "wrong number of arguments to mkrd");
	checkarg(vm, "mkrd", argv, 0, Qstr);
	checkarg(vm, "mkrd", argv, 1, Qlist);

	fmt = 0;
	if(argc == 3){
		checkarg(vm, "mkrd", argv, 1, Qcl);
		fmt = valcl(argv[2]);
	}
	listlen(argv[1], &nf);
	lst = vallist(argv[1]);
	for(n = 0; n < nf; n++){
		v = listref(vm, lst, n);
		if(Vkind(v) != Qstr)
			vmerr(vm, "operand 2 to mkrd must be a "
			      "list of field names");
	}
	*rv = mkvalrd(mkrd(vm, valstr(argv[0]), lst, fmt));
}

static void
l1_rdname(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdname");
	checkarg(vm, "rdname", argv, 0, Qrd);
	rd = valrd(argv[0]);
	*rv = mkvalstr(rd->name);
}

static void
l1_rdis(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdis");
	checkarg(vm, "rdis", argv, 0, Qrd);
	rd = valrd(argv[0]);
	*rv = mkvalcl(rd->is);
}


static void
l1_rdmk(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdmk");
	checkarg(vm, "rdmk", argv, 0, Qrd);
	rd = valrd(argv[0]);
	*rv = mkvalcl(rd->mk);
}

static void
l1_rdfmt(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdfmt");
	checkarg(vm, "rdfmt", argv, 0, Qrd);
	rd = valrd(argv[0]);
	*rv = mkvalcl(rd->fmt);
}

static void
l1_rdsetfmt(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	Closure *cl;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to rdsetfmt");
	checkarg(vm, "rdsetfmt", argv, 0, Qrd);
	checkarg(vm, "rdsetfmt", argv, 1, Qcl);
	rd = valrd(argv[0]);
	cl = valcl(argv[1]);
	rd->fmt = cl;
	USED(rv);
}

static void
l1_rdfields(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdfields");
	checkarg(vm, "rdname", argv, 0, Qrd);
	rd = valrd(argv[0]);
	*rv = mkvallist(rd->fname);
}

static void
l1_rdgettab(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdgettab");
	checkarg(vm, "rdgettab", argv, 0, Qrd);
	rd = valrd(argv[0]);
	*rv = mkvaltab(rd->get);
}

static void
l1_rdsettab(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Rd *rd;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to rdsettab");
	checkarg(vm, "rdsettab", argv, 0, Qrd);
	rd = valrd(argv[0]);
	*rv = mkvaltab(rd->set);
}

static void
l1_cntrget(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to cntrget");
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to cntrget must be a container");
	case Qlist:
		l1_listref(vm, argc, argv, rv);
		break;
	case Qstr:
		l1_strref(vm, argc, argv, rv);
		break;
	case Qvec:
		l1_vecref(vm, argc, argv, rv);
		break;
	case Qtab:
		l1_tablook(vm, argc, argv, rv);
		break;
	}
}

static void
l1_cntrput(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to cntrput");
	switch(Vkind(argv[0])){
	default:
		vmerr(vm, "operand 1 to cntrput must be a container");
	case Qlist:
		l1_listset(vm, argc, argv, rv);
		*rv = argv[2];
		break;
	case Qstr:
		l1_strput(vm, argc, argv, rv);
		if(Vkind(argv[2]) == Qcval){
			cv = valcval(argv[2]);
			cv = typecast(vm, cv->dom->ns->base[Vchar], cv);
			*rv = mkvalcval2(cv);
		}else
			*rv = argv[2];
		break;
	case Qvec:
		l1_vecset(vm, argc, argv, rv);
		*rv = argv[2];
		break;
	case Qtab:
		l1_tabinsert(vm, argc, argv, rv);
		*rv = argv[2];
		break;
	}
}

static void
l1_equal(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Qkind kind;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to equal");
	kind = Vkind(argv[0]);
	if(kind != Vkind(argv[1]))
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
l1_toupper(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	int x;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to toupper");
	checkarg(vm, "toupper", argv, 0, Qcval);
	cv = valcval(argv[0]);
	x = xtoupper(cv->val);
	*rv = mkvalcval(cv->dom, cv->type, x);
}

static void
l1_tolower(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	int x;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tolower");
	checkarg(vm, "tolower", argv, 0, Qcval);
	cv = valcval(argv[0]);
	x = xtolower(cv->val);
	*rv = mkvalcval(cv->dom, cv->type, x);
}

static void
l1_isxctype(VM *vm, Imm argc, Val *argv, Val *rv, char *name, int (*fn)(int))
{
	Cval *cv;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", name);
	checkarg(vm, name, argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(fn((int)cv->val))
		*rv = mkvalcval2(cval1);
	else
		*rv = mkvalcval2(cval0);
}

static void
l1_isalnum(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isalnum", xisalnum);
}

static void
l1_isalpha(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isalpha", xisalpha);
}

static void
l1_isascii(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isascii", xisascii);
}

static void
l1_isblank(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isblank", xisblank);
}

static void
l1_iscntrl(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "iscntrl", xiscntrl);
}

static void
l1_isdigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isdigit", xisdigit);
}

static void
l1_isgraph(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isgraph", xisgraph);
}

static void
l1_islower(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "islower", xislower);
}

static void
l1_isodigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isodigit", xisodigit);
}

static void
l1_isprint(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isprint", xisprint);
}

static void
l1_ispunct(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "ispunct", xispunct);
}

static void
l1_isspace(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isspace", xisspace);
}

static void
l1_isupper(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isupper", xisupper);
}

static void
l1_isxdigit(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isxctype(vm, argc, argv, rv, "isxdigit", xisxdigit);
}

static void
l1_isx(VM *vm, Imm argc, Val *argv, Val *rv, char *name, Qkind kind)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to %s", name);
	if(Vkind(argv[0]) == kind)
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
l1_isdom(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isdom", Qdom);
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
	l1_isx(vm, argc, argv, rv, "isnull", Qnull);
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
l1_isrec(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isrec", Qrec);
}

static void
l1_isrd(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isrd", Qrd);
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
l1_isundefined(VM *vm, Imm argc, Val *argv, Val *rv)
{
	l1_isx(vm, argc, argv, rv, "isundefined", Qundef);
}

static void
l1_finalize(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Head *hd;
	Closure *cl, *ocl;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to finalize");
	hd = (Head*)argv[0];
	if(Vkind(argv[1]) == Qcl){
		cl = valcl(argv[1]);
		ocl = valcl(tabget(finals, hd));
		if(ocl)
			ocl = gcunprotect(ocl);
		cl = gcprotect(cl);
		tabput(finals, hd, mkvalcl(cl));
//		Vsetfinal(hd, 1);
//	        xprintf("set final on %p (%d)\n", hd, Vfinal(hd));
	}else if(Vkind(argv[1]) == Qnil){
		tabdel(finals, hd);
//		Vsetfinal(hd, 0);
	}else
		vmerr(vm, "argument 1 to finalize must be a function or nil");
	USED(rv);
}

static void
l1_gcstat(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(vm);
	USED(argc);
	USED(argv);
	*rv = mkvalstr(gcstat());
}

static void
l1_meminuse(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(vm);
	USED(argc);
	USED(argv);
	*rv = mkvallitcval(Vuvlong, cqctmeminuse+meminuse());
}

static void
l1_memtotal(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(vm);
	USED(argc);
	USED(argv);
	*rv = mkvallitcval(Vuvlong, cqctmemtotal);
}

static void
l1_gc(VM *vm, Imm argc, Val *argv, Val *rv)
{
	gc(0, 1);
}

static void
l1_eval(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *str;
	Val cl;
	char *s;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to eval");
	checkarg(vm, "eval", argv, 0, Qstr);
	str = valstr(argv[0]);
	s = str2cstr(str);
	cl = cqctcompile(s, "<eval-input>", vm->top, 0);
	efree(s);
	if(cl == 0)
		return;
	*rv = dovm(vm, valcl(cl), 0, 0);
}

static void
l1_resettop(VM *vm, Imm argc, Val *argv, Val *rv)
{
	USED(argc);
	USED(argv);
	freeenv(vm->top->env);
	vm->top->env = mktopenv();
	USED(rv);
}

static void
l1_loadpath(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char **lp;
	List *l;
	if(argc != 0)
		vmerr(vm, "wrong number of arguments to loadpath");
	USED(argv);
	l = mklist();
	lp = cqctloadpath;
	while(*lp){
		listappend(vm, l, mkvalstr(mkstr0(*lp)));
		lp++;
	}
	*rv = mkvallist(l);
}

static void
l1_setloadpath(VM *vm, Imm argc, Val *argv, Val *rv)
{
	char **lp;
	List *l;
	Imm i, m;
	Val v;

	if(argc != 1)
		vmerr(vm, "wrong number of arguments to loadpath");
	checkarg(vm, "setloadpath", argv, 0, Qlist);
	l = vallist(argv[0]);
	m = listxlen(l->x);
	for(i = 0; i < m; i++){
		v = listref(vm, l, i);
		if(Vkind(v) != Qstr)
			vmerr(vm, "argument 1 to setloadpath "
			      "must be a list of strings");
	}
	lp = emalloc((m+1)*sizeof(char*));
	for(i = 0; i < m; i++)
		lp[i] = str2cstr(valstr(listref(vm, l, i)));
	efree(cqctloadpath);
	cqctloadpath = copystrv(lp);
	for(i = 0; i < m; i++)
		efree(lp[i]);
	efree(lp);
	USED(rv);
}

static Val
expr2list(Expr *e)
{
	List *l;
	Expr *p;

	if(e == 0)
		return Xnil;
	l = mklist();
	_listappend(l, mkvalstr(mkstr0(S[e->kind])));
	_listappend(l, mkvalstr(mkstr0(e->src.filename)));
	_listappend(l, mkvallitcval(Vuint, e->src.line));
	switch(e->kind){
	case Eid:
		_listappend(l, mkvalstr(mkstr0(e->id)));
		break;
	case Econst:
		_listappend(l, mkvallitcval(e->liti.base, e->liti.val));
		break;
	case Econsts:
		_listappend(l, mkvalstr(mkstr(e->lits->s, e->lits->len)));
		break;
	case Ebinop:
	case Egop:
		_listappend(l, mkvalstr(mkstr0(S[e->op])));
		_listappend(l, expr2list(e->e1));
		_listappend(l, expr2list(e->e2));
		break;
	case Eelist:
		p = e;
		while(p->kind == Eelist){
			_listappend(l, expr2list(p->e1));
			p = p->e2;
		}
		break;
	default:
		_listappend(l, expr2list(e->e1));
		_listappend(l, expr2list(e->e2));
		_listappend(l, expr2list(e->e3));
		_listappend(l, expr2list(e->e4));
		break;
	}
	return mkvallist(l);
}

static void
l1_parse(VM *vm, Imm argc, Val *argv, Val *rv)
{
	U ctx;
	Expr *e;
	char *whence;
	char *buf;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to parse");
	checkarg(vm, "parse", argv, 0, Qstr);
	if(argc == 2)
		checkarg(vm, "parse", argv, 1, Qstr);
	buf = str2cstr(valstr(argv[0]));
	whence = "<stdin>";
	if(argc == 2)
		whence = str2cstr(valstr(argv[1]));
	memset(&ctx, 0, sizeof(ctx));
	ctx.out = &vm->top->out;
	e = doparse(&ctx, buf, whence);
	if(e == 0)
		vmerr(vm, "could not parse expression");
	*rv = expr2list(e);
	freeexpr(e);
}

static void
l1_cval2str(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Str *s;
	Cval *cv;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to cval2str");
	checkarg(vm, "cval2str", argv, 0, Qcval);
	cv = valcval(argv[0]);
	s = imm2str(cv->type, cv->val);
	*rv = mkvalstr(s);
}

char*
cqctsprintval(VM *vm, Val v)
{
	Val argv[2], rv;

	Str *s;
	s = mkstrk("%a", 2, Sperm);
	argv[0] = mkvalstr(s);
	argv[1] = v;
	l1_sprintfa(vm, 2, argv, &rv);
	return str2cstr(valstr(rv));
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
mkvoidxtn(void)
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
		kv = mkvalxtn(mkbasextn(cb, Rundef));
		vv = mkvalxtn(base[cb]);
		tabput(type, kv, vv);
	}

	/* map pointer to integer representation */
	kv = mkvalxtn(mkbasextn(Vptr, Rundef));
	vv = mkvalxtn(base[Vptr]);
	tabput(type, kv, vv);

	/* define stdint-like integer typedefs */

	tn = mkstr0("uintptr");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->ptr]));
	tabput(type, kv, vv);

	tn = mkstr0("int8");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint8]));
	tabput(type, kv, vv);
	tn = mkstr0("int16");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint16]));
	tabput(type, kv, vv);
	tn = mkstr0("int32");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint32]));
	tabput(type, kv, vv);
	tn = mkstr0("int64");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xint64]));
	tabput(type, kv, vv);

	tn = mkstr0("uint8");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint8]));
	tabput(type, kv, vv);
	tn = mkstr0("uint16");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint16]));
	tabput(type, kv, vv);
	tn = mkstr0("uint32");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint32]));
	tabput(type, kv, vv);
	tn = mkstr0("uint64");
	kv = mkvalxtn(mktypedefxtn(tn, 0));
	vv = mkvalxtn(mktypedefxtn(tn, base[def->xuint64]));
	tabput(type, kv, vv);

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

static Dom*
mklitdom(void)
{
	return mkdom(mkrootns(&clp64le), mknas(), mkstr0("litdom"));
}

static Env*
mktopenv(void)
{
	Env *env;

	env = mkenv();

	builtinfn(env, "halt", halt);
	builtinfn(env, "callcc", callcc());

	FN(apply);
	FN(arraynelm);
	FN(asof);
	FN(attroff);
	FN(backtrace);
	FN(baseid);
	FN(bitfieldcontainer);
	FN(bitfieldpos);
	FN(bitfieldwidth);
	FN(bsearch);
	FN(callmethod);
	FN(car);
	FN(cdr);
	FN(close);
	FN(cntrget);
	FN(cntrput);
	FN(concat);
	FN(cons);
	FN(copy);
	FN(count);
	FN(cval2str);
	FN(delete);
	FN(domof);
	FN(enumconsts);
	FN(equal);
	FN(eval);
	FN(error);
	FN(fault);
	FN(fdname);
	FN(fieldattr);
	FN(fieldid);
	FN(fieldoff);
	FN(fields);
	FN(fieldtype);
	FN(finalize);
	FN(foreach);
	FN(gc);
	FN(gcstat);
	FN(getbytes);
	FN(index);
	FN(isalnum);
	FN(isalpha);
	FN(isarray);
	FN(isas);
	FN(isascii);
	FN(isbase);
	FN(isblank);
	FN(isbitfield);
	FN(iscntrl);
	FN(isctype);
	FN(iscvalue);
	FN(isdigit);
	FN(isdom);
	FN(isempty);
	FN(isenum);
	FN(isenumconst);
	FN(isfd);
	FN(isfunc);
	FN(isgraph);
	FN(islist);
	FN(islower);
	FN(ismapped);
	FN(ismember);
	FN(isnil);
	FN(isns);
	FN(isnull);
	FN(isodigit);
	FN(ispair);
	FN(isprint);
	FN(isprocedure);
	FN(isptr);
	FN(ispunct);
	FN(isrange);
	FN(isrec);
	FN(isrd);
	FN(isspace);
	FN(isstring);
	FN(isstruct);
	FN(issu);
	FN(istable);
	FN(istypedef);
	FN(isundefined);
	FN(isundeftype);
	FN(isunion);
	FN(isupper);
	FN(isvector);
	FN(isvoid);
	FN(isxaccess);
	FN(isxdigit);
	FN(length);
	FN(loadpath);
	FN(lookfield);
	FN(looksym);
	FN(looktype);
	FN(malloc);
	FN(map);
	FN(memcpy);
	FN(meminuse);
	FN(memset);
	FN(memtotal);
	FN(mkas);
	FN(mkattr);
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
	FN(mkctype_xaccess);
	FN(mkdom);
	FN(mkfd);
	FN(mkfield);
	FN(mknas);
	FN(mkns);
	FN(mknsraw);
	FN(mkparam);
	FN(mkrange);
	FN(mkrd);
	FN(mksas);
	FN(mkstr);
	FN(mksym);
	FN(mktab);
	FN(mkvec);
	FN(mkzas);
	FN(myrootns);
	FN(nameof);
	FN(nsof);
	FN(nsenumsym);
	FN(nsenumtype);
	FN(nslookaddr);
	FN(nsptr);
	FN(null);
	FN(paramid);
	FN(params);
	FN(paramtype);
	FN(parse);
	FN(pop);
	FN(putbytes);
	FN(rangebeg);
	FN(rangelen);
	FN(rdfields);
	FN(rdfmt);
	FN(rdgettab);
	FN(rdis);
	FN(rdmk);
	FN(rdname);
	FN(rdof);
	FN(rdsetfmt);
	FN(rdsettab);
	FN(resettop);
	FN(rettype);
	FN(setloadpath);
	FN(sort);
	FN(split);
	FN(sprintfa);
	FN(stringof);
	FN(strlen);
	FN(strput);
	FN(strref);
	FN(strstr);
	FN(strton);
	FN(substr);
	FN(subtype);
	FN(suekind);
	FN(suetag);
	FN(suattr);
	FN(susize);
	FN(symattr);
	FN(symid);
	FN(symoff);
	FN(symtype);
	FN(tabdelete);
	FN(tabenum);
	FN(tabinsert);
	FN(tabkeys);
	FN(tablook);
	FN(tabvals);
	FN(tolower);
	FN(toupper);
	FN(typedefid);
	FN(typedeftype);
	FN(veclen);
	FN(vecref);
	FN(vecset);
	FN(vector);
	FN(xaccessget);
	FN(xaccessput);

	fnlist(env);
	fns(env);		/* configuration-specific functions */

	/* FIXME: these bindings should be immutable */
	builtinval(env, "nil", Xnil);
	builtindom(env, "litdom", litdom);
	builtinns(env, "c32le", mkrootns(&c32le));
	builtinns(env, "c32be", mkrootns(&c32be));
	builtinns(env, "c64le", mkrootns(&c64le));
	builtinns(env, "c64be", mkrootns(&c64be));
	builtinns(env, "clp64le", mkrootns(&clp64le));
	builtinns(env, "clp64be", mkrootns(&clp64be));
	builtincval(env, "NULL", cvalnull);
	builtinnil(env, "$$");

	/* expanded source may call these magic functions */
	builtinfn(env, "$put", mkcfn("$put", l1_put));
	builtinfn(env, "$typeof", mkcfn("$typeof", l1_typeof));

	return env;
}

VM*
cqctmkvm(Toplevel *top)
{
	VM *vm, **vmp;

	vm = emalloc(sizeof(VM));
	vm->top = top;
	vm->emax = Errinitdepth;
	vm->err = emalloc(vm->emax*sizeof(Err));

	vmresetctl(vm);

	/* register vm with fault handler */
	vmp = vms;
	while(*vmp){
		vmp++;
		if(vmp > vms+Maxvms)
			fatal("too many vms");
	}
	*vmp = vm;
	nvms++;

	/* vm is now callable */
	return vm;
}

void
cqctfreevm(VM *vm)
{
	VM **vmp;
	efree(vm->err);
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp == vm){
			*vmp = 0;
			break;
		}
		vmp++;
	}
	efree(vm);
	nvms--;
}

static void
vmfaulthook()
{
	VM **vmp;
	vmp = vms;
	while(vmp < vms+Maxvms){
		if(*vmp){
			xprintf("backtrace of vm %p:\n", *vmp);
			fvmbacktrace(*vmp);
			xprintf("\n");
		}
		vmp++;
	}
}

void
initvm(int gcthread, u64 heapmax)
{
	Xundef = gcprotect(mal(Qundef));
	Xnil = gcprotect(mal(Qnil));
	Xnulllist = gcprotect(mal(Qnull));
	cccode = gcprotect(callccode());
	kcode = gcprotect(contcode());
	litdom = gcprotect(mklitdom());
	cvalnull = gcprotect(mkcval(litdom, litdom->ns->base[Vptr], 0));
	finals = gcprotect(mktab());
	finals->weak = 1;
	cval0 = gcprotect(mkcval(litdom, litdom->ns->base[Vint], 0));
	cval1 = gcprotect(mkcval(litdom, litdom->ns->base[Vint], 1));
	cvalminus1 = gcprotect(mkcval(litdom, litdom->ns->base[Vint], -1));
	halt = gcprotect(haltthunk());
	cqctfaulthook(vmfaulthook, 1);
	GCiterdone = emalloc(1); /* unique pointer */
}

void
finivm(void)
{
	gcunprotect(Xnulllist);
	gcunprotect(Xnil);
	gcunprotect(Xundef);
	gcunprotect(cccode);
	gcunprotect(kcode);
	gcunprotect(litdom);
	gcunprotect(cvalnull);
	gcunprotect(finals);
	gcunprotect(cval0);
	gcunprotect(cval1);
	gcunprotect(cvalminus1);
	gcunprotect(halt);
	cqctfaulthook(vmfaulthook, 0);
	efree(GCiterdone);
}

int
cqctcallfn(VM *vm, Val cl, int argc, Val *argv, Val *rv)
{
	if(waserror(vm))
		return -1;
	if(Vkind(cl) != Qcl)
		return -1;
	vm->flags &= ~VMirq;
	*rv = dovm(vm, valcl(cl), argc, argv);
	poperror(vm);
	return 0;
}

int
cqctcallthunk(VM *vm, Val cl, Val *rv)
{
	if(waserror(vm))
		return -1;
	if(Vkind(cl) != Qcl)
		return -1;
	vm->flags &= ~VMirq;
	*rv = dovm(vm, valcl(cl), 0, 0);
	poperror(vm);
	return 0;
}

Cbase
cqctvalcbase(Val v)
{
	Cval *cv;
	Xtypename *t;
	if(Vkind(v) != Qcval)
		return (Cbase)-1;
	cv = valcval(v);
	t = chasetype(cv->type);
	switch(t->tkind){
	case Tbase:
		return t->basename;
	case Tptr:
		return cv->dom->ns->base[Vptr]->basename;
	default:
		/* only scalar types in cvalues */
		fatal("bug");
	}
	return 0; /* not reached */
}

Val
cqctmkfd(Xfd *xfd, char *name)
{
	Fd *fd;
	Str *n;

	if(name)
		n = mkstr0(name);
	else
		n = mkstr0("");
	fd = mkfdfn(n, Fread|Fwrite, xfd);
	return mkvalfd(fd);
}

/* these cqctval<type> routines and their inverses assume litdom is clp64le */

int8_t
cqctvalint8(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint8];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

int16_t
cqctvalint16(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint16];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

int32_t
cqctvalint32(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint32];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

int64_t
cqctvalint64(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint64];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint8_t
cqctvaluint8(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint8];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint16_t
cqctvaluint16(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xint16];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint32_t
cqctvaluint32(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint32];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

uint64_t
cqctvaluint64(Val v)
{
	Cval *cv, *rv;
	Xtypename *t;

	if(Vkind(v) != Qcval)
		return -1;
	cv = valcval(v);
	t = litdom->ns->base[clp64le.xuint64];
	rv = mkcval(litdom, t, rerep(cv->val, cv->type, t));
	return rv->val;
}

char*
cqctvalcstr(Val v)
{
	if(Vkind(v) != Qstr)
		return 0;
	return str2cstr(valstr(v));
}

char*
cqctvalcstrshared(Val v)
{
	Str *s;
	if(Vkind(v) != Qstr)
		return 0;
	s = valstr(v);
	return s->s;
}

uint64_t
cqctvalcstrlen(Val v)
{
	Str *s;
	if(Vkind(v) != Qstr)
		return 0;
	s = valstr(v);
	return (uint64_t)s->len;
}

Val
cqctcstrval(char *s)
{
	return mkvalstr(mkstr0(s));
}

Val
cqctcstrnval(char *s, uint64_t len)
{
	return mkvalstr(mkstr(s, len));
}

Val
cqctcstrvalshared(char *s)
{
	return mkvalstr(mkstrk(s, strlen(s), Sperm));
}

Val
cqctcstrnvalshared(char *s, uint64_t len)
{
	return mkvalstr(mkstrk(s, len, Sperm));
}

Val
cqctint8val(int8_t x)
{
	return mkvallitcval(clp64le.xint8, x);
}

Val
cqctint16val(int16_t x)
{
	return mkvallitcval(clp64le.xint16, x);
}

Val
cqctint32val(int32_t x)
{
	return mkvallitcval(clp64le.xint32, x);
}

Val
cqctint64val(int64_t x)
{
	return mkvallitcval(clp64le.xint64, x);
}

Val
cqctuint8val(uint8_t x)
{
	return mkvallitcval(clp64le.xuint8, x);
}

Val
cqctuint16val(uint16_t x)
{
	return mkvallitcval(clp64le.xuint16, x);
}

Val
cqctuint32val(uint32_t x)
{
	return mkvallitcval(clp64le.xuint32, x);
}

Val
cqctuint64val(uint64_t x)
{
	return mkvallitcval(clp64le.xuint64, x);
}

void
cqctfreecstr(char *s)
{
	efree(s);
}

void
cqctenvbind(Toplevel *top, char *name, Val v)
{
	Val *vp;
	vp = envget(top->env, name);
	if(vp)
		gcwb(*vp);
	envbind(top->env, name, v);
}

Val
cqctenvlook(Toplevel *top, char *name)
{
	Val *rv;
	rv = envget(top->env, name);
	if(rv)
		return *rv;
	return 0;
}

uint64_t
cqctlength(Val v)
{
	List *lst;
	Vec *vec;
	Str *str;
	Tab *tab;

	switch(Vkind(v)){
	default:
		return (uint64_t)-1;
	case Qlist:
		lst = vallist(v);
		return listxlen(lst->x);
	case Qstr:
		str = valstr(v);
		return str->len;
	case Qvec:
		vec = valvec(v);
		return vec->len;
	case Qtab:
		tab = valtab(v);
		return tab->cnt;
	}
}

Val*
cqctlistvals(Val v)
{
	List *lst;
	Listx *x;
	if(Vkind(v) != Qlist)
		return 0;
	lst = vallist(v);
	x = lst->x;
	return &x->val[x->hd];
}

Val*
cqctvecvals(Val v)
{
	Vec *vec;
	if(Vkind(v) != Qvec)
		return 0;
	vec = valvec(v);
	return vec->vec;
}
