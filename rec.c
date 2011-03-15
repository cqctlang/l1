#include "sys.h"
#include "util.h"
#include "syscqct.h"

static Rec*
mkrec(Rd *rd)
{
	Imm m;
	Rec *r;
	r = (Rec*)malv(Qrec, recsize(rd->nf));
	r->rd = rd;
	r->nf = rd->nf;
	for(m = 0; m < r->nf; m++)
		recdata(r)[m] = Xnil;
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
		      (int)mn->len, strdata(mn));
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
		      (int)mn->len, strdata(mn));
	r = mkrec(rd);
	for(m = 0; m < argc; m++)
		recdata(r)[m] = argv[m];
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
		      (int)mn->len, strdata(mn));
	if(Vkind(argv[0]) != Qrec)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, strdata(mn),
		      (int)rd->name->len, strdata(rd->name));
	r = valrec(argv[0]);
	len = 1+rd->name->len+1+16+1+1;
	buf = emalloc(len);
	snprint(buf, len, "<%.*s %p>",
		(int)rd->name->len,
		strdata(rd->name), r);
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
		      (int)mn->len, strdata(mn));
	if(Vkind(argv[0]) != Qrec)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, strdata(mn),
		      (int)rd->name->len, strdata(rd->name));
	r = valrec(argv[0]);
	if(r->rd != rd)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, strdata(mn),
		      (int)rd->name->len, strdata(rd->name));

	/* weak test for compatible record descriptor */
	if(r->nf <= ndx->val)
		vmerr(vm, "attempt to call %.*s on incompatible %.*s record",
		      (int)mn->len, strdata(mn),
		      (int)rd->name->len, strdata(rd->name));

	*rv = recdata(r)[ndx->val];
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
		      (int)mn->len, strdata(mn));
	if(Vkind(argv[0]) != Qrec)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, strdata(mn),
		      (int)rd->name->len, strdata(rd->name));
	r = valrec(argv[0]);
	if(r->rd != rd)
		vmerr(vm, "operand 1 to %.*s must be a %.*s record",
		      (int)mn->len, strdata(mn),
		      (int)rd->name->len, strdata(rd->name));

	/* weak test for compatible record descriptor */
	if(r->nf <= ndx->val)
		vmerr(vm, "attempt to call %.*s on incompatible %.*s record",
		      (int)mn->len, strdata(mn),
		      (int)rd->name->len, strdata(rd->name));

	gcwb(mkvalrec(r));
	recdata(r)[ndx->val] = argv[1];
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

	rd = hgets(vm->top->env->rd, strdata(name), (unsigned)name->len);
	if(rd == 0){
		rd = (Rd*)malq(Qrd);
		hputs(vm->top->env->rd,
		      xstrndup(strdata(name), (unsigned)name->len),
		      (unsigned)name->len, rd);
	}else
		gcwb(mkvalrd(rd));

	rd->nf = listlen(fname);
	rd->name = name;
	rd->fname = fname;

	len = 3+name->len+1;
	buf = emalloc(len);

	snprint(buf, len, "is%.*s", (int)name->len, strdata(name));
	rd->is = mkccl(buf, recis, 2, mkvalrd(rd), mkvalstr(mkstr0(buf)));
	snprint(buf, len, "%.*s", (int)name->len, strdata(name));
	rd->mk = mkccl(buf, recmk, 2, mkvalrd(rd), mkvalstr(mkstr0(buf)));
	if(fmt == 0){
		snprint(buf, len, "fmt%.*s", (int)name->len, strdata(name));
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
			(int)name->len, strdata(name),
			(int)f->len, strdata(f));
		mn = mkvalstr(mkstr0(buf));
		cl = mkccl(buf, recget, 3,
			   mkvalrd(rd), mn, mkvallitcval(Vuint, n));
		tabput(rd->get, mkvalstr(f), mkvalcl(cl));

		/* set method */
		snprint(buf, len, "%.*sset%.*s",
			(int)name->len, strdata(name),
			(int)f->len, strdata(f));
		mn = mkvalstr(mkstr0(buf));
		cl = mkccl(buf, recset, 3,
			   mkvalrd(rd), mn, mkvallitcval(Vuint, n));
		tabput(rd->set, mkvalstr(f), mkvalcl(cl));

		efree(buf);
	}
	return rd;
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
		checkarg(vm, "mkrd", argv, 2, Qcl);
		fmt = valcl(argv[2]);
	}
	lst = vallist(argv[1]);
	nf = listlen(lst);
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

void
fnrec(Env *env)
{
	FN(mkrd);
	FN(rdfields);
	FN(rdfmt);
	FN(rdgettab);
	FN(rdis);
	FN(rdmk);
	FN(rdname);
	FN(rdof);
	FN(rdsetfmt);
	FN(rdsettab);
}
