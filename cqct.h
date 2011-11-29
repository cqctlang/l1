extern char cqctflags[];

typedef
enum {
	Qundef = 0,
	Qnil,
	Qas,
	Qbox,
	Qcl,
	Qcid,
	Qcode,
	Qctype,
	Qcval,
	Qdom,
	Qexpr,
	Qfd,
	Qlist,
	Qns,
	Qode,
	Qpair,
	Qrange,
	Qrd,
	Qrec,
	Qstr,
	Qtab,
	Qvec,
	Qnkind
} Qkind;


/* base C types */
typedef
enum Cbase {
	Vundef=0,
	Vlo,
	Vbool=Vlo,
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
	Vcomplex,
	Vdoublex,
	Vlongdoublex,
	Vnbase,
	Vptr = Vnbase,		/* alias for some other base type */
	Vvoid,
	Vnallbase,
} Cbase;

/* type representations */
typedef
enum Rkind {
	Rundef,
	Ru08le,
	Ru16le,
	Ru32le,
	Ru64le,
	Rs08le,
	Rs16le,
	Rs32le,
	Rs64le,
	Ru08be,
	Ru16be,
	Ru32be,
	Ru64be,
	Rs08be,
	Rs16be,
	Rs32be,
	Rs64be,
	Rf32,
	Rf64,
	Rf96,
	Rf128,
	Rx64,
	Rx128,
	Rx192,
	Rnrep,
} Rkind;

typedef struct Closure Closure;
typedef struct Toplevel Toplevel;
typedef struct VM VM;
typedef struct Head Head;
typedef struct Head* Val;

/*
         7 6 5 4 3 2 1 0
        |0|D|K K K K K|F|
 */

#define	Vfwdoff       0
#define	Vfwdbits      1
#define	Vfwdmask      ((1<<Vfwdbits)-1)
#define	Vkindoff      (Vfwdoff+Vfwdbits)
#define	Vkindbits     5
#define	Vkindmask     ((1<<Vkindbits)-1)
#define	Vdeadoff      (Vkindoff+Vkindbits)
#define	Vdeadbits     1
#define	Vdeadmask     ((1<<Vdeadbits)-1)

#define Vfwd(p)		  ((((p)->bits)>>Vfwdoff)&Vfwdmask)
#define Vsetfwd(p, a)     ((p)->bits = a|(Vfwdmask<<Vfwdoff))
#define Vfwdaddr(p)	  ((void*)((p)->bits & ~(Vfwdmask<<Vfwdoff)))

#define Vkind(p)          ((((p)->bits)>>Vkindoff)&Vkindmask)
#define Vsetkind(p, v)	  ((p)->bits = ((p)->bits&~(Vkindmask<<Vkindoff))|(((v)&Vkindmask)<<Vkindoff))

#define Vdead(p)          ((((p)->bits)>>Vdeadoff)&Vdeadmask)
#define Vsetdead(p, v)    ((p)->bits = ((p)->bits&~(Vdeadmask<<Vdeadoff))|(((v)&Vdeadmask)<<Vdeadoff))

struct Head
{
	uintptr_t bits; // must be able to store a forwarding pointing
};

typedef struct Xfd Xfd;
struct Xfd {
	uint64_t (*read)(Xfd*, char*, uint64_t);
	uint64_t (*write)(Xfd*, char*, uint64_t);
	void (*close)(Xfd*);
	int fd;
};

#define CQCTFN(top,id)	cqctbuiltinfn(top, "%"#id, cqctmkcfn(#id, l1_##id))

void		cqctbuiltinfn(Toplevel *top, char *name, Closure *cl);
int		cqctcallfn(VM *vm, Val cl, int argc, Val *argv, Val *rv);
int		cqctcallthunk(VM *vm, Val cl, Val *rv);
void		cqctcheckarg(VM *vm, char *fn, Val *argv,
			     unsigned arg, Qkind qkind);
Val		cqctcompile(VM *vm, char *s, char *src,
			    Toplevel *top, char *argsid);
Val		cqctcstrnval(char *s, uint64_t len);
Val		cqctcstrnvalshared(char *s, uint64_t len);
Val		cqctcstrval(char *s);
Val		cqctcstrvalshared(char *s);
void		cqctenvbind(Toplevel *top, char *name, Val v);
Val		cqctenvlook(Toplevel *top, char *name);
int		cqcteval(VM *vm, char *s, char *src, Val *rv);
int		cqctfaulthook(void (*h)(void), int in);
void		cqctfini(Toplevel *top);
void		cqctfreecstr(char *s);
void		cqctfreevm(VM *vm);
void		cqctgcdisable(VM *vm);
void		cqctgcenable(VM *vm);
Val		cqctgcpersist(VM *vm, Val v);
Val		cqctgcprotect(VM *vm, Val v);
Val		cqctgcunpersist(VM *vm, Val v);
Val		cqctgcunprotect(VM *vm, Val v);
Toplevel*	cqctinit(char **lp, Xfd *in, Xfd *out, Xfd *err);
Val		cqctint8val(int8_t);
Val		cqctint16val(int16_t);
Val		cqctint32val(int32_t);
Val		cqctint64val(int64_t);
void		cqctinterrupt(VM *vm);
uint64_t	cqctlength(Val v);
Val		cqctlistappend(Val l, Val v);
Val		cqctlistref(Val l, uint64_t idx);
Val		cqctlistset(Val l, uint64_t idx, Val v);
Val*		cqctlistvals(Val v);
Closure*	cqctmkcfn(char *id,
			  void (fn)(VM *vm, uint64_t argc, Val *argv, Val *rv));
Val		cqctmkfd(Xfd *xfd, char *name);
Val		cqctmklist(uint64_t n);
Val		cqctmkrange(Val b, Val l);
Val		cqctmkvec(uint64_t n);
VM*		cqctmkvm(Toplevel *top);
Val		cqctrangebeg(Val o);
Val		cqctrangelen(Val o);
char*		cqctsprintval(VM *vm, Val v);
Val		cqctuint8val(uint8_t);
Val		cqctuint16val(uint16_t);
Val		cqctuint32val(uint32_t);
Val		cqctuint64val(uint64_t);
Cbase		cqctvalcbase(Val);
int8_t		cqctvalint8(Val);
int16_t		cqctvalint16(Val);
int32_t		cqctvalint32(Val);
int64_t		cqctvalint64(Val);
char*		cqctvalcstr(Val);
uint64_t	cqctvalcstrlen(Val);
char*		cqctvalcstrshared(Val);
uint8_t		cqctvaluint8(Val);
uint16_t	cqctvaluint16(Val);
uint32_t	cqctvaluint32(Val);
uint64_t	cqctvaluint64(Val);
Val		cqctvecref(Val l, uint64_t idx);
Val		cqctvecset(Val l, uint64_t idx, Val v);
Val*		cqctvecvals(Val v);
void		cqctvmerr(VM *vm, char *fmt, ...);
