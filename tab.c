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
	t->tg = mkguard();
	return t;
}

Tab*
mktab()
{
	return _mktab(Tabinitsize);
}

static Pair*
mklink(Val k, Val v)
{
	return cons(cons(k, v), Xnil);
}

static void
setlinknext(Pair *lnk, Val v)
{
	setcdr(lnk, v);
}

static void
setlinkval(Pair *lnk, Val v)
{
	setcdr(car(lnk), v);
}

static Val
linkkey(Pair *lnk)
{
	return caar(lnk);
}

static Val
linkval(Pair *lnk)
{
	return cdar(lnk);
}

static Val
linknext(Pair *lnk)
{
	return cdr(lnk);
}

static Pair*
get(Tab *t, Val k, u32 *hp)
{
	Val x;
	Pair *lnk;
	Qkind kind;
	Hashop *op;
	u32 h;

	kind = Vkind(k);
	op = &hashop[kind];
	h = op->hash(k);
	*hp = h;
	x = vecref(t->ht, h&(t->sz-1));
	while(x != Xnil){
		lnk = (Pair*)x;
		x = linknext(lnk);
		if(Vkind(linkkey(lnk)) == kind && op->eq(linkkey(lnk), k))
			return lnk;
	}
	return 0;
}

Val
tabget(Tab *t, Val k)
{
	Pair *lnk;
	u32 h;
	lnk = get(t, k, &h);
	if(lnk)
		return linkval(lnk);
	return 0;
}

static void
put(Vec *ht, u32 h, Pair *lnk)
{
	vecset(ht, h, mkvalpair(lnk));
}

static void
expand(Tab *t)
{
	Vec *nht;
	Val x;
	Pair *lnk;
	u32 i, m, h, nsz;

	nsz = t->sz*2;
	nht = mkvecinit(nsz, Xnil);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			lnk = (Pair*)x;
			x = linknext(lnk);
			h = hashop[Vkind(linkkey(lnk))].hash(linkkey(lnk));
			put(nht, h&(nsz-1), lnk);
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
	Pair *lnk;
	u32 h;

	lnk = get(t, k, &h);
	if(lnk){
		setlinkval(lnk, v);
		return;
	}

	if(3*t->nent > 2*t->sz)
		expand(t);

	put(t->ht, h&(t->sz-1), mklink(k, v));
	t->nent++;
}

static void
dellink(Tab *t, Pair *lnk)
{
	Val x;
	Qkind kind;
	Hashop *op;
	Pair *c;
	u32 h;

	kind = Vkind(linkkey(lnk));
	op = &hashop[kind];
	h = op->hash(linkkey(lnk));
	x = vecref(t->ht, h&(t->sz-1));
	if(x == (Val)lnk)
		vecset(t->ht, h&(t->sz-1), linknext(lnk));
	else{
		c = (Pair*)x;
		while(lnk != (Pair*)linknext(c))
			c = (Pair*)linknext(c);
		setlinknext(c, linknext(lnk));
	}
}

void
tabdel(Tab *t, Val k)
{
	Pair *lnk;
	u32 h;

	lnk = get(t, k, &h);
	if(lnk)
		dellink(t, lnk);
}

Vec*
tabenum(Tab *t)
{
	Vec *v;
	Val x;
	Pair *lnk;
	u32 i, m;

	v = mkvec(2*t->nent);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			lnk = (Pair*)x;
			x = linknext(lnk);
			_vecset(v, m, linkkey(lnk));
			_vecset(v, m+t->nent, linkval(lnk));
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
	Pair *lnk;
	u32 i, m;

	v = mkvec(t->nent);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			lnk = (Pair*)x;
			x = linknext(lnk);
			_vecset(v, m, linkkey(lnk));
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
	Pair *lnk;
	u32 i, m;

	v = mkvec(t->nent);
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			lnk = (Pair*)x;
			x = linknext(lnk);
			_vecset(v, m, linkval(lnk));
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
	Pair *lnk;
	u32 i;

	if(t->nent == 0)
		return;		/* nil */

	for(i = 0; i < t->sz; i++){
		x = vecref(t->ht, i);
		if(x != Xnil)
			break;
	}
	lnk = (Pair*)x;
	vecset(t->ht, i, linknext(lnk));
	t->nent--;
	v = mkvec(2);
	_vecset(v, 0, linkkey(lnk));
	_vecset(v, 1, linkval(lnk));
	*rv = mkvalvec(v);
}

Tab*
tabcopy(Tab *t)
{
	Tab *rv;
	Val x;
	Pair *lnk;
	u32 i, m;

	rv = mktab();
	for(i = m = 0; i < t->sz && m < t->nent; i++){
		x = vecref(t->ht, i);
		while(x != Xnil){
			lnk = (Pair*)x;
			x = linknext(lnk);
			tabput(rv, linkkey(lnk), linkval(lnk));
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
