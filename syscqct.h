#ifndef _BISONFLAW_SYSCQCT_H_
#define _BISONFLAW_SYSCQCT_H_

#include "cqct.h"

enum{
	Vintmax		= 2147483647ULL,
	Vuintmax	= 4294967295ULL,
	Vlongmax	= 2147483647ULL,
	Vulongmax	= 4294967295ULL,
	Vvlongmax	= 9223372036854775807ULL,
	Vuvlongmax	= 18446744073709551615ULL,
	Maxliti		= 24,	/* longest integer literal plus null */
	Maxspec		= 32,
};

typedef
enum{
	Echar=0,
	Edouble,
	Efloat,
	Eint,	
	Elong,
	Eshort,
	Esigned,
	Eunsigned,
	Evoid,
	Enbase,

	Eadd,			/* 10 */
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
	Edefrec,
	Ederef,
	Ediv,
	Edo,
	Edot,
	Edotdot,
	Eelist,
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
	Egt,
	Eid,
	Eif,
	Ekon,
	Elambda,
	Eland,
	Ele,
	Elist,
	Elor,
	Elt,
	Emod,
	Emul,
	Ens,
	Eneq,
	Enop,
	Enil,
	Enull,
	Epostdec,
	Epostinc,
	Epredec,
	Epreinc,
	Eptr,
	Eref,
	Eret,
	Eshl,
	Eshr,
	Esizeofe,
	Esizeoft,
	Estruct,
	Esub,
	Eswitch,
	Etab,
	Etick,
	Etid,
	Etypedef,
	Etypeofe,
	Etypeoft,
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
	Emax,
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
	EmptyDecl = ~0,
};

typedef struct Decl Decl;
typedef struct Enum Enum;
typedef struct Type Type;
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
	Kind kind;
	Expr *e1;
	Expr *e2;
	Expr *e3;
	Expr *e4;

	union{
		char *id;		/* Eid, Etick */
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

struct Enum {
	char *id;
	Expr *val;
	Enum *link;
};

struct Decl {
	Type *type;
	char *id;
	Expr *attr;		/* offset and other attributes */
	Decl *link;
};

struct Type {
	unsigned kind;
	unsigned base;		/* base, enum? */
	char *tid;		/* typedef */
	char *tag;		/* struct, union, enum */
	char *dom;		/* optional domain qualifier for any Type */
	Decl *field;		/* struct, union */
	Enum *en;		/* enum */
	Expr *sz;		/* struct, union */
	Expr *bitw;		/* bitfield width */
	Decl *param;		/* func */
	Expr *cnt;		/* arr */
	Type *link;		/* typedef, ptr, arr, func(return type) */
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
};

struct Box {
	Head hd;
	Val v;
};

typedef struct Tabidx Tabidx;
struct Tabidx {
	u32 idx;
	Tabidx *link;
};

typedef
struct Tabx {
	u32 nxt, lim;
	u32 sz;
	Val *key;
	Val *val;
	Tabidx **idx;
} Tabx;

struct Tab {
	Head hd;
	u32 cnt;		/* key/val pairs stored */
	Tabx *x;		/* current storage, atomically swappable */
};

typedef
struct Listx {
	/* invariants:
	 *  if(hd == tl)
	 *  	list is empty.
	 *  else
	 *  	tl>hd and list has tl-hd elements,
	 *      hd points to first element in list, and
	 *	tl-1 points to last.
	 */
	u32 hd, tl, sz;
	Val *val;		/* atomically swappable */
	Val *oval;		/* buffer for gc-safe sliding */
} Listx;

struct List {
	Head hd;
	Listx *x;		/* current storage, atomically swappable */
};

struct As {
	Head hd;

	Tab *mtab;
	Str *name;

	/* cached methods */
	Closure *get;
	Closure *put;
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
	Imm nf;
	Rd *rd;
	Val *field;
};

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
	Smalloc,		/* free with free() */
	Smmap,			/* free with munmap() */
} Skind;

struct Str {
	Head hd;
	Skind skind;
	u64 len;
	size_t mlen;		/* Smmap size */
	char *s;
};

struct Vec {
	Head hd;
	Imm len;
	Val *vec;
};

struct Xtypename {
	Head hd;
	Tkind tkind;		/* = Tbase, Tstruct, ... */
	Cbase basename;		/* base (FIXME: rename cbase) */
	Rkind rep;		/* base, ptr, enum; = Ru08le ... */
	Str *tid;		/* typedef */
	Str *tag;		/* struct, union, enum */
	Val cnt;		/* arr */
	Val sz;			/* struct, union, bitfield */
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
	Xfd *xfd;
} U;

typedef
enum {
	Iadd = 0,
	Iand,
	Iargc,
	Ibin,
	Ibox,
	Ibox0,
	Icall,
	Icallc,
	Icallt,
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
	Ispec,
	Isub,
	Ixcast,
	Ixor,
	Iopmax         
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
	u64 n;
	void *x;
} Ictx;

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
	Ctl *dstlabel;
	u64 cnt;
	Src *src;
} Insn;

struct Code {
	Head hd;
	unsigned long ninsn;
	unsigned long maxinsn;
	unsigned long nconst;
	unsigned long nspec;
	Insn *insn;
	Ctl **labels;
	Ctl *clist;
	Expr *src;
	Tab *konst;
	Expr *spec[Maxspec];
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
	Xenv *con;		/* @const constants */
} Env;

struct Toplevel {
	struct Env *env;
	Xfd xfd;
};

struct VM {
	Imm sp, fp, pc;
	Val ac, cl;
	Closure *clx;
	Toplevel *top;
	Insn *ibuf;
	Val stack[Maxstk];
	Dom *litdom;
	Ns *litns;
	Xtypename **litbase;	/* always points to litns->base */
	Fd *stdout, *stdin;
	Root **prot;		/* stack of lists of GC-protected objects */
	Rootset rs;		/* Root free list for prot */
	unsigned pdepth, pmax;	/* # live and max prot lists  */
	Closure *halt;
	Err *err;		/* stack of error labels */
	unsigned edepth, emax;	/* # live and max error labels */
	Tab *prof;
};

extern char* S[];
extern char* cbasename[];
extern unsigned isunsigned[Vnbase];
extern char* tkindstr[];
extern VM*   vms[];
extern Cval  *cvalnull, *cval0, *cval1, *cvalminus1;
extern char  **cqctloadpath;
extern char *qname[];

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
Expr*		doconsts(char*, unsigned long len);
Expr*		doconstssrc(Src*, char*, unsigned long len);
Expr*		doid(char*);
Expr*		doidn(char *s, unsigned long len);
Expr*		doidnsrc(Src *src, char *s, unsigned long len);
Expr*		doparse(U*, char *buf, char *whence);
Expr*		dotick(Expr*, Expr*);
Expr*		doticksrc(Src *src, Expr*, Expr*);
void		dotop(U*, Expr*);
Expr*		dotypes(U*, Expr*);
void		finiparse();
Expr*		flatten(Expr *e);
char*		fmttype(Type *t, char *o);
void		freeenum(Enum *en);
void		freeexpr(Expr*);
void		freelits(Lits *lits);
void		initparse();
Expr*		invert(Expr*);
Expr*		mkconst(Cbase base, Imm val); /* rename newconst? */
Lits*		mklits(char*, unsigned len);
Expr*		newbinop(unsigned, Expr*, Expr*);
Expr*		newbinopsrc(Src*, unsigned, Expr*, Expr*);
Expr*		newexpr(unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newexprsrc(Src*, unsigned, Expr*, Expr*, Expr*, Expr*);
Expr*		newgop(unsigned, Expr*, Expr*);
Expr*		newgopsrc(Src*, unsigned, Expr*, Expr*);
Expr*		nullelist();
int		parseliti(char *s, unsigned long len, Liti *liti,
			  unsigned radix, char **err);
Expr*		ptrto(Expr*, Expr*);
void		parseerror(U *ctx, char *fmt, ...);
int		popyy(U *ctx);
u64		szenum(Enum *en);
u64		szexpr(Expr *e);
void		tryinclude(U *ctx, char *raw);
int		yyparse(U *ctx);

/* printexpr.c */
void		printcqct(Expr*);
void		printexpr(Expr*);
void		printdecl(Decl *d);
void		printvars(Expr *e);

/* bitfield.c */
int		bitfieldgeom(BFgeom *bfg);
Imm		bitfieldget(char *s, BFgeom *bfg);
Imm		bitfieldput(char *s, BFgeom *bfg, Imm val);

/* type.c */
Expr*		gentypename(Type *t, Expr *(recpass)(U*, Expr*), U *ctx, unsigned effect);

/* compilens.c */
int		docompilens(U *ctx, Expr *e);

/* compilea.c */
int		docompilea(U *ctx, Expr *e);

/* compile0.c */
int		docompile0(U *ctx, Expr *e);

/* compile1.c */
int		docompile1(U *ctx, Expr *e);

/* compile2.c */
Expr*		docompile2(U *ctx, Expr *el, Toplevel *top, char *argsid);

/* compilev.c */
Expr*		docompilev(Expr *el, Toplevel *top);
void		freeconst(void *u, char *id, void *v);
void		freeexprx(Expr *e);
int		issimple(Expr *e);
u64		szexprx(Expr *e); 

/* cg.c */
Closure*	callcc();
Code*		callccode();
void		cgspec(VM *vm, Closure *orig, Imm idx, Val ac);
Closure*	codegen(Expr *e);
Code*		contcode();
void		finicg();
Closure*	haltthunk();
void		initcg();
Closure*	panicthunk();
void		printinsn(Code *code, Insn *i);
void		printkon(Val v);

/* spec.c */
Expr*		residue(VM *vm, Expr *e, Expr *pat, Val v);

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
Vec*		callmap(VM *vm, As *as);
void		callput(VM *vm, As *as, Imm off, Imm len, Str *s);
Xtypename*	chasetype(Xtypename *xtn);
void		checkarg(VM *vm, char *f, Val *argv,
			 unsigned arg, Qkind qkind);
Tab*		doinsncnt(VM *vm);
Cval*		domcast(VM *vm, Dom *dom, Cval *cv);
Val		dovm(VM* vm, Closure *cl, Imm argc, Val *argv);
int		envbinds(Env *env, char *id);
Val*		envget(Env *env, char *id);
Val*		envgetbind(Env *env, char *id);
int		eqval(Val v1, Val v2);
void		freeenv(Env *env);
void*		gcpersist(VM *vm, void *hd);
void*		gcprotect(VM *vm, void *hd);
void		gcenable(VM *vm);
void		gcdisable(VM *vm);
void		gcunpersist(VM *vm, void *hd);
void		gcunprotect(VM *vm, void *hd);
Str*		getbytes(VM *vm, Cval *addr, Imm n);
void		initvm(int gcthread, u64 heapmax);
int		isbasecval(Cval *cv);
int		isnatcval(Cval *cv);
int		isnegcval(Cval *cv);
int		iszerocval(Cval *cv);
void		finivm();
int		freecode(Head *hd);
void		freetoplevel(Toplevel *top);
void		heapfree(Head *p);
int		isstrcval(Cval *cv);
List*		listappend(VM *vm, List *lst, Val v);
void		_listappend(List *lst, Val v);
List*		listpush(VM *vm, List *lst, Val v);
u32		listxlen(Listx *x);
Range*		mapstab(VM *vm, Vec *map, Imm addr, Imm len);
As*		mkastab(Tab *mtab, Str *name);
Val		mkattr(Val o);
Closure*	mkcfn(char *id, Cfn *cfn);
Closure*	mkccl(char *id, Ccl *ccl, unsigned dlen, ...);
Closure*	mkcl(Code *code, unsigned long entry, unsigned len, char *id);
Cval*		mkcval(Dom *dom, Xtypename *type, Imm val);
Fd*		mkfdfn(Str *name, int flags, Xfd *xfd);
Fd*		mkfdcl(Str *name, int flags,
		       Closure *read, Closure *write, Closure *close);
List*		mklist();
Cval*		mklitcval(Cbase base, Imm val);
Range*		mkrange(Cval *beg, Cval *len);
As*		mksas(Str *s);
Str*		mkstr(char *s, Imm len);
Str*		mkstrlits(Lits *lits);
Str*		mkstr0(char *s);
Str*		mkstrk(char *s, Imm len, Skind skind);
Str*		mkstrn(VM *vm, Imm len);
Tab*		mktab();
Toplevel*	mktoplevel(Xfd *xfd);
Val		mkvalas(As *as);
Val		mkvalbox(Val boxed);
Val		mkvalcl(Closure *cl);
Val		mkvalcval(Dom *dom, Xtypename *t, Imm imm);
Val		mkvalcval2(Cval *cv);
Val		mkvaldom(Dom *dom);
Val		mkvalfd(Fd *fd);
Val		mkvalimm(Dom *dom, Xtypename *t, Imm imm);
Val		mkvallist(List *lst);
Val		mkvallitcval(Cbase base, Imm imm);
Val		mkvalns(Ns *ns);
Val		mkvalpair(Val car, Val cdr);
Val		mkvalrange(Cval *beg, Cval *len);
Val		mkvalrange2(Range *r);
Val		mkvalrd(Rd *rd);
Val		mkvalrec(Rec *rec);
Val		mkvalstr(Str *str);
Val		mkvaltab(Tab *tab);
Val		mkvalvec(Vec *vec);
Val		mkvalxtn(Xtypename *xtn);
Vec*		mkvec(Imm len);
As*		mkzas(VM *vm, Imm len);
Code*		newcode();
void		nexterror(VM *vm) NORETURN;
void		poperror(VM *vm);
void		printvmac(VM *vm);
jmp_buf*	_pusherror(VM *vm);
void		setattroff(VM *vm, Val o, Val v);
char*		str2cstr(Str *str);
Str*		stringof(VM *vm, Cval *cv);
Str*		strslice(Str *str, Imm beg, Imm end);
u64		szcode(Code *code);
Val		tabget(Tab *tab, Val keyv);
void		_tabput(Tab *tab, Val keyv, Val val);
void		tabput(VM *vm, Tab *tab, Val keyv, Val val);
Cval*		typecast(VM *vm, Xtypename *xtn, Cval *cv);
Val		valboxed(Val v);
Head*		valhead(Val v);
Imm		valimm(Val v);
Val		vecref(Vec *vec, Imm idx);
void		_vecset(Vec *vec, Imm idx, Val v);
void		vecset(VM *vm, Vec *vec, Imm idx, Val v);
void		vmerr(VM *vm, char *fmt, ...) NORETURN;
Fd*		vmstdout(VM *vm);
Cval*		xcvalalu(VM *vm, ikind op, Cval *op1, Cval *op2);
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

/* cutil.c */
void		cerror(U *ctx, Expr *e, char *fmt, ...) NORETURN;
void		cwarn(U *ctx, Expr *e, char *fmt, ...);
void		putsrc(Expr *e, Src *src);
Expr*		Zadd(Expr *x, Expr *y);
Expr*		Zapply(Expr *fn, Expr *args);
Expr*		Zargs(unsigned n, ...);
Expr*		Zbinop(unsigned op, Expr *x, Expr *y);
Expr*		Zblock(Expr *locs, ...);
Expr*		Zcall(Expr *fn, unsigned narg, ...);
Expr*		Zcons(Expr *hd, Expr *tl);
Expr*		Zconsts(char *s);
Expr*		Zcval(Expr *dom, Expr *type, Expr *val);
Expr*		Zids2strs(Expr *l);
Expr*		Zif(Expr *cond, Expr *true);
Expr*		Zifelse(Expr *cond, Expr *true, Expr *false);
Expr*		Zkon(Val v);
Expr*		Zlambda(Expr *args, Expr *body);
Expr*		Zlambdn(Expr *args, Expr *body, Expr *name, Expr *spec);
Expr*		Zlocals(unsigned n, ...);
Expr*		Znil();
Expr*		Zrange(Expr *addr, Expr *sz);
Expr*		Zref(Expr *dom, Expr *type, Expr *val);
Expr*		Zret(Expr *e);
Expr*		Zset(Expr *l, Expr *r);
Expr*		Zsizeof(Expr *e);
Expr*		Zstr(char *s);
Expr*		Zsub(Expr *x, Expr *y);
Expr*		Zuint(Imm val);
Expr*		Zxcast(Expr *type, Expr *cval);

extern		void fns(Env*);

#endif /* _BISONFLAW_SYSCQCT_H_ */
