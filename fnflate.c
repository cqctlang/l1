#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include "x/include/lib9.h"
#include "x/include/flate.h"

struct getr {
	VM *vm;
	Cval *p;
	unsigned char *buf;
	Imm lim;
	Imm off;
	Imm avail;
	Imm used;
};

struct wr {
	VM *vm;
	char *out;
	Imm omax;
	Imm len;
	Imm avail;
};

enum {
	Unit = 4096
};

static int
rfn(void *p)
{
	struct getr *getr;
	int rv;
	Imm n;
	Str *is;

	getr = p;
	if(getr->lim == 0)
		return -1;
	if(getr->avail == 0){
		n = MIN(getr->lim, Unit);
		is = callget(getr->vm, getr->p->dom->as, getr->off, n);
		if(is->len == 0)
			vmerr(getr->vm, "inflate: not enough input"); 
		getr->off += is->len;
		getr->avail = is->len;
		getr->buf = (unsigned char*)is->s; /* safe ref until next get */
	}
	rv = *getr->buf;
	getr->buf++;
	getr->lim--;
	getr->avail--;
	getr->used++;
	return rv;
}

static int
wfn(void *p, void *buf, int cnt)
{
	struct wr *wr;
	Imm m;

	wr = p;
	if(wr->omax && wr->len+cnt >= wr->omax)
		return 0;
	if(wr->avail < cnt){
		if(wr->omax)
			vmerr(wr->vm, "incomplete inflate");
		m = 2*(wr->len+cnt);
		wr->out = erealloc(wr->out, wr->len, m);
		wr->avail = m-wr->len;
	}
	memcpy(wr->out+wr->len, buf, cnt);
	wr->len += cnt;
	wr->avail -= cnt;
	return cnt;
}

static Vec*
cvinflate(VM *vm, Cval *p, Cval *omaxcv)
{
	Vec *v;
	Range *r;
	struct getr getr;
	struct wr wr;
	int err;
	Vec *rvec;

	v = callmap(vm, p->dom->as);
	r = mapstab(vm, v, p->val, 0);	/* FIXME: type sanity */
	if(r == 0)
		vmerr(vm, "out-of-bounds address space access");

	memset(&getr, 0, sizeof(getr));
	getr.vm = vm;
	getr.p = p;
	getr.off = p->val;
	getr.lim = r->beg->val+r->len->val-p->val;

	memset(&wr, 0, sizeof(wr));
	wr.vm = vm;
	if(omaxcv){
		wr.omax = omaxcv->val;
		wr.out = emalloc(wr.omax);
		wr.avail = wr.omax;
	}else{
		wr.omax = 0;
		wr.out = emalloc(Unit);
		wr.avail = Unit;
	}

	err = inflatezlib(&wr, wfn, &getr, rfn);
	if(err < 0)
		vmerr(vm, flateerr(err));

	rvec = mkvec(2);
	_vecset(rvec, 0, mkvalstr(mkstrk((char*)wr.out, wr.len, Smalloc)));
	_vecset(rvec, 1, mkvallitcval(Vulong, getr.used));
	return rvec;
}

static void
l1_inflate(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *p, *omax;
	Str *ss;
	Vec *rvec;
	static char *err =
		"operand 1 to inflate must be a string or a"
		"char* or unsigned char*";

	if(argc != 1 && argc != 2)
		vmerr(vm, "wrong number of arguments to inflate");

	p = 0;
	ss = 0;
	omax = 0;
	if(argv[0]->qkind == Qcval){
		p = valcval(argv[0]);
		if(!isstrcval(p))
			vmerr(vm, err);
	}else if(argv[0]->qkind == Qstr){
		ss = valstr(argv[0]);
	}else
		vmerr(vm, err);
	if(argc == 2){
		checkarg(vm, "inflate", argv, 1, Qcval);
		omax = valcval(argv[1]);
	}
	if(p)
		rvec = cvinflate(vm, p, omax);
	else
		vmerr(vm, "implemented inflate mode");
	*rv = mkvalvec(rvec);
}

void
fnflate(Env *env)
{
	deflateinit();
	inflateinit();
	FN(inflate);
}