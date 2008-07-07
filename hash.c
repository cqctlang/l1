#include "sys.h"
#include "util.h"

typedef struct Hent Hent;
struct Hent {
	char *key;
	void *val;
	Hent *next;
};

enum {
	INITSZ = 128,
};

struct HT {
	unsigned long sz;
	unsigned long nent;
	Hent **ht;
};

static HT*
mkhtsz(unsigned long sz)
{
	HT *ht;
	ht = xmalloc(sizeof(HT));
	ht->sz = sz;
	ht->ht = xmalloc(ht->sz*sizeof(Hent*));
	return ht;
}

HT*
mkht()
{
	return mkhtsz(INITSZ);
}

void
freeht(HT* ht)
{
	Hent *hp, *np;
	unsigned long i;

	for(i = 0; i < ht->sz; i++){
		hp = ht->ht[i];
		while(hp){
			np = hp->next;
			free(hp);
			hp = np;
		}
	}
	free(ht->ht);
	free(ht);
}

/* one-at-a-time by jenkins */
static unsigned long
shash(char *s)
{
	unsigned char *p = (unsigned char*)s;
	uint32_t h;

	h = 0;
	while(*p){
		h += *p;
		h += h<<10;
		h ^= h>>6;
		p++;
	}
	h += h<<3;
	h ^= h>>11;
	h += h<<15;
	return (unsigned long)h;
}

#if 0
static unsigned long
shash(char* s)
{
	unsigned char *p = (unsigned char*)s;
	unsigned long h = 0;
	unsigned long m = 1;

	while(*p){
		h += *p * m;
		m *= 256;
		p++;
	}
	return h%SZ;
}
#endif

static Hent*
_hget(HT *ht, char *k)
{
	Hent *hp;

	hp = ht->ht[shash(k)%ht->sz];
	while(hp){
		if(strcmp(k, hp->key) == 0)
			return hp;
		hp = hp->next;
	}
	return NULL;
}

void*
hget(HT *ht, char *k)
{
	Hent *hp;

	hp = _hget(ht, k);
	if(hp)
		return hp->val;
	return NULL;
}

/* double hash table size and re-hash entries */
static void
hexpand(HT *ht)
{
	Hent **nht, *hp, *nxt;
	unsigned nsz;
	unsigned i;
	unsigned long idx;

	nsz = ht->sz*2;
	nht = xmalloc(nsz*sizeof(sizeof(Hent*)));
	for(i = 0; i < ht->sz; i++){
		hp = ht->ht[i];
		while(hp){
			nxt = hp->next;			
			idx = shash(hp->key)%nsz;
			hp->next = nht[idx];
			nht[idx] = hp;
			hp = nxt;
		}
	}
	free(ht->ht);
	ht->ht = nht;
	ht->sz = nsz;
}

void
hput(HT *ht, char *k, void *v)
{
	Hent *hp;
	unsigned long idx;

	hp = _hget(ht, k);
	if(hp){
		hp->val = v;
		return;
	}

	if(3*ht->nent > 2*ht->sz)
		hexpand(ht);

	hp = xmalloc(sizeof(Hent));
	hp->key = k;
	hp->val = v;
	idx = shash(k)%ht->sz;
	hp->next = ht->ht[idx];
	ht->ht[idx] = hp;
	ht->nent++;
}

void
hforeach(HT *ht, void (*f)(void *u, char *k, void *v), void *u)
{
	Hent *hp;
	unsigned long i;

	for(i = 0; i < ht->sz; i++){
		hp = ht->ht[i];
		while(hp){
			f(u, hp->key, hp->val);
			hp = hp->next;
		}
	}
}
