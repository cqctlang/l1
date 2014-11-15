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
	Str *out;
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
		/* safe ref until next get */
		getr->buf = (unsigned char*)strdata(is);
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
		wr->out = strrealloc(wr->out, m);
		wr->avail = m-wr->len;
	}
	memcpy(strdata(wr->out)+wr->len, buf, cnt);
	wr->len += cnt;
	wr->avail -= cnt;
	return cnt;
}

static Vec*
doinflate(VM *vm, struct getr *getr, Cval *omaxcv, int zlib)
{
	struct wr wr;
	Vec *rvec;
	int err;

	memset(&wr, 0, sizeof(wr));
	wr.vm = vm;
	if(omaxcv){
		wr.omax = cvalu(omaxcv);
		wr.out = mkstrn(wr.omax);
		wr.avail = wr.omax;
	}else{
		wr.omax = 0;
		wr.out = mkstrn(Unit);
		wr.avail = Unit;
	}

	if(zlib)
		err = inflatezlib(&wr, wfn, getr, rfn);
	else
		err = xinflate(&wr, wfn, getr, rfn);
	if(err < 0)
		vmerr(vm, flateerr(err));

	rvec = mkvec(2);
	_vecset(rvec, 0, mkvalstr(strrealloc(wr.out, wr.len)));
	_vecset(rvec, 1, mkvallitcval(Vulong, getr->used));
	return rvec;

}

static Vec*
cvinflate(VM *vm, Cval *p, Cval *omaxcv, int zlib)
{
	Range *r;
	struct getr getr;
	Vec *v;

	/* effectively r = unit(vm, p->dom->as, p->val) */
	v = callmap(vm, p->dom->as);
	r = mapstab(vm, v, cvalu(p), 0);	/* FIXME: type sanity */
	if(r == 0)
		vmerr(vm, "out-of-bounds address space access");

	memset(&getr, 0, sizeof(getr));
	getr.vm = vm;
	getr.p = p;
	getr.off = cvalu(p);
	getr.lim = cvalu(r->beg)+cvalu(r->len)-cvalu(p);

	return doinflate(vm, &getr, omaxcv, zlib);
}

static Vec*
strinflate(VM *vm, Str *s, Cval *omaxcv, int zlib)
{
	struct getr getr;

	memset(&getr, 0, sizeof(getr));
	getr.vm = vm;
	getr.p = 0;
	getr.off = 0;
	getr.lim = getr.avail = s->len;
	getr.buf = (unsigned char*)strdata(s);

	return doinflate(vm, &getr, omaxcv, zlib);
}

static void
_inflate(VM *vm, Imm argc, Val *argv, Val *rv, int zlib)
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
	if(Vkind(argv[0]) == Qcval){
		p = valcval(argv[0]);
		if(!isstrcval(p))
			vmerr(vm, err);
	}else if(Vkind(argv[0]) == Qstr){
		ss = valstr(argv[0]);
	}else
		vmerr(vm, err);
	if(argc == 2){
		checkarg(vm, argv, 1, Qcval);
		omax = valcval(argv[1]);
	}
	if(p)
		rvec = cvinflate(vm, p, omax, zlib);
	else if(ss)
		rvec = strinflate(vm, ss, omax, zlib);
	else
		vmerr(vm, "unimplemented inflate mode");
	*rv = mkvalvec(rvec);
}

static void
l1_inflatezlib(VM *vm, Imm argc, Val *argv, Val *rv)
{
	_inflate(vm, argc, argv, rv, 1);
}

static void
l1_inflate(VM *vm, Imm argc, Val *argv, Val *rv)
{
	_inflate(vm, argc, argv, rv, 0);
}

void
fnflate(Env env)
{
	deflateinit();
	inflateinit();
	FN(inflate);
	FN(inflatezlib);
}
