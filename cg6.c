#include "sys.h"
#include "util.h"
#include "syscqct.h"

enum
{
	MaxInsn6 = 15,
	RexW = 0x48,
};

typedef
enum Reg6
{
	Rrax = 0,
	Rrcx,
	Rrdx,
	Rrbx,
	Rrsp,
	Rrbp,
	Rrsi,
	Rrdi,
	Rr8,
	Rr9,
	Rr10,
	Rr11,
	Rr12,
	Rr13,
	Rr14,
	Rr15,
} Reg6;

typedef
struct i6
{
	unsigned char rex;
} i6;

static unsigned char*
nextx(Code *c)
{
	while(c->nx+MaxInsn6 >= c->maxx){
		xprintf("realloc\n");
		c->x = erealloc(c->x, c->maxx, 2*c->maxx);
		c->maxx *= 2;
	}
	xprintf("%p\n", c->x+c->nx);
	return c->x+c->nx;
}

// mod|reg|rm

static void
movm2rax(Code *c, void *addr)
{
	unsigned char *x;
	u64 *p;
	x = nextx(c);

	x[0] = RexW;		// mov m64,%rax
	x[1] = 0xa1;
	p = (u64*)&x[2];
	*p = (u64)addr;
	c->nx += 10;
}

static void
movr2r(Code *c, Reg6 src, Reg6 dst)
{
	unsigned char *x;
	if(src == dst)
		return;
	if(src >= Rr8 || dst >= Rr8)
		/* FIXME: easy: just roll into the right REX bit */
		fatal("unsupported operand");
	x = nextx(c);
	x[0] = RexW;		// mov r/m,r
	x[1] = 0x8b;
	x[2] = (3<<6)|(dst<<3)|src;
	c->nx += 3;
}

static void
ld(Code *c, Reg6 src, Reg6 dst)
{
	unsigned char *x;
	if(src >= Rr8 || dst >= Rr8)
		fatal("unsupported operand");
	x = nextx(c);
	x[0] = RexW;		// mov r/m,r
	x[1] = 0x8b;
	x[2] = (0<<6)|(dst<<3)|src;
	c->nx += 3;
}

static void
st(Code *c, Reg6 src, Reg6 dst)
{
	unsigned char *x;
	if(src >= Rr8 || dst >= Rr8)
		fatal("unsupported operand");
	x = nextx(c);
	x[0] = RexW;		// mov r,r/m
	x[1] = 0x89;
	x[2] = (0<<6)|(dst<<3)|src;
	c->nx += 3;
}

static void
getval(Code *c, Location *l, Reg6 reg)
{
	switch(LOCKIND(l->loc)){
	case Lreg:
		switch(LOCIDX(l->loc)){
		case Rac:
			movr2r(c, Rrax, reg);
			break;
		case Rsp:
		case Rfp:
		case Rpc:
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		fatal("unimplemented");
	case Llocal:
		fatal("unimplemented");
	case Ldisp:
		fatal("unimplemented");
	case Ltopl:
		movm2rax(c, l->var->val);
		ld(c, Rrax, reg);
		/* FIXME: check for unbound variable Qundef */
//		if(Vkind(p) == Qundef)
//			vmerr(vm, "reference to unbound variable: %s",
//			      l->var->id);
		break;
	default:
		fatal("bug");
	}
}

static void
getvalrand(Code *c, Operand *r, Reg6 reg)
{
	Val v;
	switch(r->okind){
	case Oloc:
		getval(c, &r->u.loc, reg);
		break;
	case Okon:
		v = r->u.kon;
		movm2rax(c, v);
		movr2r(c, Rrax, reg);
		/* FIXME: copy string constants in case they are mutated */
		break;
	case Onil:
		fatal("unimplemented");
		break;
	default:
		fatal("bug");
	}
}

static void
putval(Code *c, Reg6 reg, Location *l)
{
	switch(LOCKIND(l->loc)){
	case Lreg:
		switch(LOCIDX(l->loc)){
		case Rac:
			movr2r(c, reg, Rrax);
			break;
		case Rsp:
		case Rfp:
		case Rpc:
		case Rcl:
		default:
			fatal("bug");
		}
		break;
	case Lparam:
		fatal("unimplemented");
	case Llocal:
		fatal("unimplemented");
	case Ldisp:
		fatal("unimplemented");
	case Ltopl:
		movm2rax(c, l->var->val);
		st(c, reg, Rrax);
		break;
	default:
		fatal("bug");
	}
}

static void
putvalrand(Code *c, Reg6 reg, Operand *r)
{
	if(r->okind != Oloc)
		fatal("bad destination");
	putval(c, reg, &r->u.loc);
}

static void
cg6fn(Code *c, Expr *e)
{

}

void
cg6(Code *c, Expr *e)
{
	unsigned i;

	c->maxx = 100*c->ninsn;    /* guess what we'll need */
	c->x = emalloc(c->maxx);
	c->nx = 0;
}
