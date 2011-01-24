#include "sys.h"
#include "util.h"
#include "syscqct.h"

Val*
iterlist(Head *hd, Ictx *ictx)
{
	List *lst;
	Listx *x;
	lst = (List*)hd;
	if(ictx->n == 0)
		ictx->x = lst->x;
	x = ictx->x;
	if(ictx->n >= x->sz)
		return GCiterdone;
	// x->val may change from call to call because of slide,
	// but will always point to buffer of markable Vals
	return &x->val[ictx->n++];
}

static void
freelistx(Listx *x)
{
	efree(x->val);
	efree(x->oval);
	efree(x);
}

int
freelist(Head *hd)
{
	List *lst;
	lst = (List*)hd;
	freelistx(lst->x);
	return 1;
}

u32
listxlen(Listx *x)
{
	return x->tl-x->hd;
}

static Listx*
mklistx(u32 sz)
{
	Listx *x;
	x = emalloc(sizeof(Listx));
	x->hd = x->tl = sz/2;
	x->sz = sz;
	x->val = emalloc(x->sz*sizeof(Val)); /* must be 0 or Xundef */
	return x;
}

static List*
_mklist(Listx *x)
{
	List *lst;
	lst = (List*)malq(Qlist);
	lst->x = x;
	quard((Val)lst);
	return lst;
}

List*
mklistn(u32 sz)
{
	return _mklist(mklistx(sz));
}

List*
mklist(void)
{
	return mklistn(Listinitsize);
}

static List*
mklistinit(Imm len, Val v)
{
	List *l;
	Imm m;

	l = mklistn(len);
	for(m = 0; m < len; m++)
		_listappend(l, v);
	return l;
}

Val
listref(VM *vm, List *lst, Imm idx)
{
	Listx *x;
	x = lst->x;
	if(idx >= listxlen(x))
		vmerr(vm, "listref out of bounds");
	return x->val[x->hd+idx];
}

List*
listset(VM *vm, List *lst, Imm idx, Val v)
{
	Listx *x;
	x = lst->x;
	if(idx >= listxlen(x))
		vmerr(vm, "listset out of bounds");
	gcwb(mkvallist(lst));
	x->val[x->hd+idx] = v;
	return lst;
}

List*
listcopy(List *lst)
{
	List *new;
	u32 hd, tl, len;
	new = mklistn(lst->x->sz);
	new->x->hd = hd = lst->x->hd;
	new->x->tl = tl = lst->x->tl;
	len = tl-hd;
	memcpy(&new->x->val[hd], &lst->x->val[hd], len*sizeof(Val));
	return new;
}

void
listcopyv(List *lst, Imm ndx, Imm n, Val *v)
{
	Listx *x;
	x = lst->x;
	memcpy(v, x->val+x->hd+ndx, n*sizeof(Val));
}

static List*
listreverse(List *lst)
{
	List *new;
	u32 hd, tl, len, m;
	new = mklistn(lst->x->sz);
	new->x->hd = hd = lst->x->hd;
	new->x->tl = tl = lst->x->tl;
	len = tl-hd;
	for(m = 0; m < len; m++)
		new->x->val[hd++] = lst->x->val[--tl];
	return new;
}

static Val
listhead(VM *vm, List *lst)
{
	Listx *x;
	x = lst->x;
	if(listxlen(x) == 0)
		vmerr(vm, "head on empty list");
	return x->val[x->hd];
}

void
listpop(List *lst, Val *vp)
{
	Listx *x;
	x = lst->x;
	if(listxlen(x) == 0)
		return; 	/* nil */
	*vp = x->val[x->hd];
	x->val[x->hd] = Xundef;
	x->hd++;
}

static List*
listtail(VM *vm, List *lst)
{
	List *new;
	if(listxlen(lst->x) == 0)
		vmerr(vm, "tail on empty list");
	new = listcopy(lst);
	new->x->hd++;
	return new;
}

u32
hashlist(Val v)
{
	List *l;
	Listx *x;
	u32 i, len, m;
	l = vallist(v);
	x = l->x;
	m = Vkind(v);
	len = listxlen(x);
	for(i = 0; i < len; i++)
		m ^= hashval(x->val[x->hd+i]);
	return m;
}

int
equallist(List *a, List *b)
{
	Listx *xa, *xb;
	u32 len, m;
	xa = a->x;
	xb = b->x;
	len = listxlen(xa);
	if(len != listxlen(xb))
		return 0;
	for(m = 0; m < len; m++)
		if(!eqval(xa->val[xa->hd+m], xb->val[xb->hd+m]))
			return 0;
	return 1;
}

static void
listexpand(List *lst)
{
	Listx *x, *nx;
	u32 len, newsz;

	x = lst->x;
	if(x->sz)
		newsz = x->sz*2;
	else
		newsz = 1;
	nx = mklistx(newsz);
	len = listxlen(x);
	if(x->hd == 0){
		/* expanding to the left */
		nx->hd = x->sz;
		nx->tl = x->tl+x->sz;
	}else{
		/* expanding to the right */
		nx->hd = x->hd;
		nx->tl = x->tl;
	}
	memcpy(&nx->val[nx->hd], &x->val[x->hd], len*sizeof(Val));
	_mklist(x);		/* preserve potential concurrent iterlist's
				   view; same strategy as tabexpand. */
	lst->x = nx;
}

static Listx*
maybelistexpand(List *lst)
{
	Listx *x;
again:
	x = lst->x;
	if(x->hd == 0 || x->tl == x->sz){
		listexpand(lst);
		goto again;	/* at most twice iff full on both ends */
	}
	return lst->x;
}

/* maybe listdel and listins should slide whichever half is shorter */

enum {
	SlideIns,
	SlideDel,
};

/* if op==SlideIns, expect room to slide by 1 in either direction;
   however, currently we always slide to right */
static void
slide(Listx *x, u32 idx, int op)
{
	Val *t;
	u32 m;
	m = listxlen(x)-idx;
	if(m <= 1 && op == SlideDel)
		/* deleting last element; nothing to do */
		return;
	if(x->oval == 0)
		x->oval = emalloc(x->sz*sizeof(Val));
	memcpy(&x->oval[x->hd], &x->val[x->hd], idx*sizeof(Val));
	switch(op){
	case SlideIns:
		memcpy(&x->oval[x->hd+idx+1], &x->val[x->hd+idx],
		       m*sizeof(Val));
		break;
	case SlideDel:
		memcpy(&x->oval[x->hd+idx], &x->val[x->hd+idx+1],
		       (m-1)*sizeof(Val));
		break;
	}
	t = x->val;
	x->val = x->oval;
	x->oval = t;
}

List*
listdel(VM *vm, List *lst, Imm idx)
{
	Listx *x;
	u32 m, len;
	x = lst->x;
	len = listxlen(x);
	if(idx >= len)
		vmerr(vm, "listdel out of bounds");
	m = len-idx;
	slide(x, idx, SlideDel);
	x->tl--;
	return lst;
}

List*
listins(VM *vm, List *lst, Imm idx, Val v)
{
	Listx *x;
	u32 m, len;
	x = lst->x;
	len = listxlen(x);
	if(idx > len)
		vmerr(vm, "listins out of bounds");
	x = maybelistexpand(lst);
	gcwb(mkvallist(lst));
	if(idx == 0)
		x->val[--x->hd] = v;
	else if(idx == len)
		x->val[x->tl++] = v;
	else{
		m = len-idx;
		slide(x, idx, SlideIns);
		x->tl++;
		x->val[x->hd+idx] = v;
	}
	return lst;
}

void
_listappend(List *lst, Val v)
{
	Listx *x;
	x = maybelistexpand(lst);
	gcwb(mkvallist(lst));
	x->val[x->tl++] = v;
}

static List*
listpush(VM *vm, List *lst, Val v)
{
	return listins(vm, lst, 0, v);
}

List*
listappend(VM *vm, List *lst, Val v)
{
	return listins(vm, lst, listxlen(lst->x), v);
}

List*
listconcat(VM *vm, List *l1, List *l2)
{
	List *rv;
	u32 m, len1, len2;

	len1 = listxlen(l1->x);
	len2 = listxlen(l2->x);
	rv = mklistn(len1+len2);
	for(m = 0; m < len1; m++)
		listappend(vm, rv, listref(vm, l1, m));
	for(m = 0; m < len2; m++)
		listappend(vm, rv, listref(vm, l2, m));
	return rv;
}

static List*
listslice(VM *vm, List *l, Imm b, Imm e)
{
	List *rv;
	Imm m;

	rv = mklistn(e-b);
	for(m = b; m < e; m++)
		listappend(vm, rv, listref(vm, l, m));
	return rv;
}

static void
l1_list(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Imm i;

	l = mklist();
	for(i = 0; i < argc; i++)
		listappend(vm, l, argv[i]);
	*rv = mkvallist(l);
}

static void
l1_mklist(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *cv;
	Val v;

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to mklist");
	checkarg(vm, "mklist", argv, 0, Qcval);
	cv = valcval(argv[0]);
	if(argc == 2)
		v = argv[1];
	else
		v = Xnil;
	if(!isnatcval(cv))
		vmerr(vm, "operand 1 to mklist must be "
		      "a non-negative integer");
	*rv = mkvallist(mklistinit(cv->val, v));
}

void
l1_listref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val vp;
	List *lst;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to listref");
	checkarg(vm, "listref", argv, 0, Qlist);
	checkarg(vm, "listref", argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listref must be "
		      "a non-negative integer");
	lst = vallist(argv[0]);
	vp = listref(vm, lst, cv->val);
	*rv = vp;
}

static void
l1_listdel(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to listdel");
	checkarg(vm, "listdel", argv, 0, Qlist);
	checkarg(vm, "listdel", argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listdel must be "
		      "a non-negative integer");
	lst = listdel(vm, vallist(argv[0]), cv->val);
	*rv = mkvallist(lst);
}

void
l1_listset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to listset");
	checkarg(vm, "listset", argv, 0, Qlist);
	checkarg(vm, "listset", argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listset must be "
		      "a non-negative integer");
	lst = vallist(argv[0]);
	lst = listset(vm, lst, cv->val, argv[2]);
	*rv = mkvallist(lst);
}

static void
l1_listins(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to listins");
	checkarg(vm, "listins", argv, 0, Qlist);
	checkarg(vm, "listins", argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listins must be "
		      "a non-negative integer");
	lst = listins(vm, vallist(argv[0]), cv->val, argv[2]);
	*rv = mkvallist(lst);
}

static void
l1_slice(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *l;
	Cval *b, *e;
	u32 len;

	if(argc != 3)
		vmerr(vm, "wrong number of arguments to slice");
	checkarg(vm, "slice", argv, 0, Qlist);
	checkarg(vm, "slice", argv, 1, Qcval);
	checkarg(vm, "slice", argv, 2, Qcval);
	l = vallist(argv[0]);
	b = valcval(argv[1]);
	e = valcval(argv[2]);
	len = listxlen(l->x);
	if(b->val > len)
		vmerr(vm, "slice out of bounds");
	if(e->val > len)
		vmerr(vm, "slice out of bounds");
	if(b->val > e->val)
		vmerr(vm, "slice out of bounds");
	*rv = mkvallist(listslice(vm, l, b->val, e->val));
}

static void
l1_head(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val vp;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to head");
	checkarg(vm, "head", argv, 0, Qlist);
	vp = listhead(vm, vallist(argv[0]));
	*rv = vp;
}

static void
l1_tail(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tail");
	checkarg(vm, "tail", argv, 0, Qlist);
	lst = listtail(vm, vallist(argv[0]));
	*rv = mkvallist(lst);
}

static void
l1_push(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to push");
	checkarg(vm, "push", argv, 0, Qlist);
	lst = listpush(vm, vallist(argv[0]), argv[1]);
	*rv = mkvallist(lst);
}

static void
l1_append(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to append");
	checkarg(vm, "append", argv, 0, Qlist);
	lst = listappend(vm, vallist(argv[0]), argv[1]);
	*rv = mkvallist(lst);
}

static void
l1_reverse(VM *vm, Imm argc, Val *argv, Val *rv)
{
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to reverse");
	if(Vkind(argv[0]) == Qlist)
		*rv = mkvallist(listreverse(vallist(argv[0])));
	else
		vmerr(vm, "operand 1 to reverse must be a list");
}

void
fnlist(Env *env)
{
	FN(append);
	FN(head);
	FN(list);
	FN(listdel);
	FN(listins);
	FN(listref);
	FN(listset);
	FN(mklist);
	FN(push);
	FN(reverse);
	FN(slice);
	FN(tail);
}
