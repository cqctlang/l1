#ifndef _BISONFLAW_SYSCQCT_H_
#define _BISONFLAW_SYSCQCT_H_

#include "cqct.h"

enum{
	Maxliti		= 70,	/* longest integer literal (binary) plus null */
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
	Ebinop,
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
	Econst,
	Econsts,
	Econtainer,
	Econtinue,
	Edecl,			/* type specifier + 1 declarator */
	Edecls,			/* type specifier + list of declarators */
	Edefault,
	Edefconst,
	Edefine,
	Edefloc,
	Edefrec,
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
	Ege,
	Eglobal,
	Egop,
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
	Elor,
	Elt,
	Emod,
	Emul,
	Enames,
	Eneq,
	Enop,
	Enil,
	Enull,
	Epostdec,
	Epostinc,
	Epredec,
	Epreinc,
	Eptr,
	Equote,
	Eref,
	Eret,
	Escope,
	Eshl,
	Eshr,
	Esizeofe,
	Esizeoft,
	Estruct,
	Esub,
	Eswitch,
	Etab,
	Eticke,
	Etickt,
	Etid,
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
	E_cval,
	E_ref,
	E_sizeof,
	E_tid,
	E_tg,
	Emax,
	Ebad,
} Kind;

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
	Tbitfield,		/* xtn only */
	Tconst,			/* xtn only */
	Txaccess,		/* xtn only */
	Tundef,			/* xtn only */
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
typedef struct Expr Expr;

typedef
struct Src {
	char *filename;
	unsigned col, line;
} Src;

typedef
struct Lits {
	char *s;
	unsigned len;
} Lits;

typedef
struct Liti {
	Imm val;
	Cbase base;
} Liti;

struct Expr {
	Head hd;
	Kind kind;
	Expr *e1;
	Expr *e2;
	Expr *e3;
	Expr *e4;

	union{
		char *id;		/* Eid, Etick, Etickt */
		Lits *lits;		/* Econsts */
		Liti liti;		/* Econst */
		Kind op;		/* Ebinop, Egop */
	};

	char attr;			/* disambiguation attribute */

	/* source */
	Src src;

	/* compiler-managed fields */
	void *xp;
};

typedef struct As As;
typedef struct Box Box;
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
typedef struct Xtypename Xtypename;

typedef void (Cfn)(VM *vm, Imm argc, Val *argv, Val *rv);
typedef void (Ccl)(VM *vm, Imm argc, Val *argv, Val *disp, Val *rv);

struct Cval {
	Head hd;
	Dom *dom;
	Xtypename *type;
	Imm val;
};

struct Closure {
	Head hd;
	Code *code;
	unsigned long entry;
	unsigned dlen;
	Val *display;
	char *id;
	Imm fp;			/* of continuation, always >0 */
	Cfn *cfn;
	Ccl *ccl;
	Str *xfn;
};

struct Box {
	Head hd;
	Val v;
};

struct Tab {
	Head hd;
	u32 sz, nent;
	Vec *ht;
	Pair *tg;		/* transport guardian */
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
	Xtypename *base[Vnallbase];
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

struct Xtypename {
	Head hd;
	unsigned char flag;	/* Tincomplete/Tcomplete */
	Tkind tkind;		/* = Tbase, Tstruct, ... */
	Cbase basename;		/* base (FIXME: rename cbase) */
	Rkind rep;		/* base, ptr, enum; = Ru08le ... */
	Str *tid;		/* typedef */
	Str *tag;		/* struct, union, enum */
	Val cnt;		/* arr */
	Val attr;		/* struct, union, bitfield size and attrs */
	Val bit0;		/* bitfield */
	Xtypename *link;	/* ptr, arr, func, bitfield, enum,
				   const, xaccess */
	Vec *field;		/* struct, union */
	Vec *param;		/* func */
	Vec *konst;		/* enum (constants) */
	Closure *get;		/* xaccess */
	Closure *put;		/* xaccess */
};

enum {
	MaxIn	= 128,
};

typedef struct YYstate YYstate;

typedef
struct In {
	struct Src src;
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
	Icval,
	Idiv,
	Iframe,
	Ihalt,
	Iinv,
	Ijmp,
	Ijnz,
	Ijz,
	Ikg,
	Ikp,
	Ilist,
	Ilive,
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
	Iref,
	Iret,
	Ishl,
	Ishr,
	Isizeof,
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
	Vtop,
	Vdisp,
} Vwhere;

typedef
struct Var
{
	char *id;
	Vwhere where;
	unsigned idx;
	unsigned box;
	Val *val;		/* Vtop */
} Var;

typedef
enum
{
	Lreg,
	Lparam,
	Llocal,
	Ldisp,
	Ltopl,
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
	Var *var;		/* Ltopl */
	unsigned loc;		/* access with LOC macros */
} Location;

#define LOC(idx,box,kind)	(((idx)<<4)|((box&1)<<3)|((kind)&0x7))
#define LOCIDX(loc)		((loc)>>4)
#define LOCBOX(loc)		(((loc)>>3)&0x1)
#define LOCKIND(loc)		((loc)&0x7)

typedef
struct Operand {
	unsigned okind;
	union{
		Location loc;
		Val kon;
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
	Src *src;
	Code *code;
};

typedef
struct Insn {
	ikind kind;
	void *go;
	Operand op1, op2, op3, dst;
	union{
		Ctl *dstlabel;
		u64 cnt;
	};
	Src *src;
	unsigned long ox;
} Insn;

struct Code {
	Head hd;
	unsigned long ninsn;
	unsigned long maxinsn;
	unsigned long nreloc, maxreloc;
	Insn *insn;
	Ctl **labels;
	Ctl *clist;
	Expr *src;
	Tab *konst;
	unsigned char *x;
	unsigned long nx;
	unsigned long maxx;
	uptr *reloc;
};

typedef
struct BFgeom {
	Imm bp, bs;
	Imm addr, cnt;
	unsigned isbe;
	int les, bes;
} BFgeom;

typedef struct Root Root;
struct Root {
	Head *hd;
	Root *link;
};

typedef
struct Rootset {
	Root *roots;
	Root *last;
	Root *before_last;
	Root *this;
	Root *free;
} Rootset;

typedef
struct Err {
	jmp_buf esc;
	unsigned pdepth;	/* vm->pdepth when error label set */
	Imm sp, fp, pc;
	Val cl;
} Err;

typedef struct Xenv Xenv;
struct Xenv {
	HT *ht;
	Xenv *link;
};

typedef
struct Env {
	HT *var;		/* variable bindings */
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
	Imm sp, fp, pc;
	Val ac, cl;
	Closure *clx;
	unsigned int flags;
	Toplevel *top;
	Insn *ibuf;
	Val stack[Maxstk];
	//Dom *litdom;
	//Ns *litns;
	//Xtypename **litbase;	/* always points to litns->base */
	Err *err;		/* stack of error labels */
	unsigned edepth, emax;	/* # live and max error labels */
};

extern char* S[];
extern char* cbasename[];
extern unsigned isunsigned[Vnbase];
extern Hashop hashop[];
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

/* c.l */
void		freeyystate(YYstate *yy);
YYstate*	mkyystate(FILE *fp);
YYstate*	mkyystatestr(char *buf);
void		setyystate(YYstate *yy);

/* parse.c */
Expr*		G(char *s);
Expr*		copyexpr(Expr *e);
Lits*		copylits(Lits *lits);
Expr*		doconst(U *ctx, char*, unsigned long len);
Expr*		doconstssrc(Src*, char*, unsigned long len);
Expr*		doparse(U*, char *buf, char *whence);
Expr*		dosymsrc(Src *src, char *s, unsigned long len);
Expr*		dotickesrc(Src *src, Expr*, Expr*);
Expr*		doticktsrc(Src *src, Expr*, Expr*);
void		dotop(U*, Expr*);
void		finiparse(void);
Expr*		flatten(Expr *e);
void		freelits(Lits *lits);
#define		freeexpr(e)
void		initparse(void);
Expr*		invert(Expr*);
int		maybepopyy(U *ctx);
Expr*		mkconst(Cbase base, Imm val); /* rename newconst? */
Lits*		mklits(char*, unsigned len);
Expr*		newbinop(unsigned, Expr*, Expr*);
Expr*		newbinopsrc(Src*, unsigned, Expr*, Expr*);
Expr*		newexpr(unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newexprsrc(Src*, unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newgopsrc(Src*, unsigned, Expr*, Expr*);
Expr*		nullelist(void);
Expr*		nullelistsrc(Src*);
int		parseliti(char *s, unsigned long len, Liti *liti,
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
Kind		s2kind(Str *s);

/* bitfield.c */
int		bitfieldgeom(BFgeom *bfg);
Imm		bitfieldget(char *s, BFgeom *bfg);
Imm		bitfieldput(char *s, BFgeom *bfg, Imm val);

/* compileq.c */
Expr*		docompileq(U *ctx, Expr *e);

/* compilen.c */
Expr*		docompilen(U *ctx, Expr *e);

/* compilea.c */
int		docompilea(U *ctx, Expr *e);

/* compile0.c */
int		docompile0(U *ctx, Expr *e);

/* compileg.c */
int		docompileg(U *ctx, Expr *e);

/* compilel.c */
int		docompilel(U *ctx, Expr *e);

/* compilei.c */
int		docompilei(U *ctx, Expr *e);

/* compile1.c */
int		docompile1(U *ctx, Expr *e);

/* compile2.c */
Expr*		docompile2(U *ctx, Expr *el, Toplevel *top, char *argsid);

/* compileb.c */
Expr*		docompileb(U *ctx, Expr *e, Toplevel *top, char *argsid);

/* compileu.c */
Expr*		docompileu(U *ctx, Expr *e);

/* compilex.c */
Expr*		docompilex(U *ctx, Expr *e);

/* compilec.c */
Expr*		docompilec(U *ctx, Expr *e);

/* compiles.c */
Expr*		docompiles(U *ctx, Expr *e);

/* compilev.c */
Expr*		docompilev(U *ctx, Expr *el, Toplevel *top);
void		freeconst(void *u, char *id, void *v);
void		freeexprx(Expr *e);
int		issimple(Expr *e);

/* cg.c */
Closure*	callcc(void);
Code*		callccode(void);
Code*		calltccode(void);
Closure*	codegen(Expr *e);
Code*		contcode(void);
void		finicg(void);
Closure*	haltthunk(void);
void		initcg(void);
Closure*	nopthunk(void);
Closure*	panicthunk(void);
void		printinsn(Insn *i);
void		printkon(Val v);
void		resetlabels();

/* xenv.c */
void		freexenv(Xenv *xe);
void		freexenvall(Xenv *xe);
Xenv*		mkxenv(Xenv *link);
void		xenvapply(Xenv *xe, HT *d);
Xenv*		xenvcopy(Xenv *xe);
HT*		xenvdiff(Xenv *fr, Xenv *to);
void*		xenvlook(Xenv *xe, char *id);
void		xenvbind(Xenv *xe, char *id, void *v);
void		xenvforeach(Xenv *xe, void (*f)(void *u, char *k, void *v),
			    void *u);
unsigned long	xenvsize(Xenv *xe);
void		xenvupdate(Xenv *xe, char *id, void *v);

/* vm.c */
Src*		addr2line(Code *code, Imm pc);
Val		attroff(Val o);
void		builtinfd(Env *env, char *name, Fd *fd);
void		builtinfn(Env *env, char *name, Closure *cl);
Str*		callget(VM *vm, As *as, Imm off, Imm len);
Cval*		callismapped(VM *vm, As *as, Imm off, Imm len);
Vec*		callmap(VM *vm, As *as);
void		callput(VM *vm, As *as, Imm off, Imm len, Str *s);
Xtypename*	chasetype(Xtypename *xtn);
void		checkarg(VM *vm, char *f, Val *argv,
			 unsigned arg, Qkind qkind);
Tab*		doinsncnt(void);
Cval*		domcast(VM *vm, Dom *dom, Cval *cv);
void		dogc(VM *vm, u32 g, u32 tg);
Val		dovm(VM* vm, Closure *cl, Imm argc, Val *argv);
int		envbinds(Env *env, char *id);
Val*		envget(Env *env, char *id);
Val*		envgetbind(Env *env, char *id);
int		eqval(Val v1, Val v2);
Val		expr2syntax(Expr *e);
void		freeenv(Env *env);
void		fvmbacktrace(VM *vm);
Str*		getbytes(VM *vm, Cval *addr, Imm n);
u32		hashval(Val v);
void		initvm(int gcthread, u64 heapmax);
int		isbasecval(Cval *cv);
int		isnatcval(Cval *cv);
int		isnegcval(Cval *cv);
int		iszerocval(Cval *cv);
void		finivm(void);
int		freecode(Head *hd);
void		freetoplevel(Toplevel *top);
void		heapfree(Head *p);
int		iscomplete(Xtypename *t);
int		ismapped(VM *vm, As *as, Imm addr, Imm len);
int		isstrcval(Cval *cv);
Range*		mapstab(VM *vm, Vec *map, Imm addr, Imm len);
As*		mkastab(Tab *mtab, Str *name);
Val		mkattr(Val o);
Closure*	mkcfn(char *id, Cfn *cfn);
Closure*	mktcfn(char *id, Cfn *cfn);
Closure*	mkccl(char *id, Ccl *ccl, unsigned dlen, ...);
Closure*	mkxfn(Str *code);
Closure*	mkcl(Code *code, unsigned long entry, unsigned len, char *id);
Cval*		mkcval(Dom *dom, Xtypename *type, Imm val);
Dom*		mkdom(Ns *ns, As *as, Str *name);
Fd*		mkfdfn(Str *name, int flags, Xfd *xfd);
Fd*		mkfdcl(Str *name, int flags,
		       Closure *read, Closure *write, Closure *close);
Cval*		mklitcval(Cbase base, Imm val);
Ns*		mknstab(Tab *mtab, Str *name);
Xtypename*	mkptrxtn(Xtypename *t, Rkind rep);
Range*		mkrange(Cval *beg, Cval *len);
As*		mksas(Str *s);
Toplevel*	mktoplevel(Xfd *in, Xfd *out, Xfd *err);
Val		mkvalbox(Val boxed);
Val		mkvalcval(Dom *dom, Xtypename *t, Imm imm);
Val		mkvalcval2(Cval *cv);
Val		mkvallitcval(Cbase base, Imm imm);
Xtypename*	mkvoidxtn(void);
As*		mkzas(Imm len);
Code*		newcode(void);
void		nexterror(VM *vm) NORETURN;
void		poperror(VM *vm);
void		printvmac(VM *vm);
jmp_buf*	_pusherror(VM *vm);
Val		safedovm(VM* vm, Closure *cl, Imm argc, Val *argv);
u32		shash(char *s, Imm len);
Imm		stkimm(Val v);
Str*		stringof(VM *vm, Cval *cv);
Cval*		typecast(VM *vm, Xtypename *xtn, Cval *cv);
Val		valboxed(Val v);
Head*		valhead(Val v);
Imm		valimm(Val v);
Str*		valstrorcval(VM *vm, char *fn, Val *argv, unsigned arg);
void		vmerr(VM *vm, char *fmt, ...) NORETURN;
Fd*		vmstdout(VM *vm);
Cval*		xcvalalu(VM *vm, ikind op, Cval *op1, Cval *op2);
#define mkvalas(x)	((Val)(x))
#define mkvalcl(x)	((Val)(x))
#define mkvaldom(x)	((Val)(x))
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
#define mkvalxtn(x)	((Val)(x))

#define valas(v)	((As*)(v))
#define valcval(v)	((Cval*)(v))
#define valcl(v)	((Closure*)(v))
#define valdom(v)	((Dom*)(v))
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
#define valxtn(v)	((Xtypename*)(v))
#define valboxedcval(b)	((Cval*)((Box*)(b))->v)
#define waserror(vm) (setjmp(*(_pusherror(vm))))
#define FN(name) builtinfn(env, "%"#name, mkcfn(#name, l1_##name))
#define FNT(name) builtinfn(env, "%"#name, mktcfn(#name, l1_##name))

typedef int(*Freeheadfn)(Head*);
Freeheadfn getfreeheadfn(Qkind qkind);
void setfreeheadfn(Qkind qkind, Freeheadfn free1);

/* lib9's reliable, portable snprintf (x/lib9) */
int		snprint(char *buf, int len, char *fmt, ...);

/* fnfmt.c */
char*		fmtxtnc(Xtypename *xtn);
Str*		fmtxtn(Xtypename *xtn);
void		l1_sprintfa(VM *vm, Imm argc, Val *argv, Val *rv);

/* xfd.c */
void		cprintf(Xfd *xfd, char *fmt, ...);
void		cvprintf(Xfd *xfd, char *fmt, va_list args);
void		xfdclose(Xfd *xfd);
Imm		xfdread(Xfd *xfd, char *buf, Imm len);
Imm		xfdwrite(Xfd *xfd, char *buf, Imm len);

/* cutil.c */
void		cerror(U *ctx, Expr *e, char *fmt, ...) NORETURN;
void		cposterror(U *ctx, Expr *e, char *fmt, ...);
void		cwarn(U *ctx, Expr *e, char *fmt, ...);
void		cwarnln(U *ctx, Expr *e, char *fmt, ...);
Expr*		doid(char*);
Expr*		doidnsrc(Src *src, char *s, unsigned long len);
unsigned	elistlen(Expr *l);
char*		idsym(Expr *e);
int		isnull(Expr *e);
void		putsrc(Expr *e, Src *src);
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
Expr*		Zconsts(char *s);
Expr*		Zcval(Expr *dom, Expr *type, Expr *val);
Expr*		Zids2strs(Expr *l);
Expr*		Zif(Expr *cond, Expr *true);
Expr*		Zifelse(Expr *cond, Expr *true, Expr *false);
Expr*		Zint(Imm val);
Expr*		Zgoto(char *l);
Expr*		Zgoton(char *l, unsigned long len);
Expr*		Zgotosrc(Src *src, Expr *id);
Expr*		Zkon(Val v);
Expr*		Zlabel(char *l);
Expr*		Zlabeln(char *l, unsigned long len);
Expr*		Zlabelsrc(Src *src, Expr *id, Expr *s);
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
Expr*		Zstrn(char *s, unsigned long len);
Expr*		Zsub(Expr *x, Expr *y);
Expr*		Ztg(char *id, Expr *e);
Expr*		Ztgn(char *id, unsigned long len, Expr *e);
Expr*		Ztid(char *id);
Expr*		Ztidn(char *id, unsigned long len);
Expr*		Zuint(Imm val);
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
void		gcstats();
void*		gcunlock(void *v);
void		gcwb(Val v);
void		guard(Val o, Pair *g);
void		initmem();
void		instguard(Pair *p);
int		ismanagedrange(void *p, Imm len);
int		isweak(Head *h);
Head*		malq(Qkind kind);
Head*		malv(Qkind kind, Imm len);
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

/* list.c */
int		equallist(List *a, List *b);
void		fnlist(Env *env);
u32		hashlist(Val);
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
Val		listref(VM *vm, List *lst, Imm idx);
List*		listset(VM *vm, List *lst, Imm idx, Val v);
List*		mklist(void);
List*		mklistn(Imm sz);

/* tab.c */
void		fntab(Env *env);
Tab*		mktab(void);
int		islink(Val v);
void		l1_tabinsert(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_tablook(VM *vm, Imm argc, Val *argv, Val *rv);
Val		linkkey(Pair *lnk);
Val		linkval(Pair *lnk);
Val		linknext(Pair *lnk);
Tab*		tabcopy(Tab *tab);
void		tabdel(Tab *tab, Val keyv);
Vec*		tabenum(Tab *tab);
Vec*		tabenumkeys(Tab *tab);
Vec*		tabenumvals(Tab *tab);
Val		tabget(Tab *tab, Val keyv);
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
void		fnpair(Env *env);
Pair*		mkpair(Val a, Val d);
Pair*		mkweakpair(Val a, Val d);

/* rec.c */
void		fnrec(Env *env);

/* str.c */
int		equalstr(Str *a, Str *b);
void		fnstr(Env *env);
u32		hashstr(Val);
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

/* vec.c */
int		equalvec(Vec *a, Vec *b);
void		fnvec(Env *env);
u32		hashvec(Val v);
void		l1_vecref(VM *vm, Imm argc, Val *argv, Val *rv);
void		l1_vecset(VM *vm, Imm argc, Val *argv, Val *rv);
Vec*		mkvec(Imm len);
Vec*		mkvecinit(Imm len, Val v);
Vec*		veccopy(Vec *old);
Val		vecref(Vec *vec, Imm idx);
void		_vecset(Vec *vec, Imm idx, Val v);
void		vecset(Vec *vec, Imm idx, Val v);

/* cqct.c */
Val		cqctcompile0(Expr *e, Toplevel *top, char *argsid);
Expr*		cqctcompilex(Expr *e, Toplevel *top, char *argsid);
Expr*		cqctparse(char *s, Toplevel *top, char *src);

extern		void fns(Env*);

#endif /* _BISONFLAW_SYSCQCT_H_ */
