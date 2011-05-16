#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Tab*
_mktab(u32 sz)
{
	Tab *t;
	u32 i;

	t = (Tab*)malq(Qtab);
	t->sz = sz;
	t->nent = 0;
	t->ht = mkvec(sz);
	for(i = 0; i < sz; i++)
		_vecset(t->ht, i, mkvalcval(0, 0, i));
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

Val
linkkey(Pair *lnk)
{
	return caar(lnk);
}

Val
linkval(Pair *lnk)
{
	return cdar(lnk);
}

Val
linknext(Pair *lnk)
{
	return cdr(lnk);
}

int
islink(Val v)
{
	return Vkind(v) == Qpair;
}


static Imm
linkidx(Pair *lnk)
{
	Cval *cv;
	while(islink(linknext(lnk)))
		lnk = (Pair*)linknext(lnk);
	cv = valcval(linknext(lnk));
	return cv->val;
}

static void
dellink(Tab *t, Pair *lnk)
{
	Val x;
	Pair *c;
	u32 h;

	h = linkidx(lnk);
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

static void
put(Tab *t, Pair *lnk)
{
	u32 h;
	Val k;
	k = linkkey(lnk);
	h = hashval(k);
	h &= t->sz-1;
	setlinknext(lnk, vecref(t->ht, h));
	vecset(t->ht, h, mkvalpair(lnk));
}

static Pair*
getrehash(Tab *t, Val k)
{
	Pair *lnk;
	Qkind kind;
	kind = Vkind(k);
	while(1){
		lnk = (Pair*)pop1tguard(t->tg);
		if(lnk == 0)
			return 0;
		if(linknext(lnk) == 0)
			/* was deleted */
			continue;
		dellink(t, lnk);
		put(t, lnk);
		if(Vkind(linkkey(lnk)) == kind && eqval(linkkey(lnk), k))
			return lnk;
	}
}

static Pair*
get(Tab *t, Val k)
{
	Val x;
	Pair *lnk;
	Qkind kind;
	u32 h;

	kind = Vkind(k);
	h = hashval(k);
	x = vecref(t->ht, h&(t->sz-1));
	while(islink(x)){
		lnk = (Pair*)x;
		x = linknext(lnk);
		if(Vkind(linkkey(lnk)) == kind && eqval(linkkey(lnk), k))
			return lnk;
	}
	return getrehash(t, k);
}

Val
tabget(Tab *t, Val k)
{
	Pair *lnk;
	lnk = get(t, k);
	if(lnk)
		return linkval(lnk);
	return 0;
}

static void
expand(Tab *t)
{
	Vec *oht;
	Val x;
	Pair *lnk;
	u32 i, m, osz;

	osz = t->sz;
	oht = t->ht;
	gcwb(mkvaltab(t));
	t->sz *= 2;
	t->ht = mkvec(t->sz);
	for(i = 0; i < t->sz; i++)
		_vecset(t->ht, i, mkvalcval(0, 0, i));
	for(i = m = 0; i < osz && m < t->nent; i++){
		x = vecref(oht, i);
		while(islink(x)){
			lnk = (Pair*)x;
			x = linknext(lnk);
			put(t, lnk);
			m++;
		}
	}
}

void
tabput(Tab *t, Val k, Val v)
{
	Pair *lnk;

	lnk = get(t, k);
	if(lnk){
		setlinkval(lnk, v);
		return;
	}

	if(3*t->nent > 2*t->sz)
		expand(t);

	lnk = mklink(k, v);
	tguard(mkvalpair(lnk), t->tg);
	put(t, lnk);
	t->nent++;
}

void
tabdel(Tab *t, Val k)
{
	Pair *lnk;
	lnk = get(t, k);
	if(lnk){
		dellink(t, lnk);
		setlinknext(lnk, 0);
		t->nent--;
	}
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
		while(islink(x)){
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
		while(islink(x)){
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
		while(islink(x)){
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
		if(islink(x))
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
		while(islink(x)){
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
