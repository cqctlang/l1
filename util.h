#ifndef _BISONFLAW_UTIL_H_
#define _BISONFLAW_UTIL_H_
typedef struct HT HT;
void		freeht(HT *ht);
HT*		mkhtp(void);
HT*		mkhts(void);
u32		hashp(void *p);
u32		hashs(char *s, unsigned len);
u32		hashu64(u64 key);
u32		hashx(u32 a, u32 b);
int		heqs(HT *ha, HT *hb);
void		hforeach(HT *ht, void (*f)(void *u, char *k, void *v), void *u);
void		hforeachp(HT *ht, void (*f)(void *u, void *k, void *v), void *u);
void		hdelp(HT *ht, void *k);
void		hdels(HT *ht, char *k, u32 len);
void*		hgetp(HT *ht, void *k);
void*		hgets(HT *ht, char *k, u32 len);
u32		hnent(HT *ht);
void		hputp(HT *ht, void *k, void *v);
void		hputs(HT *ht, char *k, u32 len, void *v);
u64		hsz(HT *ht);


char** copystrv(char **lp);
void bug() NORETURN;
void fatal(char *fmt, ...) NORETURN;
#define bug() fatal("bug at %s:%d", __FILE__, __LINE__)
char* xstrdup(char *s);
char* xstrndup(char *s, unsigned long len);
void* emalloc(size_t size);
void* erealloc(void *p, size_t old, size_t newsize);
void tvdiff(struct timeval *a, struct timeval *b, struct timeval *c);
char* strnchr(char *s, int c, unsigned long len);
void efree(void *p);
size_t esize(void *p);
size_t xstrnlen(char *s, size_t max);
unsigned max(unsigned a, unsigned b);  /* FIXME: cpv dependency! */

/* FIXME: use one or the other */
#define min(a,b)  ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

/* misplaced */
char* readfile(char *filename);
Imm xread(int fd, char *p, Imm len);
Imm xwrite(int fd, void *p, Imm len);
void xprintf(char *fmt, ...);
void xvprintf(char *fmt, va_list args);


int xisalnum(int c);
int xisalpha(int c);
int xisascii(int c);
int xisblank(int c);
int xiscntrl(int c);
int xisdigit(int c);
int xisgraph(int c);
int xislower(int c);
int xisodigit(int c);
int xisprint(int c);
int xispunct(int c);
int xisspace(int c);
int xisupper(int c);
int xisxdigit(int c);
int xtolower(int c);
int xtoupper(int c);

void xabort(void) NORETURN;

enum {
	PopenNoErr	= 1,	/* popen flag -- send stderr to /dev/null */
	PopenStderr	= 2,	/* popen flag -- leave stderr on stderr */
	PopenStdout	= 4,	/* popen flag -- leave stdout on stdout */
	PopenFullDuplex	= 8,	/* popen flag -- put i/o on same channel */
	PopenSession	= 16,	/* popen flag -- child leads a session */
	PopenStderrOnStdout = 32, /* popen flag -- stderr, stdout together */
};

int		newchan(int *l, int *r);
void		setproftimer(u32 usec, void(*fn)());
int		xioctl(int fd, unsigned long req, char *buf);
long		xlseek(int fd, long off, int whence);
int		xpopen(Imm argc, char **argv, unsigned flags, int *rfd);
int		xmunmap(void *p, Imm len);
Imm		usec(void);

extern u64 cqctmeminuse;
extern u64 cqctmemtotal;

#endif /* _BISONFLAW_UTIL_H_ */
