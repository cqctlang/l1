typedef
enum xReg {
	rNone,
	rRAX, rRCX, rRDX, rRBX, rRSP, rRBP, rRSI, rRDI,
	rR8, rR9, rR10, rR11, rR12, rR13, rR14, rR15,
	rEAX, rECX, rEDX, rEBX, rESP, rEBP, rESI, rEDI,
	rAX, rCX, rDX, rBX, rSP, rBP, rSI, rDI,
	rAL, rCL, rDL, rBL,
	rAH, rCH, rDH, rBH,
	rSPL, rBPL, rSIL, rDIL,
} xReg;

typedef
enum Prefix {
	REX  = 0x40,
	REXB = 0x41,
	REXX = 0x42,
	REXR = 0x44,
	REXW = 0x48,
} Prefix;

typedef
enum xOpkind {
	opNone,
	opReg,
	opMem,
	opImm,
} xOpkind;

typedef
enum xSize {
	Zero = 0,
	Byte = 8,
	Word = 16,
	DWord = 32,
	QWord = 64,
} xSize;

typedef
struct xImm {
	enum xSize size;
	union {
		u8	ubyte;
		s8	sbyte;
		u16	uword;
		s16	sword;
		u32	udword;
		s32	sdword;
		u64	uqword;
		s64	sqword;
	} v;
} xImm;

typedef
struct xMem {
	xReg base;
	xReg idx;
	u8 scale;
	xImm disp;
} xMem;

typedef
struct xRand {
	xOpkind okind;
	union{
		xReg r;
		xMem m;
		xImm i;
	} u;
} xRand;

#define isnone(o)    ((o).okind == opNone)
#define isreg(o)     ((o).okind == opReg)
#define ismem(o)     ((o).okind == opMem)
#define isimm(o)     ((o).okind == opImm)

#define isrnone(r)   ((r) == rNone)
#define isdnone(i)   ((i).size == Zero)

#define randreg(o)   ((o).u.r)
#define randbase(o)  ((o).u.m.base)
#define randidx(o)   ((o).u.m.idx)
#define randscale(o) ((o).u.m.scale)
#define randdisp(o)  ((o).u.m.disp)
#define randimm(o)   ((o).u.i)

typedef
struct xOp {
	u8 extended;
	u8 op0, op1;
} xOp;

#define isxop(op)    ((op).extended != 0)

extern xRand NONE;
extern xRand RNONE, RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
	R8, R9, R10, R11, R12, R13, R14, R15,
	EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI,
	AX, CX, DX, BX, SP, BP, SI, DI,
	AL, CL, DL, BL, SPL, BPL, SIL, DIL,
	AH, CH, DH, BH;

typedef
struct NC {
	u8 *buf, *p;
	u32 n, max;
} NC;


/* amd64.c */
int		testamd64();
