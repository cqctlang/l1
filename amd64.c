#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef int8_t		s8;
typedef int16_t		s16;
typedef int32_t		s32;
typedef int64_t		s64;
typedef uint8_t		u8;
typedef uint16_t	u16;
typedef uint32_t	u32;
typedef uint64_t	u64;
typedef float		f32;
typedef double		f64;
typedef uintptr_t uptr;

__attribute__((noreturn))
static void
bug()
{
	fprintf(stderr, "bug\n");
	abort();
}

static void*
emalloc(size_t sz)
{
	void *p;
	p = malloc(sz);
	if(p == 0)
		bug();
	return p;
}

static void
efree(void *p)
{
	free(p);
}

static void*
erealloc(void *p, size_t old, size_t new)
{
	void *q;
	q = emalloc(new);
	if(new > old)
		memcpy(q, p, old);
	else
		memcpy(q, p, new);
	efree(p);
	return q;
}

#include "amd64.h"
#if 0
#include "sys.h"
#include "util.h"
#include "syscqct.h"
#endif



xRand NONE = { .okind = opNone };

xRand RNONE = { .okind = opReg, .u.r = rNone };
/* FIXME: steal that trick from luajit */
#define defreg(R) xRand R = { .okind = opReg, .u.r = r##R }
defreg(RAX);
defreg(RCX);
defreg(RDX);
defreg(RBX);
defreg(RSP);
defreg(RBP);
defreg(RSI);
defreg(RDI);
defreg(R8);
defreg(R9);
defreg(R10);
defreg(R11);
defreg(R12);
defreg(R13);
defreg(R14);
defreg(R15);
defreg(EAX);
defreg(ECX);
defreg(EDX);
defreg(EBX);
defreg(ESP);
defreg(EBP);
defreg(ESI);
defreg(EDI);
defreg(AX);
defreg(CX);
defreg(DX);
defreg(BX);
defreg(SP);
defreg(BP);
defreg(SI);
defreg(DI);
defreg(AL);
defreg(CL);
defreg(DL);
defreg(BL);
defreg(SPL);
defreg(BPL);
defreg(SIL);
defreg(DIL);
defreg(AH);
defreg(CH);
defreg(DH);
defreg(BH);
#undef defreg;

NC*
mknc()
{
	NC *nc;
	nc = emalloc(sizeof(NC));
	nc->max = 256;
	nc->p = nc->buf = emalloc(nc->max*sizeof(u8));
	return nc;
}

void
freenc(NC *nc)
{
	efree(nc);
}

static void
ncclr(NC *nc)
{
	nc->p = nc->buf;
	nc->n = 0;
}

static void
nccap(NC *nc, u32 m)
{
	if(nc->n-nc->max >= m)
		return;
	nc->buf = erealloc(nc->buf, nc->max*sizeof(u8), 2*nc->max*sizeof(u8));
	nc->max *= 2;
	nc->p = nc->buf+nc->n;
	nccap(nc, m);
}

static void
emitu8(NC *nc, u8 w)
{
	nccap(nc, sizeof(w));
	*nc->p++ = w;
	nc->n += sizeof(w);
}

static void
emituptr(NC *nc, uptr w)
{
	nccap(nc, sizeof(w));
	*nc->p++ = w;
	nc->n += sizeof(w);
}

static void
n1(NC *nc, xImm imm)
{
	emitu8(nc, imm.v.sbyte);
}

static void
n4(NC *nc, xImm imm)
{
	emitu8(nc, imm.v.sdword);
}

static void
nuptr(NC *nc, xImm imm)
{
	switch(imm.size){
	case Byte:
		emituptr(nc, imm.v.ubyte);
		break;
	case Word:
		emituptr(nc, imm.v.uword);
		break;
	case DWord:
		emituptr(nc, imm.v.udword);
		break;
	case QWord:
		emituptr(nc, imm.v.uqword);
		break;
	case Zero:
	default:
		bug();
	}
}

static xOp
mkop(u8 bits)
{
	xOp op;
	op.extended = 0;
	op.op0 = bits;
	return op;
}

static xOp
mkxop(u8 bits)
{
	xOp op;
	op.extended = 1;
	op.op0 = 017;
	op.op1 = bits;
	return op;
}

static xOp
opcodeor(xOp in, u8 bits)
{
	xOp op;
	op = in;
	if(op.extended)
		op.op0 |= bits;	/* FIXME! this seems wrong.  should it be op1?
				   i.e., the actual opcode!?!
				   if so, fix in cqct as well. */
	else
		op.op0 |= bits;
	return op;
}

static int
isreg8(xReg r)
{
	return r >= rAL && r <= rBH;
}

static int
isreg16(xReg r)
{
	return r >= rAX && r < rDI;
}

static int
isreg32(xReg r)
{
	return r >= rEAX && r < rEDI;
}

static int
isreg64(xReg r)
{
	return r >= rRAX && r <= rR15;
}

static int
isrand64(xRand rand)
{
	if(isreg(rand))
		return isreg64(randreg(rand));
	else if(ismem(rand))
		return isreg64(randbase(rand)) || isreg64(randidx(rand));
	else
		return 0;
}

static int
_regbits(xReg r)
{
	if(isreg64(r))
		return r-rRAX;
	else if(isreg32(r))
		return r-rEAX;
	else if(isreg16(r))
		return r-rAX;
	else if(isreg8(r))
		return r-rAL;
	else if(r == rNone)
		bug();
	/* FIXME: broken on high reg8 */
	bug();
}

static int
regbits(xReg r)
{
	return _regbits(r)&0x7;
}

static int
isxreg(xReg r)
{
	return isreg64(r) && _regbits(r) > 7;
}

static int
isbyte(xImm imm)
{
	return imm.size == Byte;
}

static int
fitsbyte(xImm imm)
{
	return imm.size == Byte || imm.v.uqword < 256;
}

static int
isxrand(xRand o)
{
	if(isreg(o))
		return isxreg(randreg(o));
	else if(ismem(o))
		return isxreg(randbase(o));
	else
		return 0;
}

static int
isxrand8(xRand o)
{
	if(isreg(o))
		switch(randreg(o)){
		case rSPL:
		case rBPL:
		case rSIL:
		case rDIL:
			return 1;
		default:
			return 0;
		}
	return 0;
}

static void
emitopcode(NC *nc, xOp op)
{
	emitu8(nc, op.op0);
	if(op.extended)
		emitu8(nc, op.op1);
}

static int
sibpresent(xRand rand)
{
	switch(rand.okind){
	case opMem:
		if(!isrnone(randidx(rand)) || randscale(rand) != 0)
			return 1;
		switch(randbase(rand)){
		case rESP:
		case rRSP:
		case rR12:
			return 1;
		default:
			return 0;
		}
	case opReg:
		return 0;
	case opNone:
	case opImm:
	default:
		bug();
		return 0;
	}

}

static void
modrm(NC *nc, u8 rbits, xRand rand)
{
	xImm d;
	u8 b, m, rm;

	/* reg */
	b = (rbits&0x7)<<3;

	/* mod */
	switch(rand.okind){
	case opReg:
		m = 0x3;
		break;
	case opMem:
		if(!isrnone(randbase(rand))){
			d = randdisp(rand);
			if(d.size == Zero)
				m = 0x0;
			else if(fitsbyte(d))
				m = 0x1;
			else
				m = 0x2;
		}else
			m = 0x0;
	case opImm:
	case opNone:
	default:
		bug();
	}
	b |= m<<6;

	/* r/m */
	switch(rand.okind){
	case opMem:
		if(sibpresent(rand))
			rm = regbits(rRSP);
		else
			rm = regbits(randbase(rand));
		break;
	case opReg:
		rm = regbits(randreg(rand));
		break;
	case opImm:
	case opNone:
	default:
		bug();
	}
	b |= rm;

	emitu8(nc, b);
}

static void
sib(NC *nc, xRand rand)
{
	u8 s, i, b;
	if(sibpresent(rand)){
		s = randscale(rand);
		i = isrnone(randidx(rand))  ? rESP : randidx(rand);
		b = isrnone(randbase(rand)) ? rEBP : randbase(rand);
		emitu8(nc, s<<6|i<<3|b);
	}
}

static void
disp(NC *nc, xRand rand)
{
	xImm imm;
	switch(rand.okind){
	case opMem:
		imm = randdisp(rand);
		if(imm.size == Zero)
			return;
		if(!isrnone(randbase(rand)) && fitsbyte(imm))
			n1(nc, imm);
		else
			n4(nc, imm);
		break;
	case opReg:
		return;
	case opNone:
	case opImm:
		bug();
	}
}

static void
emitaddressing(NC *nc, u8 rbits, xRand rand)
{
	modrm(nc, rbits, rand);
	sib(nc, rand);
	disp(nc, rand);
}

static void
prefix16(NC *nc, xRand rand)
{
	if(isreg(rand) && isreg16(randreg(rand)))
		emitu8(nc, 0x66);
}

static void
prefixrex(NC *nc, xRand reg, xRand rand, u8 w)
{
	u8 rex;

	rex = REX;
	if(w && ((isnone(reg) && isrand64(rand)) || isrand64(reg)))
		/* FIXME: handler register-128 case */
		rex |= REXW;
	if(isxrand(reg))
		rex |= REXR;
	if(isxrand(rand))
		rex |= REXB;
	if(ismem(rand) && isxreg(randidx(rand)))
		rex |= REXX;

	/* FIXME: handle extended 8-bit register case */
	if(rex == REX && !isxrand8(reg) && !isxrand8(rand))
		; /* no rex prefix */
	else
		emitu8(nc, rex);
}

static void
shortop(NC *nc, xRand dst, u8 n, u8 w)
{
	assert(isreg(dst));
	prefix16(nc, dst);
	prefixrex(nc, NONE, dst, w);
	emitu8(nc, regbits(randreg(dst))+n);
}

static void
op1(NC *nc, xRand dst, u8 rbits, xOp op, u8 w)
{
	prefix16(nc, dst);
	prefixrex(nc, NONE, dst, w);
	emitopcode(nc, op);
	emitaddressing(nc, rbits, dst);
}

static void
imm1x(NC *nc, xRand dst, xImm imm, u8 rbits, xOp op, u8 w)
{
	op1(nc, dst, rbits, op, w);
	n1(nc, imm);
}

static void
imm1(NC *nc, xRand dst, xImm imm, u8 rbits, xOp op, u8 w)
{
	/* FIXME: all callers seem to have established that imm is byte.
	   so why the else path?  fix in cqct if shown to be dumb.
	*/
	if(isbyte(imm))
		imm1x(nc, dst, imm, rbits, op, w);
	else
		imm1x(nc, dst, imm, rbits, opcodeor(op, 1), w); /* ? */
}

static void
imm4(NC *nc, xRand dst, xImm imm, u8 rbits, xOp op, u8 w)
{
	op1(nc, dst, rbits, opcodeor(op, 1), w);
	n4(nc, imm);
}

static void
op2x(NC *nc, xOp op, xRand dst, xRand src, u8 w)
{
	prefixrex(nc, dst, src, w);
	emitopcode(nc, op);
	emitaddressing(nc, regbits(randreg(dst)), src);
}

static void
op2(NC *nc, xOp op, xRand dst, xRand src, u8 w)
{
	xRand t;

	/* pick a direction */
	if(isreg(dst) && !isreg(src))
		op = opcodeor(op, 0x2);
	else{
		/* swapping looks odd, but these are really
		   positions in modrm byte */
		t = dst;
		dst = src;
		src = t;
	}

	/* pick a size */
	if(!isreg(dst) || !isreg8(randreg(dst)))
		op = opcodeor(op, 0x1);

	prefix16(nc, dst);
	op2x(nc, op, dst, src, w);
}

static void
movi(NC *nc, xRand dst, xImm srci)
{
	switch(dst.okind){
	case opReg:
		if(isbyte(srci)){
			shortop(nc, dst, 0xb0, REXW);
			n1(nc, srci);
		}else{
			shortop(nc, dst, 0xb8, REXW);
			nuptr(nc, srci);
		}
		break;
	case opImm:
		if(isbyte(srci))
			imm1(nc, dst, srci, 000, mkop(0xc6), REXW);
		else
			imm4(nc, dst, srci, 000, mkop(0xc6), REXW);
		break;
	case opMem:
	case opNone:
		bug();
	}
}

void
MOV(NC *nc, xRand dst, xRand src)
{
	if(isreg(src) || ismem(src))
		op2(nc, mkop(0x88), dst, src, REXW);
	else
		movi(nc, dst, randimm(src));
}

static void
printbytes(void *buf, u32 m)
{
	u32 i;
	unsigned char *p;
	for(i = 0, p = buf; i < m; i++, p++)
		printf("\\x\%2x", *p);
}

#define test(form, result)						\
{									\
	ncclr(p);							\
	form;								\
	if(p->n != sizeof(result)-1 || memcmp(p->buf, result, p->n)){   \
		printf("failed " #form "\n");				\
		printf("\texpected "); printbytes(result, sizeof(result)-1); \
		printf("\n");						\
		printf("\treceived "); printbytes(p->buf, p->n);	\
		printf("\n");						\
	}								\
}

int
testamd64()
{
	NC *p;
	p = mknc();
#include "amd64.tests.h"
//	test(MOV(nc, RDX, R12), "\x4c\x89\xe2");
//	test(MOV(nc, R12, RDX), "\x49\x89\xd4");
//	test(MOV(nc, EDX, ECX), "\x89\xca");
//	test(MOV(nc, BL, CH), "\x88\xeb");
	freenc(p);
	return 0;
}
