#ifndef _BISONFLAW_UTIL_H_
#define _BISONFLAW_UTIL_H_
typedef struct HT HT;
HT *mkht();
void freeht(HT *ht);
void hput(HT *ht, char *k, unsigned len, void *v);
void *hget(HT *ht, char *k, unsigned len);
void hforeach(HT *ht, void (*f)(void *u, char *k, void *v), void *u);
unsigned long hnent(HT *ht);
void* hrefval(HT *ht, unsigned long idx);


void warn(char *fmt, ...);
void fatal(char *fmt, ...) NORETURN;
char* xstrdup(char *s);
char* xstrndup(char *s, unsigned long len);
void* xmalloc(size_t size);
void* xrealloc(void *p, size_t old, size_t new);
void tvdiff(struct timeval *a, struct timeval *b, struct timeval *c);
char* strnchr(char *s, int c, unsigned long len);

typedef void(Faulthook)();
void setfaulthook(Faulthook *h);

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))

/* misplaced */
char* readfile(char *filename);
Imm xread(int fd, char *p, Imm len);
Imm xwrite(int fd, char *p, Imm len);

#endif /* _BISONFLAW_UTIL_H_ */
