#ifndef _BISONFLAW_SYSCQCT_H_
#define _BISONFLAW_SYSCQCT_H_

#include "cqct.h"

enum{
	Maxlit		= 70,	/* longest literal (binary) plus null */
	Maxspec		= 32,
};

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
	Edefrec,
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
	Ekon,
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
	Estxquote,
	Estxquasi,
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
	Ewhile,
	Excast,

	/* open-coded primitives */
	E_tid,
	E_tg,
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
	Maxvms=1024,
	Errinitdepth=128,	/* initial max error stack */
	Maxstk = 4096,
	InsnAlloc = 10,
	AllocBatch = 128,
};

typedef struct Ctl Ctl;
typedef struct Code Code;
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
typedef struct Rec Rec;
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
	Val aux;		/* Eid, Ekon */
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
	Insn *entry;
	unsigned dlen;
	Cid *id;
	Imm fp;			/* of continuation, always >0 */
	Cfn *cfn;
	Ccl *ccl;
	Str *xfn;
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

struct Rec {
	Head hd;
	u32 nf;
	Rd *rd;
};

#define recdata(x) ((Val*)((Rec*)(x)+1))
#define recsize(n) (sizeof(Rec)+(n)*sizeof(Val))

struct Rd {
	Head hd;
	Str *name;
	Imm nf;
	List *fname;
	Closure *is;		/* predicate */
	Closure *mk;		/* constructor */
	Closure *fmt;		/* printer */
	Tab *get;		/* field get */
	Tab *set;		/* field set */
};

typedef
enum {
	Sperm,			/* don't free */
	Sheap,			/* managed by gc */
	Smalloc,		/* free with efree() */
	Smmap,			/* free with munmap() */
} Skind;

struct Str {
	Head hd;
	u8 skind;
	u64 len;
};

typedef
struct Strmmap {
	Str str;
	size_t mlen;		/* Smmap size */
	char *s;		/* data */
} Strmmap;

typedef
struct Strperm {
	Str str;
	char *s;		/* data */
} Strperm;

typedef
struct Strmalloc {
	Str str;
	char *s;		/* data */
} Strmalloc;

#define strdata(x) (((x)->skind == Sheap)	   \
		    ? (char*)((x)+1)	           \
	            : (((x)->skind == Smmap)       \
	               ? (((Strmmap*)(x))->s)      \
		       : (((x)->skind == Smalloc)  \
                          ? (((Strmalloc*)(x))->s) \
			  : (((Strperm*)(x))->s))))
/* size of Sheap strings */
#define strsize(n) (sizeof(Str)+(n)*sizeof(char))

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
	In in[MaxIn];
	In *inp;
	Expr *el;		/* parser accumulator */
	unsigned errors;
	Xfd *out;
	Toplevel *top;		/* toplevel (compiler) */
	char *argsid;		/* toplevel arguments identifier (compiler) */
} U;

typedef
enum {
	Iadd = 0,
	Iand,
	Iargc,
	Ibox,
	Ibox0,
	Icall,
	Icallc,
	Icallt,
	Icalltc,
	Iclo,
	Icmpeq,
	Icmpgt,
	Icmpge,
	Icmplt,
	Icmple,
	Icmpneq,
	Icode,
	Idiv,
	Iframe,
	Ifmask,
	Ifsize,
	Ihalt,
	Iinv,
	Ijmp,
	Ijnz,
	Ijz,
	Ikg,
	Ikp,
	Ilist,
	Imod,
	Imov,
	Imul,
	Ineg,
	Inot,
	Ior,
	Inop,
	Ipanic,
	Ipush,
	Ipushi,
	Iret,
	Ishl,
	Ishr,
	Isub,
	Isubsp,
	Ivargc,
	Ixcast,
	Ixor,
	Iopmax=Ixor
} ikind;

typedef
enum {
	Rsp = 0,
	Rfp,
	Rpc,
	Rac,
	Rcl,
} Reg;

enum {
	Oloc = 0,
	Okon,
	Oimm,
	Onil,
};

typedef
struct Ictx {
	u32 n;
} Ictx;

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
enum
{
	Lreg,
	Lparam,
	Llocal,
	Ldisp,
	Ltopl,
	Ltopr,
} Lkind;

typedef
struct Lambda
{
	char *id;
	unsigned isvarg;
	Var **cap;
	unsigned ncap;
	Var *disp;
	unsigned ndisp;
	Var *param;
	unsigned nparam;
	unsigned nloc;
	unsigned ntmp;
} Lambda;

typedef
struct Block
{
	Var *loc;
	unsigned nloc;
} Block;

typedef
struct Boxset
{
	Var **var;
	unsigned max, n;
} Boxset;

typedef
struct Location {
	unsigned loc;		/* access with LOC macros */
	Val v;			/* topl/topr */
} Location;

#define LOC(idx,box,kind)	(((idx)<<4)|((box&1)<<3)|((kind)&0x7))
#define LOCIDX(loc)		((loc)>>4)
#define LOCBOX(loc)		(((loc)>>3)&0x1)
#define LOCKIND(loc)		((loc)&0x7)

typedef
struct Operand {
	u8 okind;
	union{
		Location loc;
		Val kon;
		Imm imm;
	} u;
} Operand;

typedef
enum {
	Clabel,
	Clabelpair,
} Ctlkind;

struct Ctl {
	Ctlkind ckind;

	/* ckind == Clabel */
	char *label;		/* for humans, only; (duplicates in code ok) */
	int used;
	unsigned long insn;

	/* ckind == Clabelpair */
	Ctl *l1, *l2;

	Ctl *link;
	Code *code;
};

struct Insn {
	void *go;
	ikind kind;
	Operand op1, op2, dst;
	union{
		Insn *targ;
		Ctl *dstlabel;
		u64 cnt;
	};
};

struct Code {
	Head hd;
	unsigned long ninsn, maxinsn;;
	unsigned long nreloc, maxreloc;
	Insn *insn;
	Ctl **labels;
	Src *src;
	Ctl *clist;
	Tab *konst;
	uptr *reloc;
	u64 *lm;
	u32 nlm, mlm;
};

/* live mask */
#define mwbits	       (8*sizeof(u64))  /* # bits in mask word */

typedef
struct BFgeom {
	Imm bp, bs;
	Imm addr, cnt;
	unsigned isbe;
	int les, bes;
} BFgeom;

typedef
struct Err {
	jmp_buf esc;
	unsigned pdepth;	/* vm->pdepth when error label set */
	Imm sp, fp;
	Insn *pc;
	Closure *cl;
} Err;

typedef struct Xenv Xenv;
struct Xenv {
	HT *ht;
	Xenv *link;
};

typedef
struct Env {
	Tab *var;		/* variable bindings */
	HT *rd;			/* record descriptors */
} Env;

struct Toplevel {
	struct Env *env;
	Xfd in;
	Xfd out;
	Xfd err;
};

enum {
	VMirq = 1,
};

typedef struct Hashop {
	u32 (*hash)(Val);
	int (*eq)(Val, Val);
} Hashop;

struct VM {
	Imm sp, fp;
	Insn *pc;
	Closure *cl;
	Val ac;
	unsigned int flags;
	Toplevel *top;
	Val stack[Maxstk];
	Err *err;		/* stack of error labels */
	unsigned edepth, emax;	/* # live and max error labels */
	u64 exetime, exelast;
	u64 gctime, postgctime;
};

extern char* S[];
extern char* cbasename[];
extern unsigned isfloat[Vnbase];
extern unsigned isunsigned[Vnbase];
extern Imm repsize[Rnrep];
extern char* tkindstr[];
extern VM*   vms[];
extern Cval  *cvalnull, *cval0, *cval1, *cvalminus1;
extern char  **cqctloadpath;
extern char *qname[];
extern void *GCiterdone;
extern Dom *litdom;
extern Val Xundef;
extern Val Xnil;
extern Code *kcode, *cccode, *tcccode;

/* top-level roots */
extern Val syms;
extern Val typecache;

/* c.l */
void		freeyystate(YYstate *yy);
YYstate*	mkyystate(FILE *fp);
YYstate*	mkyystatestr(char *buf);
void		setyystate(YYstate *yy);

/* parse.c */
Expr*		G(char *s);
Expr*		copyexpr(Expr *e);
Expr*		doconst(U *ctx, char*, unsigned long len);
Expr*		dostr(char*, unsigned long len);
Expr*		doparse(U*, char *buf, char *whence);
Expr*		dosym(char *s, unsigned long len);
Expr*		dotickesrc(Ysrc *src, Expr*, Expr*);
Expr*		doticktsrc(Ysrc *src, Expr*, Expr*);
void		dotop(U*, Expr*);
void		finiparse(void);
Expr*		flatten(Expr *e);
void		initparse(void);
Expr*		invert(Expr*);
int		maybepopyy(U *ctx);
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

/* compilee.c */
Expr*		docompilee(U *ctx, Expr *e);

/* compileq.c */
Expr*		docompileq(U *ctx, Expr *e);

/* compilex.c */
Expr*		docompilex(U *ctx, Expr *e);

/* compilen.c */
Expr*		docompilen(U *ctx, Expr *e);

/* compilem.c */
Expr*		docompilem(U *ctx, Expr *e);

/* compilea.c */
Expr*		docompilea(U *ctx, Expr *e);

/* compile0.c */
Expr*		docompile0(U *ctx, Expr *e);

/* compileg.c */
Expr*		docompileg(U *ctx, Expr *e);

/* compilek.c */
Expr*		docompilek(U *ctx, Expr *e);

/* compilel.c */
Expr*		docompilel(U *ctx, Expr *e);

/* compilei.c */
Expr*		docompilei(U *ctx, Expr *e);

/* compile1.c */
Expr*		docompile1(U *ctx, Expr *e);

/* compileo.c */
Expr*		docompileo(U *ctx, Expr *e);

/* compileb.c */
Expr*		docompileb(U *ctx, Expr *e);

/* compileu.c */
Expr*		docompileu(U *ctx, Expr *e);

/* compilex.c */
Expr*		docompilex(U *ctx, Expr *e);

/* compilec.c */
Expr*		docompilec(U *ctx, Expr *e);

/* compiles.c */
Expr*		docompiles(U *ctx, Expr *e);

/* compilev.c */
Expr*		docompilev(U *ctx, Expr *el);
void		freexp(Expr *e);
int		issimple(Expr *e);

/* cg.c */
Closure*	callcc(void);
Code*		callccode(void);
Code*		calltccode(void);
void		cgstatistics();
Closure*	codegen(Expr *e);
Code*		contcode(void);
void		finicg(void);
Closure*	haltthunk(void);
void		initcg(void);
Closure*	panicthunk(void);
void		printkon(Val v);
void		resetlabels();

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
void		builtinfd(Env *env, char *name, Fd *fd);
void		builtinfn(Env *env, char *name, Closure *cl);
Str*		callget(VM *vm, As *as, Imm off, Imm len);
Cval*		callismapped(VM *vm, As *as, Imm off, Imm len);
Vec*		callmap(VM *vm, As *as);
void		callput(VM *vm, As *as, Imm off, Imm len, Str *s);
Ctype*		chasetype(Ctype *t);
void		checkarg(VM *vm, char *f, Val *argv,
			 unsigned arg, Qkind qkind);
Tab*		doinsncnt(void);
Cval*		domcast(VM *vm, Dom *dom, Cval *cv);
void		dogc(VM *vm, u32 g, u32 tg);
Val		dovm(VM* vm, Closure *cl, Imm argc, Val *argv);
void		envbind(Env *env, char *id, Val v);
int		envbinds(Env *env, Cid *id);
void		envdefine(Env *env, Cid *id, Val v);
Val		envget(Env *env, Cid *id);
Pair*		envgetkv(Env *env, Cid *id);
int		equalval(Val v1, Val v2);
int		eqval(Val v1, Val v2);
int		eqvval(Val v1, Val v2);
Val		expr2syntax(Expr *e);
void		freeenv(Env *env);
void		fvmbacktrace(VM *vm);
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
int		freecode(Head *hd);
void		freetoplevel(Toplevel *top);
void		heapfree(Head *p);
int		iscomplete(Ctype *t);
int		ismapped(VM *vm, As *as, Imm addr, Imm len);
int		isstrcval(Cval *cv);
Range*		mapstab(VM *vm, Vec *map, Imm addr, Imm len);
As*		mkastab(Tab *mtab, Str *name);
Closure*	mkcfn(char *id, Cfn *cfn);
Closure*	mkccl(char *id, Ccl *ccl, unsigned dlen, ...);
Closure*	mkxfn(Str *code);
Closure*	mkcl(Code *code, unsigned long entry, unsigned len, char *id);
Cval*		mkcval(Dom *dom, Ctype *type, Imm val);
Dom*		mkdom(Ns *ns, As *as, Str *name);
Fd*		mkfdfn(Str *name, int flags, Xfd *xfd);
Fd*		mkfdcl(Str *name, int flags,
		       Closure *read, Closure *write, Closure *close);
Cval*		mklitcval(Cbase base, Imm val);
Ns*		mknstab(Tab *mtab, Str *name);
Range*		mkrange(Cval *beg, Cval *len);
As*		mksas(Str *s);
Toplevel*	mktoplevel(Xfd *in, Xfd *out, Xfd *err);
Val		mkvalbox(Val boxed);
Val		mkvalcval(Dom *dom, Ctype *t, Imm imm);
Val		mkvalcval2(Cval *cv);
Val		mkvallitcval(Cbase base, Imm imm);
Val		mkvallitcvalenc(Cbase base, Enc v);
As*		mkzas(Imm len);
Code*		newcode(void);
void		nexterror(VM *vm) NORETURN;
void		poperror(VM *vm);
void		printvmac(VM *vm);
jmp_buf*	_pusherror(VM *vm);
Val		safedovm(VM* vm, Closure *cl, Imm argc, Val *argv);
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
Fd*		vmstdout(VM *vm);
Cval*		xcvalalu(VM *vm, ikind op, Cval *op1, Cval *op2);
#define mkvalas(x)	((Val)(x))
#define mkvalcid(x)	((Val)(x))
#define mkvalcl(x)	((Val)(x))
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
#define valcid(v)	((Cid*)(v))
#define valcl(v)	((Closure*)(v))
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
#define waserror(vm)	(setjmp(*(_pusherror(vm))))
#define stkimm(v)	(Imm)(uptr)(v)
#define stkp(v)		(void*)(uptr)(v)
#define FN(id)		builtinfn(env, "%"#id, cqctmkcfn(#id, l1_##id))

typedef int(*Freeheadfn)(Head*);
Freeheadfn getfreeheadfn(Qkind qkind);
void setfreeheadfn(Qkind qkind, Freeheadfn free1);

/* lib9's reliable, portable snprintf (x/lib9) */
int		snprint(char *buf, int len, char *fmt, ...);

/* fnfmt.c */
char*		fmtctypec(Ctype *t);
Str*		fmtctype(Ctype *t);
void		l1_sprintfa(VM *vm, Imm argc, Val *argv, Val *rv);

/* xfd.c */
void		cprintf(Xfd *xfd, char *fmt, ...);
void		cvprintf(Xfd *xfd, char *fmt, va_list args);
void		xfdclose(Xfd *xfd);
Imm		xfdread(Xfd *xfd, char *buf, Imm len);
Imm		xfdwrite(Xfd *xfd, char *buf, Imm len);

/* cutil.c */
void		cerror(U *ctx, Expr *e, char *fmt, ...) NORETURN;
void		cwarn(U *ctx, Expr *e, char *fmt, ...);
void		cwarnln(U *ctx, Expr *e, char *fmt, ...);
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
Expr*		Zcall(Expr *fn, unsigned narg, ...);
Expr*		Zcons(Expr *hd, Expr *tl);
Expr*		Zconst(Cbase base, Imm val);
Expr*		Zcval(Expr *dom, Expr *type, Expr *val);
Expr*		Zgbinop(unsigned gop, Expr *x, Expr *y);
Expr*		Zidcid(Cid *s);
Expr*		Zid2sym(Expr *e);
Expr*		Zids2syms(Expr *l);
Expr*		Zif(Expr *cond, Expr *true);
Expr*		Zifelse(Expr *cond, Expr *true, Expr *false);
Expr*		Zint(Imm val);
Expr*		Zgoto(char *l);
Expr*		Zgotosrc(Ysrc *src, Expr *id);
Expr*		Zkon(Val v);
Expr*		Zlabel(char *l);
Expr*		Zlabelsrc(Src src, Expr *id, Expr *s);
Expr*		Zlambda(Expr *args, Expr *body);
Expr*		Zlambdn(Expr *args, Expr *body, Expr *name);
Expr*		Zletrec(Expr *binds, Expr *body);
Expr*		Zlocals(unsigned n, ...);
Expr*		Znil(void);
Expr*		Znop(void);
Expr*		Znot(Expr *e);
Expr*		Znull();
Expr*		Zref(Expr *dom, Expr *type, Expr *val);
Expr*		Zret(Expr *e);
Expr*		Zscope(Expr *block);
Expr*		Zset(Expr *l, Expr *r);
Expr*		Zsizeof(Expr *e);
Expr*		Zstr(char *s);
Expr*		Zstrn(char *s, Imm len);
Expr*		Zsub(Expr *x, Expr *y);
Expr*		Ztg(char *id, Expr *e);
Expr*		Ztid(char *id);
Expr*		Zuint(Imm val);
int		hasvarg(Expr *e);
Expr*		Zvararg(Expr *id);
Expr*		Zxcast(Expr *type, Expr *cval);

/* mem.c */
void		compact(VM *vm);
void		finimem();
void		_gc(u32 g, u32 tg);
void		gc(VM *vm);
void		gcdisable();
void		gcenable();
void		gcpoll(VM *vm);
void*		gclock(void *v);
void		gcstatistics();
void		gcstats();
void*		gcunlock(void *v);
void		gcwb(Val v);
void		guard(Val o, Pair *g);
void		initmem();
void		instguard(Pair *p);
int		ismanagedrange(void *p, Imm len);
int		isweak(Head *h);
Head*		malq(Qkind kind, u32 sz);
Head*		malv(Qkind kind, Imm len);
Head*		malbox();
Head*		malcode();
Head*		malweak();
u64		meminuse();
Pair*		mkguard();
Head*		pop1guard(Pair *t);
Head*		pop1tguard(Pair *t);
u64		protected();
void		quard(Val o);
void		tguard(Val o, Pair *g);

/* ch.c */
void		fnch(Env *env);

/* cid.c */
void		finicid();
void		fncid(Env *env);
void		initcid();
Cid*		mkcid(char *s, Imm len);
Cid*		mkcid0(char *s);

/* ctype.c */
Ctype*		chasetype(Ctype *t);
int		equalctype(Ctype *a, Ctype *b);
int		eqvctype(Ctype *a, Ctype *b);
void		finitype();
void		fnctype(Env *env);
u32		hashctype(Ctype *t);
u32		hashqvctype(Ctype *t);
void		inittype();
Val*		iterctype(Head *hd, Ictx *ictx);
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
void		fncval(Env *env);

/* list.c */
int		equallist(List *a, List *b);
void		fnlist(Env *env);
u32		hashlist(List *l);
Val*		iterlist(Head *hd, Ictx *ictx);
void		l1_listref(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_listset(VM *vm, Imm argc, Val *argv, Val *rv);
void		_listappend(List *lst, Val v);
List*		listappend(VM *vm, List *lst, Val v);
List*		listconcat(VM *vm, List *l1, List *l2);
List*		listcopy(List *lst);
void		listcopyv(List *lst, Imm ndx, Imm n, Val *v);
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
void		fntab(Env *env);
Tab*		mktab(void);
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
void		fnpair(Env *env);
u32		hashpair(Pair *p);
Pair*		mkpair(Val a, Val d);
Pair*		mkweakpair(Val a, Val d);

/* rec.c */
int		equalrec(Rec *a, Rec *b);
u32		hashrec(Rec *r);
void		fnrec(Env *env);

/* str.c */
int		equalstr(Str *a, Str *b);
void		fnstr(Env *env);
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
Str*		strslice(Str *str, Imm beg, Imm end);
int		Strcmp(Str *s1, Str *s2);

/* stx.c */
int		equalstx(Expr *a, Expr *b);
int		eqvstx(Expr *a, Expr *b);
void		fnstx(Env *env);
u32		hashqvstx(Expr *e);
u32		hashstx(Expr *e);

/* sym.c */
Val		attroff(Val o);
Val		copyattr(Val attr, Val newoff);
Ctype*		fieldtype(Vec *s);
void		fnsym(Env *env);
int		issym(Vec *sym);
int		issymvec(Vec *v);
Val		mkattr(Val o);
Ctype*		paramtype(Vec *s);
Ctype*		symtype(Vec *s);

/* vec.c */
int		equalvec(Vec *a, Vec *b);
void		fnvec(Env *env);
u32		hashvec(Vec *v);
void		l1_vecref(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_vecset(VM *vm, Imm argc, Val *argv, Val *rv);
Vec*		mkvec(Imm len);
Vec*		mkvecinit(Imm len, Val v);
Vec*		veccopy(Vec *old);
Imm		veclen(Vec *vec);
Val		vecref(Vec *vec, Imm idx);
void		_vecset(Vec *vec, Imm idx, Val v);
void		vecset(Vec *vec, Imm idx, Val v);

/* c.c */
Val		compile(VM *vm, Expr *e, Toplevel *top, char *argsid);
Expr*		compilex(VM *vm, Expr *e, Toplevel *top, char *argsid);

/* cqct.c */
Expr*		cqctparse(char *s, Toplevel *top, char *src);

extern		void fns(Env*);

#endif /* _BISONFLAW_SYSCQCT_H_ */
