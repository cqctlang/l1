#include "sys.h"
#include "util.h"
#include "syscqct.h"

Imm
listlen(List *x)
{
	return x->t-x->h;
}

static List*
_mklist(Imm cap)
{
	List *l;
	l = (List*)malq(Qlist, sizeof(List));
	l->v = mkvec(cap);
	l->h = l->t = cap/2;
	return l;
}

List*
mklistn(Imm cap)
{
	return _mklist(cap);
}

List*
mklist(void)
{
	return mklistn(Listinitsize);
}

List*
mklistinit(Imm len, Val v)
{
	List *l;
	Imm m;

	l = mklistn(len);
	for(m = 0; m < len; m++)
		_listappend(l, v);
	return l;
}

/* FIXME: maybe just call vec* routines instead? */
Val
listref(List *l, Imm idx)
{
	if(idx >= listlen(l))
		bug(); /* caller should check */
	return listdata(l)[l->h+idx];
}

List*
listset(List *l, Imm idx, Val v)
{
	if(idx >= listlen(l))
		bug(); /* caller should check */
	gcwb((Val)l->v);
	listdata(l)[l->h+idx] = v;
	return l;
}

List*
listcopy(List *l)
{
	List *n;
	n = mklistn(listcap(l));
	n->h = l->h;
	n->t = l->t;
	memcpy(&listdata(n)[n->h], &listdata(l)[l->h], (l->t-l->h)*sizeof(Val));
	return n;
}

static List*
listreverse(List *l)
{
	List *n;
	Imm h, t, len, m;
	n = mklistn(listcap(l));
	n->h = h = l->h;
	n->t = t = l->t;
	len = t-h;
	for(m = 0; m < len; m++)
		listdata(n)[h++] = listdata(l)[--t];
	return n;
}

static Val
listhead(VM *vm, List *l)
{
	if(listlen(l) == 0)
		vmerr(vm, "head on empty list");
	return listdata(l)[l->h];
}

void
listpop(List *l, Val *vp)
{
	if(listlen(l) == 0)
		return; 	/* nil */
	*vp = listdata(l)[l->h];
	listdata(l)[l->h] = Xnil;
	l->h++;
}

static List*
listtail(VM *vm, List *l)
{
	List *n;
	if(listlen(l) == 0)
		vmerr(vm, "tail on empty list");
	n = listcopy(l);
	listdata(n)[n->h] = Xnil;
	n->h++;
	return n;
}

u32
hashlist(List *l)
{
	u32 m;
	Imm i, len;
	m = Qlist;
	len = listlen(l);
	for(i = 0; i < len; i++)
		m = hashx(m, hashval(listdata(l)[l->h+i]));
	return m;
}

int
equallist(List *a, List *b)
{
	Imm len, m;
	len = listlen(a);
	if(len != listlen(b))
		return 0;
	for(m = 0; m < len; m++)
		if(!equalval(listdata(a)[a->h+m], listdata(b)[b->h+m]))
			return 0;
	return 1;
}

static void
listexpand(List *l)
{
	Vec *nv;
	Imm len, cap, nh, nt;

	if(listcap(l))
		cap = listcap(l)*2;
	else
		cap = 1;
	nv = mkvec(cap);
	len = listlen(l);
	if(l->h == 0){
		/* expanding to the left */
		nh = listcap(l);
		nt = l->t+listcap(l);
	}else{
		/* expanding to the right */
		nh = l->h;
		nt = l->t;
	}

	memcpy(&vecdata(nv)[nh], &listdata(l)[l->h], len*sizeof(Val));
	gcwb((Val)l);
	l->v = nv;
	l->h = nh;
	l->t = nt;
}

static void
maybeexpand(List *l)
{
again:
	if(l->h == 0 || l->t == listcap(l)){
		listexpand(l);
		goto again; /* once more if original was full on both ends */
	}
}

/* maybe listdel and listins should slide whichever half is shorter */

enum {
	SlideIns,
	SlideDel,
};

/* if op==SlideIns, expect room to slide by 1 in either direction;
   however, currently we always slide to right */
static void
slide(List *l, Imm idx, int op)
{
	Imm m;
	m = listlen(l)-idx;
	if(m <= 1 && op == SlideDel)
		/* deleting last element; nothing to do */
		return;
	switch(op){
	case SlideIns:
		memmove(&listdata(l)[l->h+idx+1],
			&listdata(l)[l->h+idx],
			m*sizeof(Val));
		break;
	case SlideDel:
		memmove(&listdata(l)[l->h+idx],
			&listdata(l)[l->h+idx+1],
			(m-1)*sizeof(Val));
		break;
	}
}

List*
listdel(VM *vm, List *l, Imm idx)
{
	Imm len;
	len = listlen(l);
	if(idx >= len)
		vmerr(vm, "listdel out of bounds");
	slide(l, idx, SlideDel);
	l->t--;
	return l;
}

List*
listins(VM *vm, List *l, Imm idx, Val v)
{
	Imm len;
	len = listlen(l);
	if(idx > len)
		vmerr(vm, "listins out of bounds");
	maybeexpand(l);
	gcwb((Val)l->v);
	if(idx == 0)
		listdata(l)[--l->h] = v;
	else if(idx == len)
		listdata(l)[l->t++] = v;
	else{
		slide(l, idx, SlideIns);
		l->t++;
		listdata(l)[l->h+idx] = v;
	}
	return l;
}

void
_listappend(List *l, Val v)
{
	maybeexpand(l);
	gcwb((Val)l->v);
	listdata(l)[l->t++] = v;
}

static List*
listpush(VM *vm, List *l, Val v)
{
	return listins(vm, l, 0, v);
}

List*
listappend(VM *vm, List *l, Val v)
{
	return listins(vm, l, listlen(l), v);
}

List*
listconcat(VM *vm, List *l1, List *l2)
{
	List *rv;
	Imm m, len1, len2;

	len1 = listlen(l1);
	len2 = listlen(l2);
	rv = mklistn(len1+len2);
	for(m = 0; m < len1; m++)
		listappend(vm, rv, listref(l1, m));
	for(m = 0; m < len2; m++)
		listappend(vm, rv, listref(l2, m));
	return rv;
}

static List*
listslice(VM *vm, List *l, Imm b, Imm e)
{
	List *rv;
	Imm m;

	rv = mklistn(e-b);
	for(m = b; m < e; m++)
		listappend(vm, rv, listref(l, m));
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

	if(argc > 2)
		vmerr(vm, "wrong number of arguments to mklist");
	if(argc > 0){
		checkarg(vm, argv, 0, Qcval);
		cv = valcval(argv[0]);
		if(!isnatcval(cv))
			vmerr(vm, "operand 1 to mklist must be "
			      "a non-negative integer");
	}else
		cv = cval0;
	if(argc == 2)
		v = argv[1];
	else
		v = Xnil;
	*rv = mkvallist(mklistinit(cvalu(cv), v));
}

void
l1_listref(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val vp;
	List *lst;
	Imm idx, m;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to listref");
	checkarg(vm, argv, 0, Qlist);
	checkarg(vm, argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listref must be "
		      "a non-negative integer");
	lst = vallist(argv[0]);
	m = listlen(lst);
	idx = cvalu(cv);
	if(idx >= m)
		vmerr(vm, "listref out of bounds");
	vp = listref(lst, idx);
	*rv = vp;
}

static void
l1_listdel(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to listdel");
	checkarg(vm, argv, 0, Qlist);
	checkarg(vm, argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listdel must be "
		      "a non-negative integer");
	lst = listdel(vm, vallist(argv[0]), cvalu(cv));
	*rv = mkvallist(lst);
}

void
l1_listset(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	Imm idx, m;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to listset");
	checkarg(vm, argv, 0, Qlist);
	checkarg(vm, argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listset must be "
		      "a non-negative integer");
	lst = vallist(argv[0]);
	m = listlen(lst);
	idx = cvalu(cv);
	if(idx > m)
		vmerr(vm, "listset out of bounds");
	if(idx == m)
		lst = listins(vm, lst, idx, argv[2]);
	else
		lst = listset(lst, idx, argv[2]);
	*rv = mkvallist(lst);
}

static void
l1_listins(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	Cval *cv;
	if(argc != 3)
		vmerr(vm, "wrong number of arguments to listins");
	checkarg(vm, argv, 0, Qlist);
	checkarg(vm, argv, 1, Qcval);
	cv = valcval(argv[1]);
	if(!isnatcval(cv))
		vmerr(vm, "operand 2 to listins must be "
		      "a non-negative integer");
	lst = listins(vm, vallist(argv[0]), cvalu(cv), argv[2]);
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
	checkarg(vm, argv, 0, Qlist);
	checkarg(vm, argv, 1, Qcval);
	checkarg(vm, argv, 2, Qcval);
	l = vallist(argv[0]);
	b = valcval(argv[1]);
	e = valcval(argv[2]);
	len = listlen(l);
	if(cvalu(b) > len)
		vmerr(vm, "slice out of bounds");
	if(cvalu(e) > len)
		vmerr(vm, "slice out of bounds");
	if(cvalu(b) > cvalu(e))
		vmerr(vm, "slice out of bounds");
	*rv = mkvallist(listslice(vm, l, cvalu(b), cvalu(e)));
}

static void
l1_head(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Val vp;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to head");
	checkarg(vm, argv, 0, Qlist);
	vp = listhead(vm, vallist(argv[0]));
	*rv = vp;
}

static void
l1_tail(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 1)
		vmerr(vm, "wrong number of arguments to tail");
	checkarg(vm, argv, 0, Qlist);
	lst = listtail(vm, vallist(argv[0]));
	*rv = mkvallist(lst);
}

static void
l1_push(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to push");
	checkarg(vm, argv, 0, Qlist);
	lst = listpush(vm, vallist(argv[0]), argv[1]);
	*rv = mkvallist(lst);
}

static void
l1_append(VM *vm, Imm argc, Val *argv, Val *rv)
{
	List *lst;
	if(argc != 2)
		vmerr(vm, "wrong number of arguments to append");
	checkarg(vm, argv, 0, Qlist);
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
fnlist(Env env)
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
