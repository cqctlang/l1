#include "sys.h"
#include "util.h"

typedef struct Hent Hent;
struct Hent {
	char *key;
	unsigned keylen;
	void *val;
	Hent *next;
};

enum {
	INITSZ = 128,
};

struct HT {
	unsigned long sz;
	unsigned long nent;
	Hent **ht;		/* table */
	Hent **hent;		/* vector of allocated Hents */
};

static HT*
mkhtsz(unsigned long sz)
{
	HT *ht;
	ht = emalloc(sizeof(HT));
	ht->sz = sz;
	ht->ht = emalloc(ht->sz*sizeof(Hent*));
	ht->hent = emalloc(ht->sz*sizeof(Hent*));
	return ht;
}

HT*
mkht(void)
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
			efree(hp);
			hp = np;
		}
	}
	efree(ht->ht);
	efree(ht->hent);
	efree(ht);
}

u64
hsz(HT *ht)
{
	u64 m;
	unsigned long i;
	Hent *hp;

	m = 0;
	m += esize(ht);
	m += esize(ht->ht);
	m += esize(ht->hent);
	for(i = 0; i < ht->sz; i++){
		hp = ht->ht[i];
		while(hp){
			m += esize(hp);
			hp = hp->next;
		}
	}
	return m;
}

/* one-at-a-time by jenkins */
static unsigned long
shash(char *s, unsigned len)
{
	unsigned char *p = (unsigned char*)s;
	u32 h;

	h = 0;
	while(len != 0){
		h += *p;
		h += h<<10;
		h ^= h>>6;
		p++;
		len--;
	}
	h += h<<3;
	h ^= h>>11;
	h += h<<15;
	return (unsigned long)h;
}

static Hent*
_hget(HT *ht, char *k, unsigned len)
{
	Hent *hp;

	hp = ht->ht[shash(k, len)%ht->sz];
	while(hp){
		if(hp->keylen == len && memcmp(k, hp->key, len) == 0)
			return hp;
		hp = hp->next;
	}
	return 0;
}

void*
hget(HT *ht, char *k, unsigned len)
{
	Hent *hp;

	hp = _hget(ht, k, len);
	if(hp)
		return hp->val;
	return 0;
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
	nht = emalloc(nsz*sizeof(sizeof(Hent*)));
	for(i = 0; i < ht->sz; i++){
		hp = ht->ht[i];
		while(hp){
			nxt = hp->next;
			idx = shash(hp->key, hp->keylen)%nsz;
			hp->next = nht[idx];
			nht[idx] = hp;
			hp = nxt;
		}
	}
	efree(ht->ht);
	ht->ht = nht;
	ht->hent = erealloc(ht->hent,
			    ht->sz*sizeof(sizeof(Hent*)),
			    nsz*sizeof(sizeof(Hent*)));
	ht->sz = nsz;
}

void
hput(HT *ht, char *k, unsigned len, void *v)
{
	Hent *hp;
	unsigned long idx;

	hp = _hget(ht, k, len);
	if(hp){
		hp->val = v;
		return;
	}

	if(3*ht->nent > 2*ht->sz)
		hexpand(ht);

	hp = emalloc(sizeof(Hent));
	hp->key = k;
	hp->keylen = len;
	hp->val = v;
	idx = shash(k, len)%ht->sz;
	hp->next = ht->ht[idx];
	ht->ht[idx] = hp;
	ht->hent[ht->nent] = hp;
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

unsigned long
hnent(HT *ht)
{
	return ht->nent;
}

void*
hrefval(HT *ht, unsigned long idx)
{
	if(idx >= ht->nent)
		return 0;
	return ht->hent[idx]->val;
}

int
heq(HT *ha, HT *hb)
{
	Hent *hp;
	unsigned long i;
	void* v;

	if(hnent(ha) != hnent(hb))
		return 0;

	for(i = 0; i < ha->sz; ++i){
		hp = ha->ht[i];
		while(hp){
			v = hget(hb,hp->key,hp->keylen);
			if (v != hp->val)
				return 0;
			hp = hp->next;
		}
	}
	for(i = 0; i < hb->sz; ++i){
		hp = hb->ht[i];
		while(hp) {
			v = hget(ha,hp->key,hp->keylen);
			if (v != hp->val)
				return 0;
			hp = hp->next;
		}
	}
	return 1;
}
