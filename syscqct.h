#ifndef _BISONFLAW_SYSCQCT_H_
#define _BISONFLAW_SYSCQCT_H_

#include "cqct.h"

/* #define since these are not all int-sized */
#define	Vintmax		2147483647ULL
#define	Vuintmax	4294967295ULL
#define	Vlongmax	2147483647ULL
#define Vulongmax	4294967295ULL
#define	Vvlongmax	9223372036854775807ULL
#define	Vuvlongmax	18446744073709551615ULL

typedef
enum{
	Ebool=0,
	Echar,
	Ecomplex,
	Edouble,
	Efloat,
	Eint,
	Elong,
	Eshort,
	Esigned,
	Eunsigned,
	Evoid,
	Enbase,

	Eadd,			/* 11 */
	Eambig,
	Earef,
	Earr,
	Earrow,
	Eband,
	Ebase,
	Ebitfield,
	Eblock,
	Ebor,
	Eboxref,
	Eboxset,
	Ebreak,
	Ebxor,
	Ecall,
	Ecase,
	Ecast,
	Ecomma,
	Econd,
	Econtainer,
	Econtinue,
	Edecl,			/* type specifier + 1 declarator */
	Edecls,			/* type specifier + list of declarators */
	Edefault,
	Edefine,
	Edefloc,
	Edefstx,
	Ederef,
	Ediv,
	Edo,
	Edot,
	Edotdot,
	Eelist,
	Eellipsis,
	Eenum,
	Eenumel,
	Eeq,
	Efieldoff,
	Efields,
	Efor,
	Efun,
	Eg,
	Egadd,
	Egband,
	Egbor,
	Egbxor,
	Egdiv,
	Ege,
	Egmod,
	Egmul,
	Egshl,
	Egshr,
	Egsub,
	Eglobal,
	Egoto,
	Egt,
	Eid,
	Eif,
	Elabel,
	Elambda,
	Elapply,
	Eland,
	Ele,
	Eletrec,
	Elist,
	Epair,
	Elor,
	Elt,
	Emcall,
	Ematch,
	Emkbox,
	Emkbox0,
	Emkctype,
	Emod,
	Emul,
	Enames,
	Eneq,
	Enop,
	Enil,
	Enull,
	Eorpat,
	Epostdec,
	Epostinc,
	Epredec,
	Epreinc,
	Eptr,
	Eref,
	Eret,
	Escope,
	Eshl,
	Eshr,
	Esizeofe,
	Esizeoft,
	Estruct,
	Estx,
	Estxlist,
	Estxquote,
	Estxquasi,
	Estxsplice,
	Estxunquote,
	Esub,
	Eswitch,
	Etab,
	Eticke,
	Etickt,
	Etypedef,
	Etypename,
	Etypeofe,
	Etypeoft,
	Etypespec,
	Euminus,
	Eunion,
	Eunot,
	Euplus,
	Eutwiddle,
	Eval,
	Ewhile,
	Excast,

	/* open-coded primitives */
	E_tid,
	E_tg,

	/* compiler attributes */
	E_attr,

	Emax,
	Euser,
} Kind;

#define EBINOP       \
	     Eadd:   \
	case Esub:   \
	case Emul:   \
	case Ediv:   \
	case Emod:   \
	case Eshl:   \
	case Eshr:   \
	case Eband:  \
	case Ebor:   \
	case Ebxor:  \
	case Egt:    \
	case Ege:    \
	case Elt:    \
	case Ele:    \
	case Eeq:    \
	case Eneq:   \
	case Excast

#define EGOP         \
	     Egadd:  \
	case Egband: \
	case Egbor:  \
	case Egbxor: \
	case Egdiv:  \
	case Egmod:  \
	case Egmul:  \
	case Egshl:  \
	case Egshr:  \
	case Egsub

/* ctypes */
typedef
enum Tkind {
	Tvoid=0,
	Tbase,
	Tstruct,
	Tunion,
	Tenum,
	Tptr,
	Tarr,
	Tfun,
	Ttypedef,
	Tbitfield,
	Tconst,
	Tundef,
} Tkind;
enum {
	Tincomplete=0,
	Tcomplete,

	Tntkind=Tundef+1,	/* keep outside of Tkind */
};

enum {
	Tabinitsize=1,	/* power of 2 */
	Listinitsize=16,
	Maxprintint=32,
	Typepos=0,
	Idpos=1,
	Attrpos=2,

	/* frame offsets */
	Ora = 0,		/* offset of return address */
	Ocl,			/* offset of caller */
	Onfrhd,			/* number of words in frame preamble */
	Oarg0=Onfrhd,		/* offset of first argument */

	Maxstk = 16384*8,
	Redline = 16*sizeof(uptr),
	Maxargs = ((Maxstk-Onfrhd*sizeof(uptr)-Redline)/sizeof(uptr)),

	Maxlit		= 70,	/* longest literal (binary) plus null */
	InsnAlloc = 10,
	AllocBatch = 128,
};

typedef struct Ctl Ctl;
typedef struct Code Code;
typedef struct Cont Cont;
typedef struct Precode Precode;
typedef struct Insn Insn;
typedef struct Expr Expr;

typedef struct As As;
typedef struct Box Box;
typedef struct Cid Cid;
typedef struct Ctype Ctype;
typedef struct Cval Cval;
typedef struct Dom Dom;
typedef struct Fd Fd;
typedef struct List List;
typedef struct Ns Ns;
typedef struct Pair Pair;
typedef struct Range Range;
typedef struct Rd Rd;
//typedef struct Rec Rec;
typedef struct Str Str;
typedef struct Tab Tab;
typedef struct Vec Vec;

typedef Vec* Src;
#define SRCLEN 2
#define srcfileval(s) (vecdata(s)[0])
#define srclineval(s) (vecdata(s)[1])
#define srcfile(s)    (ciddata(valcid(srcfileval(s))))
#define srcline(s)    (cvalu(valcval(srclineval(s))))

typedef
struct Ysrc {
	char *filename;
	unsigned col, line;
} Ysrc;

typedef
union Enc
{
	Imm u;
	float f;
	double d;
} Enc;

typedef
struct Lit {
	Enc v;
	Cbase base;
} Lit;

struct Expr {
	Head hd;
	Kind kind;
	Cid *skind;
	Expr *e1;
	Expr *e2;
	Expr *e3;
	Expr *e4;
	Val aux;		/* Eid, Eval */
	char attr;		/* disambiguation attribute */

	/* source */
	Src src;

	/* compiler-managed fields */
	void *xp;
};

#define sete1(e,w)    do{ ((e)->e1 = (w)); gcwb((Val)(e)); }while(0)
#define sete2(e,w)    do{ ((e)->e2 = (w)); gcwb((Val)(e)); }while(0)
#define sete3(e,w)    do{ ((e)->e3 = (w)); gcwb((Val)(e)); }while(0)
#define sete4(e,w)    do{ ((e)->e4 = (w)); gcwb((Val)(e)); }while(0)
#define setaux(e,p)   do{ ((e)->aux = (p)); gcwb((Val)(e)); }while(0)
#define setsrc(e,s)   do{ ((e)->src = (s)); gcwb((Val)(e)); }while(0)
#define setskind(e,s) do{ ((e)->skind = (s)); gcwb((Val)(e)); }while(0)

typedef void (Cfn)(VM *vm, Imm argc, Val *argv, Val *rv);
typedef void (Ccl)(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv);
typedef void (Xentry)(VM *vm, Closure *cl, Imm argc);

extern Closure *xentry;

struct Cid {
	Head hd;
	u64 len;
};

#define ciddata(x) ((char*)((x)+1))
#define cidsize(n) (sizeof(Cid)+(n)*sizeof(char))

struct Cval {
	Head hd;
	Dom *dom;
	Ctype *type;
	Enc v;
//	Imm val;
};

#define cvalenc(cv) ((cv)->v)
#define cvalu(cv) ((cv)->v.u)
#define cvalf(cv) ((cv)->v.f)
#define cvald(cv) ((cv)->v.d)

struct Closure {
	Head hd;
	Code *code;
	unsigned dlen;
	/* display follows */
};

#define cldisp(x) ((Val*)((x)+1))
#define clsize(n) (sizeof(Closure)+(n)*sizeof(Val))

struct Box {
	Head hd;
	Val v;
};

struct Tab {
	Head hd;
	u32 sz, nent;
	Val def;		/* default value */
	Vec *ht;
	Pair *tg;		/* transport guardian */
	int (*equal)(Val, Val);
	u32 (*hash)(Val);
	u32 priv;
	Str *name;
	Closure *fmt;           /* printer */
};

struct List {
	/* invariants:
	 *  h <= t <= v->len
	 *  if(h == t)
	 *  	list is empty.
	 *  else
	 *  	t>h and list has t-h elements,
	 *      hd points to first element in list, and
	 *	t-1 points to last.
	 */
	Head hd;
	Imm h, t;
	Vec *v;  /* indirect to allow re-size */
};

#define listdata(x) (vecdata(((List*)(x))->v))
#define listcap(l)  ((l)->v->len)

struct As {
	Head hd;

	Tab *mtab;
	Str *name;

	/* cached methods */
	Closure *get;
	Closure *put;
	Closure *ismapped;
	Closure *map;
	Closure *dispatch;
};

struct Ns {
	Head hd;

	Tab *mtab;
	Str *name;

	/* cached methods */
	Closure *enumsym;
	Closure *enumtype;
	Closure *looksym;
	Closure *looktype;
	Closure *lookaddr;
	Closure *dispatch;

	/* cached base type definition */
	Ctype *base[Vnallbase];
};

struct Dom {
	Head hd;
	As *as;
	Ns *ns;
	Str *name;
};

struct Fd {
	Head hd;
	union {
		Xfd fn;
		struct {
			Closure *read;
			Closure *write;
			Closure *close;
		} cl;
	} u;
	Str *name;
	enum Fflag {
		Ffn =		1,
		Fclosed =	Ffn<<1,
		Fread =		Fclosed<<1,
		Fwrite =	Fread<<1,
	} flags;
};

struct Pair {
	Head hd;
	Val car;
	Val cdr;
};

struct Range {
	Head hd;
	Cval *beg;
	Cval *len;
};

typedef
enum {
	Sperm = 1,			/* don't free */
	Sheap = 2,			/* managed by gc */
	Sro = 4,			/* read-only */
} Skind;

struct Str {
	Head hd;
	u8 skind;
	u64 len;
};

typedef
struct Strperm {
	Str str;
	char *s;		/* data */
} Strperm;

#define strdata(x) (((x)->skind & Sheap)	   \
		    ? (char*)((x)+1)	           \
	            : (((Strperm*)(x))->s))
/* size of Sheap strings */
#define strsize(n) (sizeof(Str)+(n)*sizeof(char))
/* is the string read-only? */
#define strro(x) (((x)->skind & Sro)?1:0)

struct Vec {
	Head hd;
	Imm len;
};

#define vecdata(x) ((Val*)((Vec*)(x)+1))
#define vecsize(n) (sizeof(Vec)+(n)*sizeof(Val))

struct Ctype {
	Head hd;
	Tkind tkind;		/* = Tbase, Tstruct, ... */
};

typedef
struct Ctypebase {
	Ctype ct;
	Cbase cbase;
	Rkind rep;
} Ctypebase;

typedef
struct Ctypedef {
	Ctype ct;
	Cid *tid;
	Ctype *sub;
} Ctypedef;

typedef
struct Ctypesu {
	Ctype ct;
	Cid *tag;
	Vec *field;
	Val attr;
} Ctypesu;

typedef
struct Ctypeenum {
	Ctype ct;
/*	Rkind rep;   FIXME: seems there should be a rep or sub, not both */
	Cid *tag;
	Vec *konst;
	Ctype *sub;
} Ctypeenum;

typedef
struct Ctypefunc {
	Ctype ct;
	Ctype *sub;		/* return type */
	Vec *param;
} Ctypefunc;

typedef
struct Ctypeptr {
	Ctype ct;
	Rkind rep;
	Ctype *sub;
} Ctypeptr;

typedef
struct Ctypearr {
	Ctype ct;
	Val cnt;
	Ctype *sub;
} Ctypearr;

typedef
struct Ctypebitfield {
	Ctype ct;
	Val cnt;
	Val bit0;
	Ctype *sub;
} Ctypebitfield;

typedef
struct Ctypeconst {
	Ctype ct;
	Ctype *sub;
} Ctypeconst;

typedef
struct Ctypeundef {
	Ctype ct;
	Ctype *sub;
} Ctypeundef;

enum {
	MaxIn	= 128,
};

typedef struct YYstate YYstate;

typedef
struct In {
	struct Ysrc src;
	char *buf;
	int dofree;
	YYstate *yy;
} In;

typedef
struct U {
	jmp_buf jmp;
	VM *vm;
	In in[MaxIn];
	In *inp;
	Expr *el;		/* parser accumulator */
	Xfd *out;
	Env top;		/* toplevel (compiler) */
} U;

typedef
enum {
	Iabort = 0,
	Iadd,
	Iaddfp,
	Iand,
	Iapply,
	Iargc,
	Ibox,
	Ibox0,
	Icall,
	Icall2,
	Icallt,
	Ichkcl,
	Ichkint,
	Ichksp,
	Icmpeq,
	Icmpgt,
	Icmpge,
	Icmplt,
	Icmple,
	Icmpneq,
	Idiv,
	Igcpoll,
	Ihalt,
	Iinv,
	Iiret,
	Ijmp,
	Ijnz,
	Ijz,
	Ikg,
	Ikp,
	Imod,
	Imov,
	Imovvc,
	Imul,
	Ineg,
	Inot,
	Inop,
	Ior,
	Iret,
	Ishl,
	Ishr,
	Isub,
	Iunderflow,
	Ivargc,
	Ixcast,
	Ixor,
	Iopmax=Ixor
} ikind;

typedef
enum {
	Rvc = 0,
	Rfp,
	Rpc,
	Rac,
	Rcl,
} Reg;

typedef
struct Vs
{
	Expr *vs;
} Vs;

typedef
enum
{
	Vparam,
	Vlocal,
	Vtopl,
	Vtopr,
	Vdisp,
} Vwhere;

typedef
struct Var
{
	char *id;
	Vwhere where;
	unsigned idx;
	unsigned box;
	Cid *sym;		/* Vtopl (FIXME: not gc protected) */
	Pair *kv;		/* Vtopr (FIXME: not gc protected) */
} Var;

typedef
enum {
	/* these need to match Lkind */
	Oreg,
	Oframe,
	Odisp,
	Otopl,
	Otopr,
	Oval,
	Oimm,
	Onil,
} Okind;

typedef
struct Lambda
{
	char *id;
	unsigned isvarg;
	Var **cap;
	unsigned ncap;
	Var *disp;
	unsigned ndisp;
	Var *arg;
	unsigned narg;
	unsigned nloc;
	unsigned ntmp;
} Lambda;

typedef
struct Block
{
	Var *loc;
	unsigned nloc;
} Block;

#define OMODE(idx,box,kind)	(((idx)<<4)|((box&1)<<3)|((kind)&0x7))
#define OIDX(omode)		((omode)>>4)
#define OBOX(omode)		(((omode)>>3)&0x1)
#define OKIND(omode)		((omode)&0x7)

typedef
struct Operand {
	u16 mode;
	union{
		Val val;
		Imm imm;
	};
} Operand;

typedef
enum {
	Clabel,
	Clabelpair,
} Ctlkind;

typedef u32 Ctlidx;

struct Ctl {
	Ctlkind ckind;

	/* ckind == Clabel */
	unsigned long insn;

	/* ckind == Clabelpair */
	Ctlidx l1, l2;
};

struct Insn {
	void *go;
	ikind kind;
	Operand op1, op2, dst;
	union{
		Ctlidx dstidx;
		s64 scnt;
	};
};

typedef
struct Reloc {
	uptr	coff;		/* pointer offset in bytes from start of code */
} Reloc;

typedef
struct Dbg {
	uptr	off;		/* pc offset in bytes from start of code */
	u32	fsz;		/* size of frame */
	u32	lex;		/* index into code's lexical descriptor */
	u64	lm;		/* live mask (directly encoded or offset) */
} Dbg;

struct Precode {
	Head hd;
	unsigned long ninsn, maxinsn;
	Str *insn;
	Cid *id;
	Vec *src;
	Str *ctl;
	u32 nctl, mctl;
	Str *lm;
	u32 nlm, mlm;
	Str *dbg;
	u32 ndbg, mdbg;
	Vec *lex;
	u32 nlex, rlex;
};

/* live mask */
#define mwbits	       (8*sizeof(u64))  /* # bits in mask word */

typedef
enum Ckind {
	Cvm,
	Ccfn,
	Cccl,
	Cnative,
	Calien,
} Ckind;

struct Code {
	Head hd;
	Ckind kind;
	Cid *id;
	Imm sz;		/* bytes from header to end of last insn */
	Imm nreloc;
	u32 nfree;	/* number of free variables */
	Str *reloc;
	Str *lm;
	Str *dbg;	/* debug info, ordered by offset */
	u32 ndbg;	/* number of debug records */
	Vec *src;	/* belongs in pure storage? */
	Vec *lex;	/* lexical descriptor */
	union {
		Cfn *cfn;
		Ccl *ccl;
		Val alien;
	};
	/* instructions follow */
};

//#define trampsize    64
#define trampsize    0
#define _codeinsn(x) ((void*)((Code*)(x)+1))
#define codetramp(x) _codeinsn(x)
#define codekind(x)  (((Code*)x)->kind)
#define codeinsn(x)  (codekind(x) == Cvm ? _codeinsn(x)+trampsize : _codeinsn(x))
#define codeend(x)   ((void*)(x)+(((Code*)(x))->sz))
#define codesize(n)  (sizeof(Code)+(n))
#define codeentry(x) (codeinsn(x))

struct Cont {
	Head hd;
	void *base;
	Cont *link;
	Closure *cl;
	void *ra;
	u32 sz;
	u32 level;
	u64 gen;
};

typedef
struct NC {
	u8 *buf, *p;
	u32 n, max;
} NC;

typedef
struct BFgeom {
	Imm bp, bs;
	Imm addr, cnt;
	unsigned isbe;
	int les, bes;
} BFgeom;

typedef struct Xenv Xenv;
struct Xenv {
	HT *ht;
	Xenv *link;
};

enum {
	VMirq = 1,
};

typedef struct Hashop {
	u32 (*hash)(Val);
	int (*eq)(Val, Val);
} Hashop;

struct VM {
	Val *fp;
	Closure *cl;
	Imm vc;
	Val ac;
	Insn *pc;
	void *stk;
	u32 stksz;
	Cont *klink;
	unsigned int flags;
	Env top;
	u32 level;
	u64 gen;
	u64 levgen[128];
	jmp_buf dovm[128];
	u64 exetime, exelast;
	u64 gctime, postgctime;
};

extern VM*		vms[];
extern u32		nvm;

extern char* S[];
extern char* cbasename[];
extern unsigned isfloat[Vnallbase];
extern unsigned isunsigned[Vnallbase];
extern Imm repsize[Rnrep];
extern char* tkindstr[];
extern char  **cqctloadpath;
extern char *qname[];
extern NC *trampentry;
extern Xfd l1stdin, l1stdout, l1stderr;

/* global roots */
extern Val Xnil;
extern Val syms;
extern Val typecache;
extern Dom *litdom;
extern Closure *halt, *vabort, *stkunderflow;
extern Cval *cvalnull, *cval0, *cval1, *cvalminus1;

/* c.l */
void		freeyystate(YYstate *yy);
YYstate*	mkyystate(FILE *fp);
YYstate*	mkyystatestr(char *buf, u64 len);
void		setyystate(YYstate *yy);

/* parse.c */
Expr*		G(char *s);
Expr*		copyexpr(Expr *e);
Expr*		doconst(U *ctx, char*, unsigned long len);
Expr*		dostr(char*, unsigned long len);
Expr*		doparse(U*, char *buf, u64 len,
			char *whence, u32 wlen, unsigned line);
Expr*		dosym(char *s, unsigned long len);
Expr*		dotickesrc(Ysrc *src, Expr*, Expr*);
Expr*		doticktsrc(Ysrc *src, Expr*, Expr*);
void		dotop(U*, Expr*);
void		expandstr(char *s, unsigned long len, unsigned long *nlen);
void		finiparse(void);
Expr*		flatten(Expr *e);
void		initparse(void);
char*		internfilename(char *s, unsigned len);
Expr*		invert(Expr*);
int		maybepopyy(U *ctx);
Expr*		mkexpr();
Expr*		newbinop(unsigned, Expr*, Expr*);
Expr*		newbinopsrc(Ysrc*, unsigned, Expr*, Expr*);
Expr*		newexpr(unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newexprsrc(Ysrc*, unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newgopsrc(Ysrc*, unsigned, Expr*, Expr*);
Expr*		nullelist(void);
Expr*		nullelistsrc(Ysrc*);
int		parselit(char *s, unsigned long len, Lit *lit,
			 unsigned radix, char **err);
Expr*		ptrto(Expr*, Expr*);
void		parseerror(U *ctx, char *fmt, ...);
int		popyy(U *ctx);
void		tryinclude(U *ctx, char *raw);
int		yyparse(U *ctx);

/* printexpr.c */
void		printcqct(Expr*);
void		printexpr(Expr*);
void		printids(Expr *e);
Kind		s2kind(char *s);

/* bitfield.c */
int		bitfieldgeom(BFgeom *bfg);
Imm		bitfieldget(char *s, BFgeom *bfg);
Imm		bitfieldput(char *s, BFgeom *bfg, Imm val);

/* code.c */
void		addreloc(Code *code, uptr coff);
void		fncode(Env env);
List*		frameinfo(Closure *cl, void *pc);
Code*		mkncode(Imm nbytes);
Imm		ra2mask(void *ra, Code *code);
Imm		ra2size(void *ra, Code *code);

/* compilee.c */
Expr*		docompilee(U *ctx, Expr *e);

/* cpq.c */
Expr*		docompileq(U *ctx, Expr *e);
void		l1_cpq(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpx.c */
Expr*		docompilex(U *ctx, Expr *e);
void		l1_cpx(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpn.c */
Expr*		docompilen(U *ctx, Expr *e);
void		l1_cpn(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpm.c */
Expr*		docompilem(U *ctx, Expr *e);
void		l1_cpm(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpw.c */
Expr*		docompilew(U *ctx, Expr *e);
void		l1_cpw(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpa.c */
Expr*		docompilea(U *ctx, Expr *e);
void		l1_cpa(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpc.c */
Expr*		docompilec(U *ctx, Expr *e);
void		l1_cpc(VM *vm, Imm argc, Val *argv, Val *rv);

/* cp0.c */
Expr*		docompile0(U *ctx, Expr *e);
void		l1_cp0(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpg.c */
Expr*		docompileg(U *ctx, Expr *e);
void		l1_cpg(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpk.c */
Expr*		docompilek(U *ctx, Expr *e);
void		l1_cpk(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpl.c */
Expr*		docompilel(U *ctx, Expr *e);
void		l1_cpl(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpi.c */
Expr*		docompilei(U *ctx, Expr *e);
void		resetcpilabel();
void		l1_cpi(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpr.c */
Expr*		docompiler(U *ctx, Expr *e);
void		l1_cpr(VM *vm, Imm argc, Val *argv, Val *rv);

/* cp1.c */
Expr*		docompile1(U *ctx, Expr *e);
void		l1_cp1(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpb.c */
Expr*		docompileb(U *ctx, Expr *e);
void		l1_cpb(VM *vm, Imm argc, Val *argv, Val *rv);

/* cpv.c */
Expr*		docompilev(U *ctx, Expr *el);
void		freevars(Expr *e);
int		issimple(Expr *e);

/* compileo.c */
Expr*		docompileo(U *ctx, Expr *e);

/* compileu.c */
Expr*		docompileu(U *ctx, Expr *e);

/* compilex.c */
Expr*		docompilex(U *ctx, Expr *e);

/* compilec.c */
Expr*		docompilec(U *ctx, Expr *e);

/* compiles.c */
Expr*		docompiles(U *ctx, Expr *e);

/* cg.c */
Code*		kresumecode();
Closure*	abortthunk(void);
Closure*	codegen(U *ctx, Expr *e);
void		finicg(void);
Closure*	haltthunk(void);
void		initcg(void);
void		l1_codegen(VM *vm, Imm argc, Val *argv, Val *rv);
Closure*	mkkcapture(void);
Closure*	mkapply(void);
Code*		mkcode(Ckind kind, Imm nbytes);
Closure*	mkraiseinterrupt(void);
void		printcode(Code *c);
void		printinsn(Insn *i);
void		printval(Val v);
void		resetlabels();
Closure*	stkunderflowthunk(void);
char*		itos(ikind kind);

/* xenv.c */
void		freexenv(Xenv *xe);
void		freexenvall(Xenv *xe);
Xenv*		mkxenv(Xenv *link);
void		xenvapply(Xenv *xe, HT *d);
Xenv*		xenvcopy(Xenv *xe);
HT*		xenvdiff(Xenv *fr, Xenv *to);
void		xenvlink(Xenv *xe, Xenv *link);
void*		xenvlook(Xenv *xe, char *id);
void		xenvbind(Xenv *xe, char *id, void *v);
void		xenvforeach(Xenv *xe, void (*f)(void *u, char *k, void *v),
			    void *u);
unsigned long	xenvsize(Xenv *xe);
void		xenvupdate(Xenv *xe, char *id, void *v);

/* vm.c */
Src		addr2line(Code *code, Insn *pc);
void		builtinfd(Env env, char *name, Fd *fd);
void		builtinfn(Env env, char *name, Closure *cl);
Str*		callget(VM *vm, As *as, Imm off, Imm len);
Cval*		callismapped(VM *vm, As *as, Imm off, Imm len);
Vec*		callmap(VM *vm, As *as);
void		callput(VM *vm, As *as, Imm off, Imm len, Str *s);
Val		ccall(VM* vm, Closure *cl, Imm argc, Val *argv);
Ctype*		chasetype(Ctype *t);
void		checkarg(VM *vm, Val *argv, unsigned arg, Qkind qkind);
Tab*		doinsncnt(void);
Cval*		domcast(VM *vm, Dom *dom, Cval *cv);
void		dogc(VM *vm, u32 g, u32 tg);
Val		dovm(VM* vm);
void		envbind(Env env, char *id, Val v);
void		envdel(Env env, char *id);
int		envbinds(Env env, Cid *id);
void		envdefine(Env env, Cid *id, Val v);
Val		envget(Env env, Cid *id);
Pair*		envgetkv(Env env, Cid *id);
int		equalval(Val v1, Val v2);
int		eqval(Val v1, Val v2);
int		eqvval(Val v1, Val v2);
Val		expr2syntax(Expr *e);
void		freeenv(Env env);
Str*		getbytes(VM *vm, Cval *addr, Imm n);
int		getlasterrno();
u32		hashval(Val v);
u32		hashqval(Val v);
u32		hashqvval(Val v);
void		initvm();
int		isbasecval(Cval *cv);
int		isnatcval(Cval *cv);
int		isnegcval(Cval *cv);
int		iszerocval(Cval *cv);
void		finivm(void);
void		heapfree(Head *p);
int		iscomplete(Ctype *t);
int		ismapped(VM *vm, As *as, Imm addr, Imm len);
int		isstrcval(Cval *cv);
int		ischarcval(Cval *cv);
Range*		mapstab(VM *vm, Vec *map, Imm addr, Imm len);
As*		mkastab(Tab *mtab, Str *name);
Closure*	mkcfn(char *id, Cfn *cfn);
Closure*	mkcl(Code *code, unsigned len);
Cval*		mkcval(Dom *dom, Ctype *type, Imm val);
Dom*		mkdom(Ns *ns, As *as, Str *name);
Fd*		mkfdfn(Str *name, int flags, Xfd *xfd);
Fd*		mkfdcl(Str *name, int flags,
		       Closure *read, Closure *write, Closure *close);
Cval*		mklitcval(Cbase base, Imm val);
Ns*		mknsraw(VM *vm, Ns *ons, Tab *rawtype,
			Tab *rawsym, Str *name);
Ns*		mknstab(Tab *mtab, Str *name);
Range*		mkrange(Cval *beg, Cval *len);
As*		mksas(Str *s);
Env		mktoplevel();
Val		mkvalbox(Val boxed);
Val		mkvalcval(Dom *dom, Ctype *t, Imm imm);
Val		mkvalcval2(Cval *cv);
Val		mkvallitcval(Cbase base, Imm imm);
Val		mkvallitcvalenc(Cbase base, Enc v);
As*		mkzas(Imm len);
Val		myrootns(Env env);
void		printframe(VM *vm, Insn *pc, Code *c);
jmp_buf*	_pusherror(VM *vm);
void		registercfn(char *name, void *addr);
void		setgo(Code *c);
void		setlasterrno(int no);
u32		shash(char *s, Imm len);
Str*		stringof(VM *vm, Cval *cv);
Cval*		typecast(VM *vm, Ctype *t, Cval *cv);
Val		valboxed(Val v);
Head*		valhead(Val v);
Imm		valimm(Val v);
Str*		valstrorcval(VM *vm, char *fn, Val *argv, unsigned arg);
void		vmerr(VM *vm, char *fmt, ...) NORETURN;
char*		vmfnid(VM *vm);
#ifdef NEVER
Fd*		vmstdout(VM *vm);
#endif
void		vmwarn(VM *vm, char *fmt, ...);
Cval*		xcvalalu(VM *vm, ikind op, Cval *op1, Cval *op2);
Val		xunop(VM *vm, ikind op, Val v);
#define mkvalas(x)	((Val)(x))
#define mkvalcid(x)	((Val)(x))
#define mkvalcl(x)	((Val)(x))
#define mkvalcode(x)	((Val)(x))
#define mkvalcont(x)	((Val)(x))
#define mkvalctype(x)	((Val)(x))
#define mkvaldom(x)	((Val)(x))
#define mkvalexpr(x)	((Val)(x))
#define mkvalfd(x)	((Val)(x))
#define mkvallist(x)	((Val)(x))
#define mkvalns(x)	((Val)(x))
#define mkvalpair(x)	((Val)(x))
#define mkvalrange(x)	((Val)(x))
#define mkvalrd(x)	((Val)(x))
#define mkvalrec(x)	((Val)(x))
#define mkvalstr(x)	((Val)(x))
#define mkvaltab(x)	((Val)(x))
#define mkvalvec(x)	((Val)(x))

#define valas(v)	((As*)(v))
#define valbox(v)	((Box*)(v))
#define valcid(v)	((Cid*)(v))
#define valcl(v)	((Closure*)(v))
#define valcode(v)	((Code*)(v))
#define valcont(v)	((Cont*)(v))
#define valctype(v)	((Ctype*)(v))
#define valcval(v)	((Cval*)(v))
#define valdom(v)	((Dom*)(v))
#define valexpr(v)	((Expr*)(v))
#define valfd(v)	((Fd*)(v))
#define vallist(v)	((List*)(v))
#define valns(v)	((Ns*)(v))
#define valpair(v)	((Pair*)(v))
#define valrange(v)	((Range*)(v))
#define valrd(v)	((Rd*)(v))
#define valrec(v)	((Rec*)(v))
#define valstr(v)	((Str*)(v))
#define valtab(v)	((Tab*)(v))
#define valvec(v)	((Vec*)(v))
#define stkimm(v)	(Imm)(uptr)(v)
#define stkp(v)		(void*)(uptr)(v)
#define REGFN(id)	{ registercfn(#id, (void *)id); }
#define FN(id)		{ builtinfn(env, "%"#id, cqctmkcfn(#id, l1_##id)); REGFN(l1_##id); }

#define mkccl cqctmkccl

/* lib9's reliable, portable snprintf (x/lib9) */
int		snprint(char *buf, int len, char *fmt, ...);
int		vsnprint(char *buf, int len, char *fmt, va_list args);

/* fnfmt.c */
char*		fmtctypec(Ctype *t);
Str*		fmtctype(Ctype *t);
void		l1_sprintfa(VM *vm, Imm argc, Val *argv, Val *rv);

/* fndis.c */
void		disx86(unsigned char *s, unsigned char *e, unsigned inc);

/* xfd.c */
void		cprintf(Xfd *xfd, char *fmt, ...);
void		cvprintf(Xfd *xfd, char *fmt, va_list args);
void		finiio();
void		initio(Xfd *in, Xfd *out, Xfd *err);
void		xfdclose(Xfd *xfd);
Imm		xfdread(Xfd *xfd, char *buf, Imm len);
Imm		xfdwrite(Xfd *xfd, char *buf, Imm len);

/* cutil.c */
void		cerror(U *ctx, Expr *e, char *fmt, ...) NORETURN;
void		cwarn(U *ctx, Expr *e, char *fmt, ...);
Expr*		doid(char*);
Expr*		doidnsrc(Ysrc *src, char *s, unsigned long len);
unsigned	elistlen(Expr *l);
Cid*		idcid(Expr *e);
char*		idsym(Expr *e);
int		isbinop(Kind k);
int		isnull(Expr *e);
Src		mksrc(Ysrc *y);
Src		mksrcfake(char *f);
Expr*		putsrc(Expr *e, Src src);
void		resetuniqid();
Expr*		uniqid(char *id);
Expr*		vdiff(Expr *a, Expr *b);
Expr*		vinsert(Expr *e, Expr *vs);
Expr*		vintersect(Expr *a, Expr *b);
int		visempty(Expr *a);
int		vmember(Expr *e, Expr *l);
void		vsappend(Expr *e, Vs *vs);
void		vsdiff(Vs *a, Expr *b);
void		vsfree(Vs *vs);
void		vsinit(Vs *vs);
void		vsinsert(Expr *e, Vs *vs);
void		vsunion(Vs *from, Vs *to);
Expr*		vunion(Expr *a, Expr *b);
Expr*		Z0(unsigned kind);
Expr*		Z1(unsigned kind, Expr *e1);
Expr*		Z2(unsigned kind, Expr *e1, Expr *e2);
Expr*		Z3(unsigned kind, Expr *e1, Expr *e2, Expr *e3);
Expr*		Z4(unsigned kind, Expr *e1, Expr *e2, Expr *e3, Expr *e4);
Expr*		Zadd(Expr *x, Expr *y);
Expr*		Zapply(Expr *fn, Expr *args);
Expr*		Zbind(Expr *id, Expr *e);
Expr*		Zbinop(unsigned op, Expr *x, Expr *y);
Expr*		Zblock(Expr *locs, ...);
Expr*		Zboxref(Expr *id);
Expr*		Zboxset(Expr *id, Expr *e);
Expr*		Zcall(Expr *fn, unsigned narg, ...);
Expr*		Zcid(char *s);
Expr*		Zcons(Expr *hd, Expr *tl);
Expr*		Zconst(Cbase base, Imm val);
Expr*		Zcval(Expr *dom, Expr *type, Expr *val);
Expr*		Zerror(char *s);
Expr*		Zgbinop(unsigned gop, Expr *x, Expr *y);
Expr*		Zidcid(Cid *s);
Expr*		Zid2sym(Expr *e);
Expr*		Zids2syms(Expr *l);
Expr*		Zif(Expr *cond, Expr *truecond);
Expr*		Zifelse(Expr *cond, Expr *truecond, Expr *falsecond);
Expr*		Zint(Imm val);
Expr*		Zgoto(char *l);
Expr*		Zgotosrc(Ysrc *src, Expr *id);
Expr*		Zlabel(char *l);
Expr*		Zlabelsrc(Src src, Expr *id, Expr *s);
Expr*		Zlambda(Expr *args, Expr *body);
Expr*		Zlambdn(Expr *args, Expr *body, Expr *name);
Expr*		Zletrec(Expr *binds, Expr *body);
Expr*		Zlocals(unsigned n, ...);
Expr*		Zmkbox(Expr *id);
Expr*		Zmkbox0(Expr *id);
Expr*		Znil(void);
Expr*		Znop(void);
Expr*		Znot(Expr *e);
Expr*		Znull();
Expr*		Zref(Expr *dom, Expr *type, Expr *val);
Expr*		Zret(Expr *e);
Expr*		Zreverse(Expr *e);
Expr*		Zscope(Expr *block);
Expr*		Zset(Expr *l, Expr *r);
Expr*		Zsizeof(Expr *e);
Expr*		Zstr(char *s);
Expr*		Zstrn(char *s, Imm len);
Expr*		Zsub(Expr *x, Expr *y);
Expr*		Ztg(char *id, Expr *e);
Expr*		Ztid(char *id);
Expr*		Zuint(Imm val);
Expr*		Zval(Val v);
int		hasvarg(Expr *e);
Expr*		Zvararg(Expr *id);
Expr*		Zxcast(Expr *type, Expr *cval);

/* mem.c */
void		compact(VM *vm);
void		finimem();
void		fullgc(VM *vm);
void		_gc(u32 g, u32 tg);
void		gc(VM *vm);
void		gcdisable();
void		gcenable();
void		gcpoll(VM *vm);
void		gcstatistics();
void		gcstats();
void		gcwb(Val v);
void		guard(Val o, Pair *g);
void		initmem();
void		instguard(Pair *p);
int		ismanagedrange(void *p, Imm len);
int		isweak(Head *h);
Head*		malv(Qkind kind, Imm len);
Head*		malbox();
void*		malstack(u32 sz);
Head*		malweak();
u64		meminuse();
Pair*		mkguard();
Head*		pop1guard(Pair *t);
Head*		pop1tguard(Pair *t);
int		saveheapfd(Tab *toplevel, int fd);
void		tguard(Val o, Pair *g);

/* ch.c */
void		fnch(Env env);

/* cid.c */
Str*		cid2str(Cid *id);
void		finicid();
void		fncid(Env env);
void		initcid();
Cid*		mkcid(char *s, Imm len);
Cid*		mkcid0(char *s);

/* ctype.c */
Ctype*		chasetype(Ctype *t);
int		equalctype(Ctype *a, Ctype *b);
int		eqvctype(Ctype *a, Ctype *b);
void		finitype();
void		fnctype(Env env);
u32		hashctype(Ctype *t);
u32		hashqvctype(Ctype *t);
void		inittype();
Ctype*		mkctypearr(Ctype *sub, Val cnt);
Ctype*		mkctypebase(Cbase cbase, Rkind rep);
Ctype*		mkctypebitfield(Ctype *sub, Val cnt, Val bit0);
Ctype*		mkctypeconst(Ctype *sub);
Ctype*		mkctypedef(Cid *tid, Ctype *sub);
Ctype*		mkctypeenum(Cid *tag, Ctype *sub, Vec *konst);
Ctype*		mkctypefunc(Ctype *sub, Vec *param);
Ctype*		mkctypeptr(Ctype *sub, Rkind rep);
Ctype*		mkctypesu(Tkind kind, Cid *tag, Vec *field, Val attr);
Ctype*		mkctypeundef(Ctype *sub);
Ctype*		mkctypevoid(void);
Ctype*		safechasetype(Ctype *t);
void		setsubtype(Ctype *t, Ctype *s);
Ctype*		subtype(Ctype *t);
Cbase		typecbase(Ctype *t);
Ctype*		ctypename(Ctype *td);
Rkind		typerep(Ctype *t);
void		typesetrep(Ctype *t, Rkind rep);
Imm		typesize(VM *vm, Ctype *t);
Cid*		typetag(Ctype *t);
Cid*		typetid(Ctype *t);

/* cval.c */
u32		hashcval(Cval *v);
u32		hashqcval(Cval *v);
u32		hashqvcval(Cval *v);
int		eqcval(Cval *a, Cval *b);
int		equalcval(Cval *a, Cval *b);
int		eqvcval(Cval *a, Cval *b);
void		fncval(Env env);

/* list.c */
int		equallist(List *a, List *b);
void		fnlist(Env env);
u32		hashlist(List *l);
void		l1_listref(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_listset(VM *vm, Imm argc, Val *argv, Val *rv);
void		_listappend(List *lst, Val v);
List*		listappend(VM *vm, List *lst, Val v);
List*		listconcat(VM *vm, List *l1, List *l2);
List*		listcopy(List *lst);
List*		listdel(VM *vm, List *lst, Imm idx);
List*		listins(VM *vm, List *lst, Imm idx, Val v);
Imm		listlen(List *x);
void		listpop(List *lst, Val *vp);
Val		listref(List *lst, Imm idx);
List*		listset(List *lst, Imm idx, Val v);
List*		mklist(void);
List*		mklistinit(Imm len, Val v);
List*		mklistn(Imm sz);

/* tab.c */
void		fntab(Env env);
Tab*		mktab(void);
Tab*		mktabq(void);
Tab*		mktabqv(void);
int		islink(Val v);
void		l1_tabinsert(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_tablook(VM *vm, Imm argc, Val *argv, Val *rv);
Val		linkkey(Pair *lnk);
Pair*		linkkv(Pair *lnk);
Val		linkval(Pair *lnk);
Val		linknext(Pair *lnk);
Tab*		tabcopy(Tab *tab);
void		tabdel(Tab *tab, Val keyv);
Vec*		tabenum(Tab *tab);
Vec*		tabenumkeys(Tab *tab);
Vec*		tabenumvals(Tab *tab);
Val		tabget(Tab *tab, Val keyv);
Pair*		tabgetkv(Tab *t, Val k);
void		tabpop(Tab *tab, Val *rv);
void		tabput(Tab *tab, Val keyv, Val val);
void		tabrehash(Tab *tab);

/* pair.c */
#define 	car(p)  (((Pair*)(p))->car)
#define 	cdr(p)  (((Pair*)(p))->cdr)
#define 	caar(p) (car(car(p)))
#define 	cadr(p) (car(cdr(p)))
#define 	cdar(p) (cdr(car(p)))
#define 	cddr(p) (cdr(cdr(p)))
#define 	setcar(p,x) do{ gcwb((Val)(p)); car(p) = (Val)(x); }while(0)
#define 	_setcar(p,x) do{ car(p) = (Val)(x); }while(0)
#define 	setcdr(p,x) do{ gcwb((Val)(p)); cdr(p) = (Val)(x); }while(0)
#define 	_setcdr(p,x) do{ cdr(p) = (Val)(x); }while(0)
#define		cons(a,d)  (mkpair((Val)(a), (Val)(d)))
#define		weakcons(a,d)  (mkweakpair((Val)(a), (Val)(d)))
int		equalpair(Pair *a, Pair *b);
void		fnpair(Env env);
u32		hashpair(Pair *p);
Pair*		mkpair(Val a, Val d);
Pair*		mkweakpair(Val a, Val d);

/* rec.c */
//int		equalrec(Rec *a, Rec *b);
//void		finirec();
//u32		hashrec(Rec *r);
//void		initrec();
//void		fnrec(Env env);

/* str.c */
int		equalstr(Str *a, Str *b);
void		fnstr(Env env);
u32		hashstr(Str *s);
void		l1_strref(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_strput(VM *vm, Imm argc, Val *argv, Val *rv);
Str*		mkstr(char *s, Imm len);
Str*		mkstr0(char *s);
Str*		mkstrk(char *s, Imm len, Skind skind);
Str*		mkstrn(Imm len);
char*		str2cstr(Str *str);
Str*		strconcat(Str *s1, Str *s2);
Str*		strcopy(Str *s);
Str*		strrealloc(Str *str, Imm len);
Str*		strslice(Str *str, Imm beg, Imm end);
int		Strcmp(Str *s1, Str *s2);

/* stx.c */
int		equalstx(Expr *a, Expr *b);
int		eqvstx(Expr *a, Expr *b);
void		fnstx(Env env);
u32		hashqvstx(Expr *e);
u32		hashstx(Expr *e);
void		l1_stxref(VM *vm, Imm argc, Val *argv, Val *rv);

/* sym.c */
Val		attroff(Val o);
Val		copyattr(Val attr, Val newoff);
Ctype*		fieldtype(Vec *s);
void		fnsym(Env env);
int		issym(Vec *sym);
int		issymvec(Vec *v);
Val		mkattr(Val o);
Ctype*		paramtype(Vec *s);
Ctype*		symtype(Vec *s);

/* vec.c */
int		equalvec(Vec *a, Vec *b);
void		fnvec(Env env);
u32		hashvec(Vec *v);
void		l1_vecref(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_vecset(VM *vm, Imm argc, Val *argv, Val *rv);
Vec*		mkvec(Imm len);
Vec*		mkvecinit(Imm len, Val v);
Vec*		veccopy(Vec *old);
Imm		veclen(Vec *vec);
Val		vecref(Vec *vec, Imm idx);
Vec*		vecrealloc(Vec *vec, Imm len);
void		_vecset(Vec *vec, Imm idx, Val v);
void		vecset(Vec *vec, Imm idx, Val v);

/* qc.c */
Val		bootcompile(Env top, Expr *e);
void		finiqc();
void		fncompile(Env env);
void		initctx(U *ctx, VM *vm);
void		initctxboot(U *ctx, Env top);
void		initqc();

/* nc.c */
void		emitu8(NC *nc, u8 w);
void		emitu16(NC *nc, u16 w);
void		emitu32(NC *nc, u32 w);
void		emitu64(NC *nc, u64 w);
void		fininc();
void		freenc(NC *nc);
NC*		mknc();
void		ncclr(NC *nc);
void		initnc();

/* boot.c */
void		boot(VM *vm);

/* os-dependent (unix.c, noix.c) */
extern char	sysarch[];
extern char	sysos[];
void		initos();

extern		void fns(Env);

#endif /* _BISONFLAW_SYSCQCT_H_ */
