#ifndef CQCT_CQCT_H_
#define CQCT_CQCT_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

extern char cqctflags[];

typedef
enum {
	Qnil = 0,
	Qas,
	Qbox,
	Qcl,
	Qcid,
	Qcode,
	Qcont,
	Qctype,
	Qcval,
	Qdom,
	Qexpr,
	Qlist,
	Qns,
	Qpair,
	Qprecode,
	Qrange,
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

typedef struct Closure Closure;
typedef struct VM VM;
typedef struct Head Head;
typedef struct Head* Val;
typedef struct Tab* Env;

typedef struct Xfd Xfd;
struct Xfd {
	uint64_t (*read)(Xfd*, char*, uint64_t);
	uint64_t (*write)(Xfd*, char*, uint64_t);
	void (*close)(Xfd*);
	int fd;
};

#define CQCTFN(top,id)	cqctbuiltinfn(top, "%"#id, cqctmkcfn(#id, l1_##id))

void		cqctbuiltinfn(VM *vm, const char *name, Closure *cl);
int		cqctcallfn(VM *vm, Val cl, int argc, Val *argv, Val *rv);
int		cqctcallthunk(VM *vm, Val cl, Val *rv);
void		cqctcheckarg(VM *vm, Val *argv, unsigned arg, Qkind qkind);
Val		cqctcstrnval(const char *s, uint64_t len);
Val		cqctcstrnvalshared(char *s, uint64_t len);
Val		cqctcstrval(const char *s);
Val		cqctcstrvalshared(char *s);
void		cqctenvbind(VM *vm, const char *name, Val v);
Val		cqctenvlook(VM *vm, const char *name);
void		cqctenvdel(VM *vm, const char *name);
int		cqcteval(VM *vm, const char *s, const char *src, Val *rv);
void		cqctfreecstr(char *s);
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
Val		cqctlooktop(VM *vm, const char *name);
Closure*	cqctmkcfn(const char *id,
			  void (fn)(VM *vm, uint64_t argc, Val *argv, Val *rv));
/**
 * Create a closure with a C function body.
 *
 * The closure consists of a C function body with the following signature,
 * and zero or more bound Val arguments:
 *
 *   void closure(VM* vm, uint64_t argc, Val *argv, Val *disp, Val *rv)
 *
 * The bound arguments are the free variables that are "captured" by the
 * closure, and are available to the callback in the disp[placement] parameter,
 * in the order that they were provided to cqctmkccl. For example,
 *
 *     cqctmkccl("foo", &foo_impl, 2, cqctuint64val(0xf00), cqctuint64val(11));
 *
 * Creates an identifier 'foo' in the top-level environment that when invoked
 * within cqct execution will invoke foo_impl with
 *
 *     disp[] = { Val(0xf00), Val(11) }
 *
 * As with C functions registered with cqctmkcfn, it is up to the closure
 * implementation to verify that the expected _bound_ parameters are passed
 * in argv.
 *
 * @param id the identifier for the new closure
 * @param cl the C callback
 * @param dlen the number bound parameters
 */
Closure*	cqctmkccl(const char *id,
			  void (cl)(VM *vm, uint64_t argc, Val *argv, Val *disp, Val *rv),
			  unsigned dlen, ...);
Val		cqctmkfd(Xfd *xfd, const char *name);
Val		cqctmklist(uint64_t n);
Val		cqctmkrange(Val b, Val l);
Val		cqctmkvec(uint64_t n);
Val		cqctrangebeg(Val o);
Val		cqctrangelen(Val o);
char*		cqctsprintval(VM *vm, Val v);
Val		cqctmktab();
Val		cqcttabget(Val l, Val k);
int		cqcttabput(Val t, Val k, Val v);
Val		cqcttabenum(Val t);
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
const char*     cqcttypename(Qkind kind);
Val		cqctnil();
Val		cqctvalcar(Val);
Val		cqctvalcdr(Val);
Val		cqctmkpair(Val a, Val b);

/** Returns the type of a val. */
Qkind cqctkind(Val);

/**
 * Raise an error in the VM.
 *
 * This method signals an error condition in the VM, transferring control
 * to the error handler. It does not return.
 *
 * @param vm the vm
 * @param message the error message
 */
void		cqctvmerr(VM *vm, Val message);

/* mem.c */
Env		restoreheap(const char *file);
void	cqctgcdisable();
void	cqctgcenable();

/* vm.c */
VM*		cqctinit(const char *memfile, const char **loadpath);
VM*		cqctinitxfd(const char *memfile, const char **loadpath, Xfd *in, Xfd *out, Xfd *err);
void		cqctfini(VM *vm);

struct vm_options;

/**
 * Create and initialize the VM with the provided options.
 */
VM* cqctinitwithopt(const struct vm_options* options);

/**
 * Sets or clears a runtime flag.
 */
void cqctsetflag(unsigned char flag, int value);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* CQCT_CQCT_H_ */
