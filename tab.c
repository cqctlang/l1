#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Tab*
_mktab(u32 sz)
{
	Tab *t;

	t = (Tab*)mal(Qtab);
	t->sz = sz;
	t->nent = 0;
	t->ht = mkvecinit(sz, Xnil);
	return t;
}

Tab*
mktab()
{
	return _mktab(Tabinitsize);
}

static Pair*
get(Tab *t, Val k, u32 *hp)
{
	Val x;
	Pair *p;
	Qkind kind;
	Hashop *op;
	u32 h;

	kind = Vkind(k);
	op = &hashop[kind];
	h = op->hash(k);
	*hp = h;
	x = vecref(t->ht, h&(t->sz-1));
	while(x != Xnil){
		p = (Pair*)car(x);
		x = cdr(x);
		if(Vkind(car(p)) == kind && op->eq(car(p), k))
			return p;
	}
	return 0;
}

Val
tabget(Tab *t, Val k)
{
	Pair *p;
	u32 h;
	p = get(t, k, &h);
	if(p)
		return cdr(p);
	return 0;
}

static void
put(Vec *ht, u32 h, Val x)
{
	vecset(ht, h, mkvalpair(cons(x, vecref(ht, h))));
}

static void
expand(Tab *t)
{
	Vec *nht;
	Val x;
	Pair *p;
	u32 i, m, h, nsz;

	nsz = t->sz*2;
	nht = mkvecinit(nsz, Xnil);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			p = (Pair*)car(x);
			x = cdr(x);
			h = hashop[Vkind(car(p))].hash(car(p));
			put(nht, h&(nsz-1), mkvalpair(p));
			m++;
		}
	}
	gcwb(mkvaltab(t));
	t->ht = nht;
	t->sz = nsz;
}

void
tabput(Tab *t, Val k, Val v)
{
	Pair *p;
	u32 h;
	
	p = get(t, k, &h);
	if(p){
		setcdr(p, v);
		return;
	}

	if(3*t->nent > 2*t->sz)
		expand(t);

	put(t->ht, h&(t->sz-1), mkvalpair(cons(k, v)));
	t->nent++;
}

void
tabdel(Tab *t, Val k)
{
	Val x;
	Pair *p, *q;
	Qkind kind;
	Hashop *op;
	u32 h;

	kind = Vkind(k);
	op = &hashop[kind];
	h = op->hash(k);
	x = vecref(t->ht, h&(t->sz-1));
	q = 0;
	while(x != Xnil){
		p = (Pair*)car(x);
		if(Vkind(car(p)) == kind && op->eq(car(p), k)){
			if(q)
				setcdr(q, cdr(p));
			else
				put(t->ht, h&(t->sz-1), cdr(p));
			return;
		}
		q = (Pair*)x;
		x = cdr(x);
	}
}

Vec*
tabenum(Tab *t)
{
	Vec *v;
	Val x;
	Pair *p;
	u32 i, m;

	v = mkvec(2*t->nent);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			p = (Pair*)car(x);
			x = cdr(x);
			_vecset(v, m, car(p));
			_vecset(v, m+t->nent, cdr(p));
			m++;
		}
	}
	return v;
}

Vec*
tabenumkeys(Tab *t)
{
	Vec *v;
	Val x;
	Pair *p;
	u32 i, m;

	v = mkvec(t->nent);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			p = (Pair*)car(x);
			x = cdr(x);
			_vecset(v, m, car(p));
			m++;
		}
	}
	return v;
}

Vec*
tabenumvals(Tab *t)
{
	Vec *v;
	Val x;
	Pair *p;
	u32 i, m;

	v = mkvec(t->nent);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			p = (Pair*)car(x);
			x = cdr(x);
			_vecset(v, m, cdr(p));
			m++;
		}
	}
	return v;
}

void
tabpop(Tab *t, Val *rv)
{
	Vec *v;
	Val x;
	Pair *p;
	u32 i;

	if(t->nent == 0)
		return;		/* nil */

	for(i = 0; i < t->sz; i++){
		x = vecref(t->ht, i);
		if(x != Xnil)
			break;
	}
	p = (Pair*)car(x);
	put(t->ht, i, cdr(x));
	t->nent--;
	v = mkvec(2);
	_vecset(v, 0, car(p));
	_vecset(v, 1, cdr(p));
	*rv = mkvalvec(v);
}

Tab*
tabcopy(Tab *t)
{
	Tab *rv;
	Val x;
	Pair *p;
	u32 i, m;

	rv = mktab();
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			p = (Pair*)car(x);
			x = cdr(x);
			tabput(rv, car(p), cdr(p));
			m++;
		}
	}
	return rv;
}


#if 0
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
	for(i = 0; i < x->sz && m < tab->nent; i++){
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
	nx->nxt = tab->nent;

	/* fresh garbage reference to pre-expanded state of table.
	   this preserves a reference to the pre-expand storage
	   (tab->x) in case gc (via itertab) is concurrently (i.e., in
	   this epoch) marking it.  we exploit the property that
	   objects always survive the epoch of their creation. */
//	x->sz = 0;		/* so freetab does not free nx's Tabkeys */
//	_mktab(x);
	freetabx(x);

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
	gcwb(mkvaltab(tab));
	tk = _tabget(tab, keyv, 0);
	if(tk){
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
	x->key[tk->idx] = Xundef;
	x->val[tk->idx] = Xundef;
	*ptk = tk->link;
	efree(tk);
	tab->cnt--;
}

/* create a new vector of len 2N, where N is number of elements in TAB.
   elements 0..N-1 are the keys of TAB; N..2N-1 are the associated vals. */
Vec*
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

Vec*
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

Vec*
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

void
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
	x->key[tk->idx] = Xundef;
	x->val[tk->idx] = Xundef;
	efree(tk);
	*rv = mkvalvec(vec);
}

Tab*
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

#endif

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

void
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

void
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




void
fntab(Env *env)
{
	FN(mktab);
	FN(tabdelete);
	FN(tabenum);
	FN(tabinsert);
	FN(tabkeys);
	FN(tablook);
	FN(tabvals);
}
