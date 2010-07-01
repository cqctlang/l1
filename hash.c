#include "sys.h"
#include "util.h"

typedef struct Hent Hent;
struct Hent {
	void *key, *val;
	u32 keylen;
	Hent *next;
};

enum {
	INITSZ = 128,
};

struct HT {
	u32 sz;
	u32 nent;
	Hent **ht;		/* table */
	Hent **hent;		/* vector of allocated Hents */
	u32 (*hash)(Hent *hp);
};

static u32 strhash(Hent *hp);
static u32 ptrhash(Hent *hp);

static HT*
mkhtsz(u32 sz)
{
	HT *ht;
	ht = emalloc(sizeof(HT));
	ht->sz = sz;
	ht->ht = emalloc(ht->sz*sizeof(Hent*));
	ht->hent = emalloc(ht->sz*sizeof(Hent*));
	return ht;
}

HT*
mkhts(void)
{
	HT *ht;
	ht = mkhtsz(INITSZ);
	ht->hash = strhash;
	return ht;
}

HT*
mkhtp(void)
{
	HT *ht;
	ht = mkhtsz(INITSZ);
	ht->hash = ptrhash;
	return ht;
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

/* double hash table size and re-hash entries */
static void
hexpand(HT *ht)
{
	Hent **nht, *hp, *nxt;
	u32 nsz, i, idx;

	nsz = ht->sz*2;
	nht = emalloc(nsz*sizeof(sizeof(Hent*)));
	for(i = 0; i < ht->sz; i++){
		hp = ht->ht[i];
		while(hp){
			nxt = hp->next;
			idx = ht->hash(hp)%nsz;
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

u32
hnent(HT *ht)
{
	return ht->nent;
}

static u32
phash(void *p)
{
	uintptr_t key;
	key = (uintptr_t)p;
	key = (~key) + (key << 18);
	key = key ^ (key >> 31);
	key = key * 21;
	key = key ^ (key >> 11);
	key = key + (key << 6);
	key = key ^ (key >> 22);
	return (u32)key;
}

static u32
ptrhash(Hent *hp)
{
	return phash(hp->key);
}

static Hent*
_hgetp(HT *ht, void *k)
{
	Hent *hp;

	hp = ht->ht[phash(k)%ht->sz];
	while(hp){
		if(hp->key == k)
			return hp;
		hp = hp->next;
	}
	return 0;
}

void*
hgetp(HT *ht, void *k)
{
	Hent *hp;

	hp = _hgetp(ht, k);
	if(hp)
		return hp->val;
	return 0;
}

void
hputp(HT *ht, void *k, void *v)
{
	Hent *hp;
	u32 idx;

	hp = _hgetp(ht, k);
	if(hp){
		hp->val = v;
		return;
	}

	if(3*ht->nent > 2*ht->sz)
		hexpand(ht);

	hp = emalloc(sizeof(Hent));
	hp->key = k;
	hp->val = v;
	idx = phash(k)%ht->sz;
	hp->next = ht->ht[idx];
	ht->ht[idx] = hp;
	ht->hent[ht->nent] = hp;
	ht->nent++;
}

void
hdelp(HT *ht, void *k)
{
	Hent *hp, **q;

	q = &ht->ht[phash(k)%ht->sz];
	hp = *q;
	while(hp){
		if(hp->key == k){
			*q = hp->next;
			efree(hp);
			break;
		}
		q = &hp->next;
		hp = hp->next;
	}
}

/* one-at-a-time by jenkins */
static u32
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

static u32
strhash(Hent *hp)
{
	return shash(hp->key, hp->keylen);
}

static Hent*
_hgets(HT *ht, char *k, u32 len)
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
hgets(HT *ht, char *k, u32 len)
{
	Hent *hp;

	hp = _hgets(ht, k, len);
	if(hp)
		return hp->val;
	return 0;
}

void
hputs(HT *ht, char *k, u32 len, void *v)
{
	Hent *hp;
	unsigned long idx;

	hp = _hgets(ht, k, len);
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
hdels(HT *ht, char *k, u32 len)
{
	Hent *hp, **q;

	q = &ht->ht[shash(k, len)%ht->sz];
	hp = *q;
	while(hp){
		if(hp->keylen == len && memcmp(k, hp->key, len) == 0){
			*q = hp->next;
			efree(hp);
			break;
		}
		q = &hp->next;
		hp = hp->next;
	}
}

int
heqs(HT *ha, HT *hb)
{
	Hent *hp;
	u32 i;
	void *v;

	if(hnent(ha) != hnent(hb))
		return 0;

	for(i = 0; i < ha->sz; i++){
		hp = ha->ht[i];
		while(hp){
			v = hgets(hb, hp->key, hp->keylen);
			if(v != hp->val)
				return 0;
			hp = hp->next;
		}
	}
	for(i = 0; i < hb->sz; i++){
		hp = hb->ht[i];
		while(hp){
			v = hgets(ha, hp->key, hp->keylen);
			if(v != hp->val)
				return 0;
			hp = hp->next;
		}
	}
	return 1;
}
