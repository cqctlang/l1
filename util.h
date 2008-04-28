typedef struct HT HT;
HT *mkht();
void freeht(HT *ht);
void hput(HT *ht, char *k, void *v);
void *hget(HT *ht, char *k);
void hforeach(HT *ht, void (*f)(void *u, char *k, void *v), void *u);

void warn(char *fmt, ...);
void fatal(char *fmt, ...) __attribute__((noreturn));
void fatalno(char *fmt, ...) __attribute__((noreturn));
char* xstrdup(char *s);
void* xmalloc(size_t size);
void* xrealloc(void *p, size_t size);
void tvdiff(struct timeval *a, struct timeval *b, struct timeval *c);
