enum {
	Fcompile	= 0x01,
	Fexec		= 0x02,
	Fprintir	= 0x04,
	Fprintobj	= 0x08,
	Ftime		= 0x10,
};

enum{
	Vintmax		= 2147483647ULL,
	Vuintmax	= 4294967295ULL,
	Vlongmax	= 2147483647ULL,
	Vulongmax	= 4294967295ULL,
	Vvlongmax	= 9223372036854775807ULL,
	Vuvlongmax	= 18446744073709551615ULL,
};

typedef unsigned long long Imm;

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

	Eadd=0,
	Earef,
	Earr,
	Earrow,
	Eband,
	Ebinop,
	Ebits,
	Eblock,
	Ebor,
	Ebreak,
	Ebxor,
	Ecall,
	Ecast,
	Ecomma,
	Econd,
	Econst,
	Econsts,
	Econtinue,
	Edecl,
	Edecls,
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
	Egop,
	Ege,
	Egt,
	Eid,
	Eif,
	Elambda,
	Eland,
	Ele,
	Elor,
	Elt,
	Emod,
	Emul,
	Eneq,
	Enop,
	Enull,
	Epostdec,
	Epostinc,
	Epredec,
	Epreinc,
	Eptr,
	Eret,
	Eshl,
	Eshr,
	Esizeofe,
	Esizeoft,
	Estruct,
	Esub,
	Etick,
	Etypedef,
	Euand,
	Euminus,
	Eumul,
	Eunion,
	Eunot,
	Euplus,
	Eutwiddle,
	Ewhile,
} Kind;

enum{
	Tbase=0,
	Tstruct,
	Tunion,
	Tenum,
	Tptr,
	Tarr,
	Tfun,
	Ttypedef,

	/* Type flags */
	Ffwd=(1<<0),

	Verr=0,
	Vchar,
	Vshort,
	Vint,
	Vlong,
	Vvlong,
	Vuchar,
	Vushort,
	Vuint,
	Vulong,
	Vuvlong,
	Vfloat,
	Vdouble,
	Vlongdouble,
	Vvoid,
	Vnil,        
};

typedef struct Expr Expr;
typedef struct Decl Decl;
typedef struct Enum Enum;
typedef struct Type Type;
typedef struct Cval Cval;

struct Cval {
	Type *type;
	Imm val;
};

typedef
struct Src {
	char *filename;
	unsigned line;
} Src;

typedef
struct Lits {
	char *s;
	unsigned len;
} Lits;

struct Expr {
	Kind kind;

	char *id;		/* Eid */
	Lits *lits;		/* Econsts, Etick */
	Cval cval;		/* Econst */
	Kind op;		/* Ebinop, Egop */

	Expr *e1;
	Expr *e2;
	Expr *e3;
	Expr *e4;

	/* source */
	Src src;

	/* compiler-managed fields */
	void *x;
};

struct Enum {
	char *id;
	Expr *val;
	Type *type;
	Enum *link;
};

struct Decl {
	Type *type;
	char *id;
	Expr *offs;		/* offset */
	Expr *bits;		/* bit field size */
	Decl *link;
};

struct Type {
	unsigned kind;
	unsigned base;		/* base, enum? */
	char *tid;		/* typedef */
	char *tag;		/* struct, union, enum */
	Decl *field;		/* struct, union */
	Enum *en;		/* enum */
	Expr *sz;		/* struct, union */
	Decl *param;		/* func */
	Expr *cnt;		/* arr */
	Type *link;		/* typedef, ptr, arr, func(return type) */
	unsigned char flags;	/* Ffwd, ... */
};

enum {
	MaxIn	= 128,
};

typedef
struct NS {
	HT* tagtab;
	HT* tidtab;
	HT* symtab;
	HT* enctab;
} NS;

typedef struct YYstate YYstate;

typedef
struct In {
	char *filename;
	FILE *fp;
	unsigned col, line;
	YYstate *yy;
} In;

typedef
struct U {
	jmp_buf jmp;
	In in[MaxIn];
	In *inp;
	NS* ns;
	Expr *el;
} U;

extern U ctx;
extern char* S[];
extern char *stdinname;
extern Imm basesize[];
extern Imm ptrsize;

YYstate* mkyystate(FILE *fp);
void freeyystate(YYstate *yy);
void setyystate(YYstate *yy);

int yyparse();
Expr* newexpr(unsigned, Expr*, Expr*, Expr*, Expr*);
Expr* newbinop(unsigned, Expr*, Expr*);
Expr* newgop(unsigned, Expr*, Expr*);
Expr* mkconst(unsigned type, Imm val); /* rename newconst? */
void freeexpr(Expr*, void(*xfn)(Expr*));
Expr* invert(Expr*);
Expr* nullelist();
Expr* ptrto(Expr*, Expr*);
Expr* doid(char*);
Expr* doconst(char*);
Expr* doconsts(char*);
Expr* dotick(char*);
Lits* copylits(Lits *lits);
void freelits(Lits *lits);
char* fmttype(Type *t, char *o);

void initcval(Cval *cval, Type *type, Imm val);

void dotop(Expr*);
void initparse();
void finiparse();

void printexpr(Expr*);

Type* taglookup(NS *ns, char *tag);
Type* tidlookup(NS *ns, char *tid);
NS* mkns();
void freens();
void pushyy(char *filename);
int popyy();
void tryinclude(NS *ns, char *raw);
void parseerror(char *fmt, ...);
int doparse(NS *ns, char *filename);
