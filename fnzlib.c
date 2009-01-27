#include "sys.h"
#include "util.h"
#include "syscqct.h"
#include <zlib.h>

static Vec*
cvinflate(VM *vm, Cval *p, Cval *omaxcv)
{
	Vec *v;
	Range *r;
	Imm n, ilim, ioff, iused, iavail;
	Imm oavail, omax, ooff, olen;
	Str *is;
	unsigned char *out;
	z_stream z;
	int rv;
	static unsigned long unit = 4096;
	Vec *rvec;

	v = callmap(vm, p->dom->as);
	r = mapstab(vm, v, p->val, 0);	/* FIXME: type sanity */
	if(r == 0)
		vmerr(vm, "out-of-bounds address space access");

	ilim = r->beg->val+r->len->val-p->val;
	ioff = p->val;
	iused = 0;

	if(omaxcv){
		omax = omaxcv->val;
		out = emalloc(omax);
		olen = omax;
	}else{
		out = emalloc(unit);
		olen = unit;
	}
	ooff = 0;
	oavail = olen;

	memset(&z, 0, sizeof(z));
	z.next_out = out;
	z.avail_out = oavail;
	rv = inflateInit(&z);

	if(rv != Z_OK)
		goto badrv;
	while(ilim > 0 && (!omaxcv || ooff < omax)){
		if(z.avail_in == 0){
			n = MIN(ilim, unit);
			is = callget(vm, p->dom->as, ioff, n);
			ilim -= is->len;
			ioff += is->len;
			z.next_in = (unsigned char*)is->s;
			z.avail_in = is->len;
		}
		if(z.avail_out == 0){
			if(omaxcv)
				vmerr(vm, "incomplete inflate");
			n = MIN(omax-ooff, unit);
			out = erealloc(out, olen, olen+n);
			olen += n;
			oavail += n;
			z.next_out = out+ooff;
			z.avail_out = oavail;
		}
		iavail = z.avail_in;
		rv = inflate(&z, Z_SYNC_FLUSH);
		if(rv != Z_STREAM_END && rv != Z_OK)
			goto badrv;
		iused += iavail-z.avail_in;
		ooff += oavail-z.avail_out;
		oavail = z.avail_out;
		if(rv == Z_STREAM_END)
			break;
	}
	rv = inflateEnd(&z);
	if(rv != Z_OK)
		goto badrv;
	rvec = mkvec(2);
	_vecset(rvec, 0, mkvalstr(mkstrk((char*)out, ooff, Smalloc)));
	_vecset(rvec, 1, mkvallitcval(Vulong, iused));
	return rvec;

	badrv:
	switch(rv){
	case Z_DATA_ERROR:
		vmerr(vm, "deflated input is corrupt near offset %"
		      PRIu64, ioff);
	case Z_NEED_DICT:
		vmerr(vm, "need dictionary");
	case Z_STREAM_ERROR:
	case Z_BUF_ERROR:
		vmerr(vm, "inflate implementation bug near input "
		      "offset %" PRIu64, ioff);
	case Z_MEM_ERROR:
		vmerr(vm, "inflate exhausted memory near input "
		      "offset %" PRIu64, ioff);
	default:
		vmerr(vm, "inflate return unknown error %d near input "
		      "offset %" PRIu64, rv, ioff);
	}
}

static void
l1_inflate(VM *vm, Imm argc, Val *argv, Val *rv)
{
	Cval *p, *omax;
	Str *ss;
	Vec *rvec;
	static char *err =
		"operand 1 to inflate must be a string or a"
		"char* or unsigned char* pointer";

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
		checkarg(vm, "zinflate", argv, 1, Qcval);
		omax = valcval(argv[1]);
	}
	if(p)
		rvec = cvinflate(vm, p, omax);
	else
		vmerr(vm, "implemented inflate mode");
	*rv = mkvalvec(rvec);
}

void
fnzlib(Env *env)
{
	FN(inflate);
}
