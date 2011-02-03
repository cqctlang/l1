#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Tab*
_mktab(u32 sz)
{
	Tab *t;

	t = (Tab*)malq(Qtab);
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
	Val x, p, q;
	Qkind kind;
	Hashop *op;
	u32 h;

	kind = Vkind(k);
	op = &hashop[kind];
	h = op->hash(k);
	x = vecref(t->ht, h&(t->sz-1));
	q = 0;
	while(x != Xnil){
		p = car(x);
		if(Vkind(car(p)) == kind && op->eq(car(p), k)){
			if(q)
				setcdr(q, cdr(x));
			else
				vecset(t->ht, h&(t->sz-1), cdr(x));
			t->nent--;
			return;
		}
		q = x;
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
	vecset(t->ht, i, cdr(x));
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
